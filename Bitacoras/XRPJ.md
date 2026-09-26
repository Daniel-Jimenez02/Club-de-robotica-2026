from XRPLib.defaults import drivetrain
import bluetooth
import time

NOMBRE = "XRP-ROBOTICADOS"   # debe coincidir con la app
ACELERACION = 2.5            # rampa: más alto = arranques más bruscos
WATCHDOG_MS = 1000           # sin señal de la app 1 s -> parar

SERVICIO = bluetooth.UUID("6E400001-B5A3-F393-E0A9-E50E24DCCA9E")
RX = bluetooth.UUID("6E400002-B5A3-F393-E0A9-E50E24DCCA9E")
TX = bluetooth.UUID("6E400003-B5A3-F393-E0A9-E50E24DCCA9E")

objetivo = [0.0, 0.0]        # lo que pide la app (izq, der) de -1 a 1
actual = [0.0, 0.0]          # lo que tienen los motores (con rampa)
ultimo_comando = time.ticks_ms()


def parar():
    objetivo[0] = objetivo[1] = 0.0
    actual[0] = actual[1] = 0.0
    drivetrain.stop()


def procesar(texto):
    global ultimo_comando
    ultimo_comando = time.ticks_ms()
    texto = texto.strip().upper()

    if texto == "STOP":
        parar()
        return

    try:
        izq, der = texto.split(",")
        objetivo[0] = max(-100, min(100, int(izq[2:]))) / 100
        objetivo[1] = max(-100, min(100, int(der[2:]))) / 100
        print("L:", izq[2:], " R:", der[2:])
    except ValueError:
        print("Comando inválido:", texto)
        parar()


def evento_ble(evento, datos):
    if evento == 1:                      # teléfono conectado
        print("Conectado")
    elif evento == 2:                    # teléfono desconectado
        print("Desconectado: motores parados")
        parar()
        anunciar()
    elif evento == 3:                    # llegó un comando
        procesar(ble.gatts_read(rx_handle).decode())


def anunciar():
    # Nombre en el anuncio y servicio en la respuesta (máx. 31 bytes cada uno)
    nombre = NOMBRE.encode()
    anuncio = bytes((2, 0x01, 0x06, len(nombre) + 1, 0x09)) + nombre
    respuesta = bytes((17, 0x07)) + bytes(SERVICIO)
    ble.gap_advertise(100000, adv_data=anuncio, resp_data=respuesta)
    print("Esperando la app como", NOMBRE)


ble = bluetooth.BLE()
ble.active(True)
ble.irq(evento_ble)
((tx_handle, rx_handle),) = ble.gatts_register_services(
    ((SERVICIO, ((TX, bluetooth.FLAG_NOTIFY), (RX, bluetooth.FLAG_WRITE))),)
)
ble.gatts_set_buffer(rx_handle, 100, False)   # cada comando reemplaza al anterior
anunciar()

try:
    while True:
        # Seguridad: si la app deja de enviar, parar
        if time.ticks_diff(time.ticks_ms(), ultimo_comando) > WATCHDOG_MS:
            if objetivo != [0.0, 0.0] or actual != [0.0, 0.0]:
                print("Sin señal de la app: parando")
                parar()

        # Rampa: acercar cada motor a su objetivo poco a poco (cada 20 ms)
        paso = ACELERACION * 0.02
        for i in (0, 1):
            diferencia = objetivo[i] - actual[i]
            if abs(diferencia) <= paso:
                actual[i] = objetivo[i]
            elif diferencia > 0:
                actual[i] += paso
            else:
                actual[i] -= paso

        if actual == [0.0, 0.0]:
            drivetrain.stop()
        else:
            drivetrain.set_effort(actual[0], actual[1])

        time.sleep_ms(20)
finally:
    drivetrain.stop()
    print("Programa detenido")

