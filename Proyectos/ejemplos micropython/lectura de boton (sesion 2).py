import machine
import utime
  
led = machine.Pin(25, machine.Pin.OUT)
boton = machine.Pin(15, machine.Pin.IN, machine.Pin.PULL_DOWN) #machine.Pin.PULL_UP
  
while True:
    if boton():
        utime.sleep_ms(200)
        led.value(1)
        print("boton presionado")
    else:
        led.off()
    