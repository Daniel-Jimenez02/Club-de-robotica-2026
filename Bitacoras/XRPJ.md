# #############################################################################
# XRP ROBOTICADOS - Control por Bluetooth con la app "XRP Controller"
# #############################################################################
from XRPLib.defaults import drivetrain, servo_one
from XRPLib.servo import Servo
import bluetooth
import time

NOMBRE = "XRP-Valeria"      # debe coincidir con la app

# ---------------- CALIBRACIÓN REAL DE TU SERVO ----------------
SERVO_CENTRO = 94               # Punto neutro exacto (el servo no se mueve)
SERVO_VELOCIDAD = 10            # 104° para horario / 84° para antihorario
MS_POR_GRADO_HORARIO = 50       # Milisegundos por grado en sentido horario
MS_POR_GRADO_ANTIHORARIO = 50   # Milisegundos por grado en sentido antihorario

# ---------------- SERVO 180° (puerto SERVO 3) ----------------
# La app lo mueve de 10 en 10 grados (botones -10° y +10°) y envía "BRAZO:<ángulo>".
BRAZO_INICIO = 0                # ángulo al encender el robot
BRAZO_MIN = 0
BRAZO_MAX = 180
brazo = Servo.get_default_servo(3)
brazo_angulo = BRAZO_INICIO


def mover_brazo(angulo):
    """Lleva el servo de 180° a ese ángulo y lo mantiene ahí."""
    global brazo_angulo
    brazo_angulo = max(BRAZO_MIN, min(BRAZO_MAX, angulo))
    brazo.set_angle(brazo_angulo)
    print("BRAZO:", brazo_angulo)


# ---------------- AUTÓNOMO ORBIT ODYSSEY (botón AUTO, máximo 30 s) ----------------
# Empuja el escombro precargado hasta la Zona BAJA y se queda apoyado en el tubo:
# 1 (escombro) + 5 (estacionado) = 6 puntos.
# Salida: pasando un poco la línea central, MIRANDO a tus zonas, escombro delante de la pala.
SALIDA = "PARED"       # "PARED" o "CENTRO" (la app lo elige al pulsar AUTO)
ZONA = "DERECHA"       # "DERECHA" o "IZQUIERDA": esquina de tu Zona Baja vista desde el robot

VELOCIDAD_AUTO = 0.5   # avanzar
VELOCIDAD_GIRO = 0.35  # girar despacio para que el escombro no se escape
ANCHO_RUEDAS_CM = 15.5 # distancia entre ruedas: súbelo si girar(90) se queda corto, bájalo si se pasa

# avanzar(cm) / avanzar(cm, segundos_max)    girar(grados): + derecha, - izquierda
def pared_derecha():
    girar(-15)
    avanzar(75)
    girar(56)          # de frente al tubo diagonal
    avanzar(25, 3)     # empuja el escombro sobre el tubo y se apoya


def centro_derecha():
    girar(4)
    avanzar(73)
    girar(36)
    avanzar(25, 3)


def pared_izquierda():
    girar(-20)
    avanzar(79)
    girar(-66)
    avanzar(46)
    girar(41)
    avanzar(25, 3)


def centro_izquierda():
    girar(5)
    avanzar(74)
    girar(-91)
    avanzar(54)
    girar(41)
    avanzar(25, 3)


RUTAS = {
    ("PARED", "DERECHA"): pared_derecha,
    ("CENTRO", "DERECHA"): centro_derecha,
    ("PARED", "IZQUIERDA"): pared_izquierda,
    ("CENTRO", "IZQUIERDA"): centro_izquierda,
}


def rutina_autonoma(salida, zona):
    RUTAS[(salida, zona)]()
    # se queda quieto, apoyado contra la Zona Baja


# ---------------- ESTADO ----------------
izq = der = 0.0              # potencia que pide la app (-1 a 1)
act_izq = act_der = 0.0      # potencia actual de los motores (con arranque suave)
ultimo = time.ticks_ms()     # cuándo llegó el último comando
servo_pendiente = 0          # grados que faltan por girar
servo_pos = 0                # grados girados desde el inicio
auto_pedido = False
auto_ruta = (SALIDA, ZONA)   # salida y zona que pide la app
en_auto = False
auto_inicio = 0
cancelar = False
auto_pausado = False


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


def revisar():
    """Pausa (AUTO:PAUSA) o termina (STOP / 30 s) el autónomo según lo que pida la app.
    Se llama cada 20 ms mientras el robot se mueve, así responde al instante."""
    global auto_inicio
    if auto_pausado and not cancelar:
        drivetrain.stop()
        print("AUTÓNOMO: en pausa")
        inicio_pausa = time.ticks_ms()
        while auto_pausado and not cancelar:
            time.sleep_ms(20)
        # el tiempo en pausa no cuenta para los 30 s
        auto_inicio = time.ticks_add(auto_inicio, time.ticks_diff(time.ticks_ms(), inicio_pausa))
        print("AUTÓNOMO: sigue")
    if cancelar or time.ticks_diff(time.ticks_ms(), auto_inicio) > 30000:
        drivetrain.stop()
        raise FinAutonomo()


def esperar(ms):
    """Espera, respetando pausa, STOP y los 30 s."""
    restante = ms
    while restante > 0:
        revisar()
        time.sleep_ms(10)
        restante -= 10


def segundos_restantes():
    return max(0.1, (30000 - time.ticks_diff(time.ticks_ms(), auto_inicio)) / 1000)


def mover_ruedas(izq_cm, der_cm, velocidad, segundos_max=None):
    """Mueve cada rueda esos cm (medidos con los encoders), en pasos de 20 ms,
    revisando pausa y STOP en cada paso. Iguala las ruedas para ir recto."""
    drivetrain.reset_encoder_position()
    meta = (abs(izq_cm) + abs(der_cm)) / 2
    signo_izq = 1 if izq_cm >= 0 else -1
    signo_der = 1 if der_cm >= 0 else -1
    limite_ms = int(min(segundos_restantes(), segundos_max or 30) * 1000)
    movido_ms = 0
    while movido_ms < limite_ms:
        revisar()                        # aquí se pausa o se detiene
        izq = abs(drivetrain.get_left_encoder_position())
        der = abs(drivetrain.get_right_encoder_position())
        if (izq + der) / 2 >= meta:
            break
        correccion = (izq - der) * 0.05  # la rueda que va adelantada va un poco más lento
        drivetrain.set_effort(signo_izq * max(0, velocidad - correccion),
                              signo_der * max(0, velocidad + correccion))
        time.sleep_ms(20)
        movido_ms += 20
    drivetrain.stop()


def avanzar(cm, segundos_max=None):
    """Avanza (o retrocede si es negativo) esos cm.
    segundos_max: deja de empujar a ese tiempo (p. ej. si topa con un tubo)."""
    mover_ruedas(cm, cm, VELOCIDAD_AUTO, segundos_max)


def girar(grados):
    """Gira sobre sí mismo: + derecha, - izquierda."""
    arco = abs(grados) / 360 * 3.1416 * ANCHO_RUEDAS_CM
    if grados >= 0:
        mover_ruedas(arco, -arco, VELOCIDAD_GIRO)     # derecha: rueda izquierda adelante
    else:
        mover_ruedas(-arco, arco, VELOCIDAD_GIRO)
def mover(izquierda, derecha, ms):
    drivetrain.set_effort(izquierda / 100, derecha / 100)
    esperar(ms)


def servo(grados):
    girar_servo(grados)


def ejecutar_autonomo():
    global en_auto, auto_inicio, cancelar, auto_pausado, izq, der, act_izq, act_der
    print("AUTÓNOMO: inicio", auto_ruta)
    izq = der = act_izq = act_der = 0.0
    en_auto = True
    cancelar = False
    auto_pausado = False
    auto_inicio = time.ticks_ms()
    try:
        rutina_autonoma(*auto_ruta)
    except FinAutonomo:
        pass
    drivetrain.stop()
    detener_servo_de_golpe()
    en_auto = False
    auto_pausado = False
    print("AUTÓNOMO: fin")


# ---------------- COMANDOS QUE LLEGAN DE LA APP ----------------
def procesar(texto):
    global izq, der, act_izq, act_der, ultimo, servo_pendiente, auto_pedido, auto_ruta, cancelar, auto_pausado
    ultimo = time.ticks_ms()
    texto = texto.strip().upper()
    try:
        if texto == "STOP":                     # Parar todo (motores, servo y autónomo)
            cancelar = True
            izq = der = act_izq = act_der = 0.0
            drivetrain.stop()
            detener_servo_de_golpe()
        elif texto.startswith("AUTO:START"):     # "AUTO:START:PD" = salida Pared, zona Derecha
            codigo = texto[11:13]                # P/C = pared/centro, D/I = derecha/izquierda
            if len(codigo) == 2 and codigo[0] in "PC" and codigo[1] in "DI":
                auto_ruta = ("PARED" if codigo[0] == "P" else "CENTRO",
                             "DERECHA" if codigo[1] == "D" else "IZQUIERDA")
            auto_pedido = True
        elif texto == "AUTO:STOP":
            cancelar = True
        elif texto == "AUTO:PAUSA":             # pausa el autónomo (el tiempo no corre)
            auto_pausado = True
        elif texto == "AUTO:SEGUIR":            # sigue desde donde se pausó
            auto_pausado = False
        elif texto.startswith("SERVO_PASO:"):   # Botones de dirección "+1" o "-1"
            servo_pendiente += int(texto[11:])
        elif texto.startswith("BRAZO:"):        # Servo 180°: "BRAZO:40"
            mover_brazo(int(texto[6:]))
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
mover_brazo(BRAZO_INICIO)

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
