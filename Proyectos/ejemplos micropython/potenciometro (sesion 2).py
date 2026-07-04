import machine
import utime

pot = machine.ADC(machine.Pin(26))  # GPIO26 = ADC0

while True:
    lectura = pot.read_u16()  # Rango: 0 - 65535
    voltaje = (lectura * 3.3) / 65535
    print("Valor ADC:", lectura, "| Voltaje:", round(voltaje, 2), "V")
    utime.sleep(0.5)
