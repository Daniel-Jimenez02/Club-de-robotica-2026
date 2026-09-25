# ============================================================
# XRP ROBOTICADOS - Control por Bluetooth (BLE)
# Para usar con la app "XRP Controller".
#
# Cómo usarlo en Thonny:
#   - Probar:     abrir este archivo y pulsar Ejecutar (F5).
#   - Arranque automático al encender el robot:
#                 Archivo > Guardar como... > Raspberry Pi Pico
#                 y guardarlo con el nombre  main.py
#
# Protocolo (el mismo que usa la app):
#   Nombre BLE : XRP-ROBOTICADOS
#   Servicio   : 6E400001-B5A3-F393-E0A9-E50E24DCCA9E  (Nordic UART)
#   RX         : 6E400002-...  la app escribe aquí
#   TX         : 6E400003-...  el robot responde aquí ("OK ...")
#   Comandos   : "L:<-100..100>,R:<-100..100>"   potencia de cada motor
#                "STOP"                           parar ya
#
# Qué hace además de mover los motores:
#   - Rampa de aceleración: los cambios de velocidad y los giros empiezan
#     suave y llegan a la velocidad pedida en unas décimas de segundo.
#   - STOP, soltar el joystick y la desconexión frenan AL INSTANTE.
#   - Watchdog: si deja de llegar señal de la app durante 1 s, se detiene.
#   - Responde "OK L:x,R:y" para que la app confirme que llegó.
#   - LED: parpadea esperando conexión, fijo cuando está conectado.
# ============================================================

from XRPLib.defaults import drivetrain, board
import bluetooth
import time
from micropython import const


# ============================================================
# AJUSTES (puedes cambiarlos)
# ============================================================

DEVICE_NAME = "XRP-ROBOTICADOS"   # Debe coincidir con la app

# Rampa (unidades de "effort" por segundo; 1.0 = 100 %)
ACELERACION = 2.5    # 0 -> 100 % en 0.4 s. Más alto = más brusco
FRENADO = 5.0        # 100 % -> 0 en 0.2 s al reducir velocidad

# Potencia mínima para que las ruedas realmente giren.
# Con muy poca potencia los motores del XRP zumban pero no se mueven,
# así que los valores pequeños (giros lentos) se suben a este mínimo.
# Pon 0.0 para desactivarlo. Súbelo si a baja velocidad no se mueve.
POTENCIA_MINIMA = 0.12

WATCHDOG_MS = const(1000)   # sin comandos durante este tiempo -> STOP
CICLO_MS = const(20)        # control de motores 50 veces por segundo

MOSTRAR_COMANDOS = True     # imprimir en Thonny cada comando recibido


# ============================================================
# BLE: UUIDs y servicio
# ============================================================

_UART_SERVICE_UUID = bluetooth.UUID("6E400001-B5A3-F393-E0A9-E50E24DCCA9E")
_UART_RX_UUID = bluetooth.UUID("6E400002-B5A3-F393-E0A9-E50E24DCCA9E")
_UART_TX_UUID = bluetooth.UUID("6E400003-B5A3-F393-E0A9-E50E24DCCA9E")

_UART_SERVICE = (
    _UART_SERVICE_UUID,
    (
        (_UART_TX_UUID, bluetooth.FLAG_READ | bluetooth.FLAG_NOTIFY),
        (_UART_RX_UUID, bluetooth.FLAG_WRITE),
    ),
)

_IRQ_CENTRAL_CONNECT = const(1)
_IRQ_CENTRAL_DISCONNECT = const(2)
_IRQ_GATTS_WRITE = const(3)


def _campo(tipo, valor):
    return bytes((len(valor) + 1, tipo)) + valor


def _paquetes_anuncio():
    # Un anuncio BLE admite máximo 31 bytes.
    # Anuncio: flags (3) + nombre (2 + 15) = 20 bytes
    anuncio = _campo(0x01, b"\x06") + _campo(0x09, DEVICE_NAME.encode())
    # Respuesta de escaneo: UUID de 128 bits (2 + 16) = 18 bytes
    respuesta = _campo(0x07, bytes(_UART_SERVICE_UUID))
    if len(anuncio) > 31 or len(respuesta) > 31:
        raise ValueError("Anuncio BLE demasiado largo: acorta DEVICE_NAME")
    return anuncio, respuesta


# ============================================================
# CLASE BLE
# Las interrupciones sólo guardan datos; el trabajo con los motores
# se hace en el bucle principal.
# ============================================================

class RobotBLE:

    def __init__(self):
        self._ble = bluetooth.BLE()
        self._ble.active(True)
        self._ble.irq(self._irq)

        ((self._tx, self._rx),) = self._ble.gatts_register_services((_UART_SERVICE,))

        # append=False: cada comando reemplaza al anterior (nunca se mezclan)
        self._ble.gatts_set_buffer(self._rx, 100, False)

        self._anuncio, self._respuesta = _paquetes_anuncio()

        self.conexion = None        # conn_handle del teléfono, o None
        self.comando = None         # último texto recibido
        self.comando_n = 0          # contador: cambia con cada comando nuevo
        self.desconectado = False   # aviso para el bucle principal

        self.anunciar()

    def _irq(self, evento, datos):
        if evento == _IRQ_CENTRAL_CONNECT:
            conn, _, _ = datos
            self.conexion = conn
            print(">> Bluetooth CONECTADO")

        elif evento == _IRQ_CENTRAL_DISCONNECT:
            conn, _, _ = datos
            if conn == self.conexion:
                self.conexion = None
            # Seguridad: frenar ya, sin esperar al bucle
            drivetrain.stop()
            self.desconectado = True
            print(">> Bluetooth DESCONECTADO (motores detenidos)")
            self.anunciar()

        elif evento == _IRQ_GATTS_WRITE:
            conn, handle = datos
            if handle == self._rx:
                self.comando = self._ble.gatts_read(self._rx)
                self.comando_n += 1

    def anunciar(self):
        self._ble.gap_advertise(100000, adv_data=self._anuncio, resp_data=self._respuesta)
        print("Anunciándose como", DEVICE_NAME, "- esperando la app...")

    def responder(self, texto):
        if self.conexion is None:
            return
        try:
            self._ble.gatts_notify(self.conexion, self._tx, texto.encode())
        except OSError as e:
            print("No se pudo responder a la app:", e)


# ============================================================
# COMANDOS
# Devuelve ("STOP", None), ("MOTORES", (izq, der)) o ("ERROR", texto)
# ============================================================

def interpretar(datos):
    try:
        texto = datos.decode().strip().upper()
    except UnicodeError:
        return ("ERROR", "texto no valido")

    # Si llegaran varias líneas juntas, vale la última
    lineas = [l.strip() for l in texto.split("\n") if l.strip()]
    if not lineas:
        return ("ERROR", "vacio")
    texto = lineas[-1]

    if texto == "STOP":
        return ("STOP", None)

    izquierda = None
    derecha = None
    for parte in texto.split(","):
        parte = parte.strip()
        try:
            if parte.startswith("L:"):
                izquierda = int(parte[2:])
            elif parte.startswith("R:"):
                derecha = int(parte[2:])
        except ValueError:
            return ("ERROR", "numero no valido: " + parte)

    if izquierda is None or derecha is None:
        return ("ERROR", "comando desconocido: " + texto)

    izquierda = max(-100, min(100, izquierda))
    derecha = max(-100, min(100, derecha))
    return ("MOTORES", (izquierda, derecha))


# ============================================================
# RAMPA Y POTENCIA
# ============================================================

def acercar(actual, objetivo, dt):
    """Mueve 'actual' hacia 'objetivo' sin superar la aceleración permitida.
    Acelera suave; reduce y cambia de sentido más rápido (más seguro)."""
    if actual == objetivo:
        return objetivo

    mismo_sentido = actual == 0 or (actual > 0) == (objetivo > 0)
    acelerando = mismo_sentido and abs(objetivo) > abs(actual)
    paso = (ACELERACION if acelerando else FRENADO) * dt

    if objetivo > actual:
        return min(actual + paso, objetivo)
    return max(actual - paso, objetivo)


def potencia_real(effort):
    """Aplica la potencia mínima: 0 se queda en 0; lo demás se reparte
    entre POTENCIA_MINIMA y 1.0 manteniendo el sentido."""
    if abs(effort) < 0.005:
        return 0.0
    signo = 1 if effort > 0 else -1
    return signo * (POTENCIA_MINIMA + (1.0 - POTENCIA_MINIMA) * abs(effort))


# ============================================================
# LED DE ESTADO
# ============================================================

_led_ok = hasattr(board, "led_on") and hasattr(board, "led_off") and hasattr(board, "led_blink")
if not _led_ok:
    print("Aviso: esta versión de XRPLib no tiene control de LED; se omite.")

_led_estado = None


def led(conectado):
    global _led_estado, _led_ok
    if not _led_ok or conectado == _led_estado:
        return
    _led_estado = conectado
    try:
        if conectado:
            board.led_blink(0)   # detiene el parpadeo
            board.led_on()
        else:
            board.led_blink(2)   # parpadeo lento: esperando conexión
    except Exception as e:
        # El LED es sólo informativo: si esta XRPLib no lo soporta igual,
        # se avisa una vez y se sigue sin LED (los motores no dependen de él).
        _led_ok = False
        print("Aviso: LED desactivado ->", e)


# ============================================================
# PROGRAMA PRINCIPAL
# ============================================================

def main():
    robot = RobotBLE()

    objetivo_izq = 0.0   # lo que pide la app (-1.0 .. 1.0)
    objetivo_der = 0.0
    actual_izq = 0.0     # lo que tienen los motores ahora (con rampa)
    actual_der = 0.0

    ultimo_n = 0
    ultimo_comando = time.ticks_ms()
    anterior = time.ticks_ms()

    print("")
    print("XRP LISTA - abre la app, elige XRP REAL y pulsa Conectar")
    print("")

    try:
        while True:
            ahora = time.ticks_ms()
            dt = time.ticks_diff(ahora, anterior) / 1000
            anterior = ahora

            led(robot.conexion is not None)

            # --- Desconexión: la IRQ ya frenó; reiniciar el estado ---
            if robot.desconectado:
                robot.desconectado = False
                objetivo_izq = objetivo_der = 0.0
                actual_izq = actual_der = 0.0

            # --- Comando nuevo ---
            if robot.comando_n != ultimo_n:
                ultimo_n = robot.comando_n
                datos = robot.comando
                ultimo_comando = ahora

                tipo, valor = interpretar(datos)

                if tipo == "STOP":
                    # STOP no usa rampa: freno inmediato
                    objetivo_izq = objetivo_der = 0.0
                    actual_izq = actual_der = 0.0
                    drivetrain.stop()
                    robot.responder("OK STOP")
                    if MOSTRAR_COMANDOS:
                        print("STOP")

                elif tipo == "MOTORES":
                    izq, der = valor
                    objetivo_izq = izq / 100
                    objetivo_der = der / 100
                    robot.responder("OK L:{},R:{}".format(izq, der))
                    if MOSTRAR_COMANDOS:
                        print("Motores  L:{:4d}%   R:{:4d}%".format(izq, der))

                else:
                    # Comando inválido: parar por seguridad
                    objetivo_izq = objetivo_der = 0.0
                    actual_izq = actual_der = 0.0
                    drivetrain.stop()
                    robot.responder("ERR " + valor)
                    print("Comando inválido ->", valor)

            # --- Watchdog: sin señal de la app mientras se mueve ---
            moviendo = objetivo_izq != 0 or objetivo_der != 0 or actual_izq != 0 or actual_der != 0
            if moviendo and time.ticks_diff(ahora, ultimo_comando) > WATCHDOG_MS:
                objetivo_izq = objetivo_der = 0.0
                actual_izq = actual_der = 0.0
                drivetrain.stop()
                print("WATCHDOG: sin comandos de la app, motores detenidos")

            # --- Rampa y salida a motores ---
            if moviendo:
                actual_izq = acercar(actual_izq, objetivo_izq, dt)
                actual_der = acercar(actual_der, objetivo_der, dt)
                if actual_izq == 0 and actual_der == 0:
                    drivetrain.stop()
                else:
                    drivetrain.set_effort(potencia_real(actual_izq), potencia_real(actual_der))

            time.sleep_ms(CICLO_MS)

    finally:
        # Al pulsar Detener en Thonny (Ctrl+C) o si hay un error
        drivetrain.stop()
        print("Programa detenido: motores parados")


main()


