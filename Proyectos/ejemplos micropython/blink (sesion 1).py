from machine import Pin#libreria de microcontroladores
import utime #libreria para control del tiempo en microcontroladores
  
led = Pin(25, Pin.OUT) #pin del led intgrado en el micro
              
  
while True:
    led.on()
    utime.sleep(1)
    led.off()
    utime.sleep(1)
