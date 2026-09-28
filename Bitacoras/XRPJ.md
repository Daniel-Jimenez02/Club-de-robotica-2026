# #############################################################################
# XRP ROBOTICADOS - Control por Bluetooth con la app "XRP Controller"
# #############################################################################
from XRPLib.defaults import drivetrain, servo_one
import bluetooth
import time

NOMBRE = "XRP-Valeria"      # debe coincidir con la app

# ---------------- CALIBRACIÓN REAL DE TU SERVO ----------------
SERVO_CENTRO = 94               # Punto neutro exacto (el servo no se mueve)
SERVO_VELOCIDAD = 10            # 104° para horario / 84° para antihorario
MS_POR_GRADO_HORARIO = 50       # Milisegundos por grado en sentido horario
MS_POR_GRADO_ANTIHORARIO = 50   # Milisegundos por grado en sentido antihorario


# ---------------- RUTINA AUTÓNOMA (botón AUTO, máximo 30 s) ----------------
def rutina_autonoma():
    mover(50, 50, 2000)      # adelante 2 s  (izquierda, derecha, milisegundos)
    mover(40, -40, 700)      # girar a la derecha
    servo(45)                # servo 45 grados horario
    servo(-45)               # servo 45 grados antihorario
    mover(-50, -50, 2000)    # atrás 2 s
    mover(0, 0, 500)         # parar


# ---------------- ESTADO ----------------
izq = der = 0.0              # potencia que pide la app (-1 a 1)
act_izq = act_der = 0.0      # potencia actual de los motores (con arranque suave)
ultimo = time.ticks_ms()     # cuándo llegó el último comando
servo_pendiente = 0          # grados que faltan por girar
servo_pos = 0                # grados girados desde el inicio
auto_pedido = False
en_auto = False
auto_inicio = 0
cancelar = False


def detener_servo_de_golpe():
    """Lleva el servo a 94° e inmediatamente corta la señal PWM."""
    global servo_pos
    servo_one.set_angle(SERVO_CENTRO)  # Pulso en neutro real (94°)
    time.sleep_ms(50)                   # Margen de estabilización
    servo_one.free()                   # Cortar señal completamente
    servo_pos = 0                      # Reiniciar conteo de posición
    print("SERVO: Detenido de golpe.")


def girar_servo(grados):
    """Mueve el servo en la dirección indicada y se frena inmediatamente al terminar."""
    global servo_pos
    if grados == 0:
        detener_servo_de_golpe()
        return

    if grados > 0:
        # Giro horario (94 + 10 = 104)
        angulo = SERVO_CENTRO + SERVO_VELOCIDAD
        tiempo = int(grados * MS_POR_GRADO_HORARIO)
    else:
        # Giro antihorario (94 - 10 = 84)
        angulo = SERVO_CENTRO - SERVO_VELOCIDAD
        tiempo = int(-grados * MS_POR_GRADO_ANTIHORARIO)

    # Iniciar movimiento
    servo_one.set_angle(angulo)
    time.sleep_ms(tiempo)

    # Detener de golpe al cumplir el tiempo
    detener_servo_de_golpe()


# ---------------- AYUDAS PARA LA RUTINA AUTÓNOMA ----------------
class FinAutonomo(Exception):
    pass


def esperar(ms):
    """Espera, pero termina si la app pide parar o se cumplen los 30 s."""
    fin = time.ticks_add(time.ticks_ms(), ms)
    while time.ticks_diff(fin, time.ticks_ms()) > 0:
        if cancelar or time.ticks_diff(time.ticks_ms(), auto_inicio) > 30000:
            raise FinAutonomo()
        time.sleep_ms(10)


def mover(izquierda, derecha, ms):
    drivetrain.set_effort(izquierda / 100, derecha / 100)
    esperar(ms)


def servo(grados):
    girar_servo(grados)


def ejecutar_autonomo():
    global en_auto, auto_inicio, cancelar, izq, der, act_izq, act_der
    print("AUTÓNOMO: inicio")
    izq = der = act_izq = act_der = 0.0
    en_auto = True
    cancelar = False
    auto_inicio = time.ticks_ms()
    try:
        rutina_autonoma()
    except FinAutonomo:
        pass
    drivetrain.stop()
    detener_servo_de_golpe()
    en_auto = False
    print("AUTÓNOMO: fin")


# ---------------- COMANDOS QUE LLEGAN DE LA APP ----------------
def procesar(texto):
    global izq, der, act_izq, act_der, ultimo, servo_pendiente, auto_pedido, cancelar
    ultimo = time.ticks_ms()
    texto = texto.strip().upper()
    try:
        if texto == "STOP":                     # Parar todo (motores, servo y autónomo)
            cancelar = True
            izq = der = act_izq = act_der = 0.0
            drivetrain.stop()
            detener_servo_de_golpe()
        elif texto == "AUTO:START":
            auto_pedido = True
        elif texto == "AUTO:STOP":
            cancelar = True
        elif texto.startswith("SERVO_PASO:"):   # Botones de dirección "+1" o "-1"
            servo_pendiente += int(texto[11:])
        elif texto == "SERVO:0":                # BOTÓN DE RESET/CENTRO: Detener de golpe
            servo_pendiente = 0                 # Cancela cualquier movimiento en cola
            detener_servo_de_golpe()
        elif texto.startswith("L:") and not en_auto:   # "L:50,R:-20"
            l, r = texto.split(",")
            izq = int(l[2:]) / 100
            der = int(r[2:]) / 100
    except ValueError:
        print("Comando inválido:", texto)


def evento_ble(evento, datos):
    global izq, der, act_izq, act_der
    if evento == 1:
        print("Conectado")
    elif evento == 2:
        print("Desconectado")
        if not en_auto:
            izq = der = act_izq = act_der = 0.0
            drivetrain.stop()
            detener_servo_de_golpe()
        anunciar()
    elif evento == 3:
        procesar(ble.gatts_read(rx).decode())


def anunciar():
    nombre = NOMBRE.encode()
    anuncio = bytes((2, 0x01, 0x06, len(nombre) + 1, 0x09)) + nombre
    respuesta = bytes((17, 0x07)) + bytes(SERVICIO)
    ble.gap_advertise(100000, adv_data=anuncio, resp_data=respuesta)
    print("Esperando la app como", NOMBRE)


# ---------------- BLUETOOTH ----------------
SERVICIO = bluetooth.UUID("6E400001-B5A3-F393-E0A9-E50E24DCCA9E")
RX_UUID = bluetooth.UUID("6E400002-B5A3-F393-E0A9-E50E24DCCA9E")
TX_UUID = bluetooth.UUID("6E400003-B5A3-F393-E0A9-E50E24DCCA9E")

ble = bluetooth.BLE()
ble.active(True)
ble.irq(evento_ble)
((tx, rx),) = ble.gatts_register_services(
    ((SERVICIO, ((TX_UUID, bluetooth.FLAG_NOTIFY), (RX_UUID, bluetooth.FLAG_WRITE))),)
)
ble.gatts_set_buffer(rx, 100, False)
anunciar()
detener_servo_de_golpe()

# ---------------- PROGRAMA PRINCIPAL ----------------
PASO = 0.05     # arranque suave: de 0 a 100 % en 0.4 s

try:
    while True:
        if auto_pedido:
            auto_pedido = False
            ejecutar_autonomo()

        if servo_pendiente != 0:
            grados = servo_pendiente
            servo_pendiente = 0        # Limpia la cola inmediatamente
            girar_servo(grados)

        # Seguridad: si la app deja de enviar datos por 1 s, parar
        if time.ticks_diff(time.ticks_ms(), ultimo) > 1000:
            izq = der = 0.0

        act_izq += max(-PASO, min(PASO, izq - act_izq))
        act_der += max(-PASO, min(PASO, der - act_der))
        drivetrain.set_effort(act_izq, act_der)
        time.sleep_ms(20)
finally:
    drivetrain.stop()
    detener_servo_de_golpe()
    print("Programa detenido")

