import machine
import utime

trig = machine.Pin(3, machine.Pin.OUT)
echo = machine.Pin(2, machine.Pin.IN)

def medir_distancia():
    trig.low()
    utime.sleep_us(2)
    trig.high()
    utime.sleep_us(10)
    trig.low()

    while echo.value() == 0:
        inicio = utime.ticks_us()

    while echo.value() == 1:
        fin = utime.ticks_us()

    duracion = utime.ticks_diff(fin, inicio)
    distancia_cm = (duracion / 2) / 29.1  # Constante de velocidad del sonido
    return round(distancia_cm, 2)

while True:
    distancia = medir_distancia()
    print("Distancia:", distancia, "cm")
    utime.sleep(1)

