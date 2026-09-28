# XRP ROBOTICADOS - control por Bluetooth con la app "XRP Controller"
from XRPLib.defaults import drivetrain, servo_one
import bluetooth
import time

NOMBRE = "XRP-ROBOTICADOS"      # debe coincidir con la app

# Servo 360 en el puerto Servo 1 (calibrar con calibrar_servo.py y prueba_sentidos.py)
SERVO_CENTRO = 100              # valor en el que el servo queda quieto
SERVO_VELOCIDAD = 10            # lento = más preciso (ideal ~20 grados por segundo)
MS_POR_GRADO_HORARIO = 50       # ms que tarda 1 grado en sentido horario
MS_POR_GRADO_ANTIHORARIO = 50   # ms que tarda 1 grado en sentido antihorario


# ---------------- RUTINA AUTÓNOMA (botón AUTO, máximo 30 s) ----------------
def rutina_autonoma():
    mover(50, 50, 2000)      # adelante 2 s  (izquierda, derecha, milisegundos)
    mover(40, -40, 700)      # girar a la derecha
    servo(45)                # servo 45 grados horario
    servo(-45)               # servo 45 grados antihorario
    mover(-50, -50, 2000)    # atrás 2 s
    mover(0, 0, 500)         # parar


# ---------------- estado ----------------
izq = der = 0.0              # potencia que pide la app (-1 a 1)
act_izq = act_der = 0.0      # potencia actual de los motores (con arranque suave)
ultimo = time.ticks_ms()     # cuándo llegó el último comando
servo_pendiente = 0          # grados que faltan por girar
servo_pos = 0                # grados girados desde el inicio
auto_pedido = False
en_auto = False
auto_inicio = 0
cancelar = False


def girar_servo(grados):
    """Gira el servo esos grados (+ horario, - antihorario) y lo apaga."""
    global servo_pos
    if grados > 0:
        servo_one.set_angle(SERVO_CENTRO + SERVO_VELOCIDAD)
        time.sleep_ms(grados * MS_POR_GRADO_HORARIO)
    elif grados < 0:
        servo_one.set_angle(SERVO_CENTRO - SERVO_VELOCIDAD)
        time.sleep_ms(-grados * MS_POR_GRADO_ANTIHORARIO)
    servo_one.free()             # sin señal: el servo se queda quieto
    servo_pos += grados


# ---------------- ayudas para la rutina autónoma ----------------
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
    izq = der = act_izq = act_der = 0.0      # al terminar no sigue lo del joystick
    en_auto = True
    cancelar = False
    auto_inicio = time.ticks_ms()
    try:
        rutina_autonoma()
    except FinAutonomo:
        pass
    drivetrain.stop()
    girar_servo(-servo_pos)      # el servo vuelve a donde empezó
    en_auto = False
    print("AUTÓNOMO: fin")


# ---------------- comandos que llegan de la app ----------------
def procesar(texto):
    global izq, der, act_izq, act_der, ultimo, servo_pendiente, auto_pedido, cancelar
    ultimo = time.ticks_ms()
    texto = texto.strip().upper()
    try:
        if texto == "STOP":                     # parar ya (también el autónomo)
            cancelar = True
            izq = der = act_izq = act_der = 0.0
            drivetrain.stop()
        elif texto == "AUTO:START":
            auto_pedido = True
        elif texto == "AUTO:STOP":
            cancelar = True
        elif texto.startswith("SERVO_PASO:"):   # "SERVO_PASO:+1" o "SERVO_PASO:-1"
            servo_pendiente += int(texto[11:])
        elif texto == "SERVO:0":                # volver a la posición inicial
            servo_pendiente = -servo_pos
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
        if not en_auto:                         # el autónomo sigue hasta terminar
            izq = der = act_izq = act_der = 0.0
            drivetrain.stop()
        anunciar()
    elif evento == 3:
        procesar(ble.gatts_read(rx).decode())


def anunciar():
    nombre = NOMBRE.encode()
    anuncio = bytes((2, 0x01, 0x06, len(nombre) + 1, 0x09)) + nombre
    respuesta = bytes((17, 0x07)) + bytes(SERVICIO)
    ble.gap_advertise(100000, adv_data=anuncio, resp_data=respuesta)
    print("Esperando la app como", NOMBRE)


# ---------------- Bluetooth ----------------
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
servo_one.free()

# ---------------- programa principal ----------------
PASO = 0.05     # arranque suave: de 0 a 100 % en 0.4 s

try:
    while True:
        if auto_pedido:
            auto_pedido = False
            ejecutar_autonomo()

        if servo_pendiente != 0:
            grados = servo_pendiente
            servo_pendiente -= grados
            girar_servo(grados)

        # Seguridad: si la app deja de enviar 1 s, parar
        if time.ticks_diff(time.ticks_ms(), ultimo) > 1000:
            izq = der = 0.0

        act_izq += max(-PASO, min(PASO, izq - act_izq))
        act_der += max(-PASO, min(PASO, der - act_der))
        drivetrain.set_effort(act_izq, act_der)
        time.sleep_ms(20)
finally:
    drivetrain.stop()
    servo_one.free()
    print("Programa detenido")


===================================================================================================================================================================
CALIBRAR SERVO
===================================================================================================================================================================
from XRPLib.defaults import servo_one
import time

try:
    for valor in range(80, 121, 2):
        print("Valor:", valor)
        servo_one.set_angle(valor)
        time.sleep(2)
finally:
    servo_one.free()          # sin señal: el servo se detiene
    print("Fin de la prueba")

===================================================================================================================================================================
PROBAR GIROS DE SERVO
===================================================================================================================================================================
from XRPLib.defaults import servo_one
import time

SERVO_CENTRO = 100      # el mismo valor que en main.py
SERVO_VELOCIDAD = 10    # el mismo valor que en main.py
SEGUNDOS = 5

try:
    print("HORARIO", SEGUNDOS, "s -> valor", SERVO_CENTRO + SERVO_VELOCIDAD)
    servo_one.set_angle(SERVO_CENTRO + SERVO_VELOCIDAD)
    time.sleep(SEGUNDOS)
    servo_one.free()
    print("Quieto: mide cuántos grados giró. Sigue en 5 s...")
    time.sleep(5)

    print("ANTIHORARIO", SEGUNDOS, "s -> valor", SERVO_CENTRO - SERVO_VELOCIDAD)
    servo_one.set_angle(SERVO_CENTRO - SERVO_VELOCIDAD)
    time.sleep(SEGUNDOS)
finally:
    servo_one.free()
    print("Fin: calcula 5000 / grados para cada sentido")
