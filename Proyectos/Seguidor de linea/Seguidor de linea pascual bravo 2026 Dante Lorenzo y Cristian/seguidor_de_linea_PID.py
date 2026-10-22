from machine import Pin, ADC, PWM   # <- UNA sola línea reemplaza tus 3 imports
import time

# Variables y pines(van hasta funciones)
LED = Pin("LED", Pin.OUT) # Led para calibracion
velocidad_base = 0.5
velocidad_max = 0.95
regleta = [0] * 15
minimos = [65535] * 15
maximos = [0] * 15
# Variables PID(Dani espero que este bueno, sino me pones comentario en el github para saber como hacerlo)
kp = 1
kd = 1
ki = 1

# Motores
MOT_IZQ = (PWM(Pin(14)), PWM(Pin(15)))   
MOT_DER = (PWM(Pin(12)), PWM(Pin(13)))
for p in MOT_IZQ:
    p.freq(1000)
for p in MOT_DER:
    p.freq(1000)
def motor(m, v):
    """m = (Pin_adelante, Pin_atras) del motor. v = -1.0 a 1.0"""
    if v > 1.0: v = 1.0        
    if v < -1.0: v = -1.0
    duty = int(abs(v) * 65535) 
    adelante, atras = m
    if v >= 0:                 
        adelante.duty_u16(duty)
        atras.duty_u16(0)
    else:
        adelante.duty_u16(0)
        atras.duty_u16(duty)

# Multiplexor
mux_s0 = Pin(2, Pin.OUT)    
mux_s1 = Pin(3, Pin.OUT)
mux_s2 = Pin(4, Pin.OUT)
mux_s3 = Pin(5, Pin.OUT)
mux_sig = ADC(Pin(26)) 

# Funciones
def leer_sensor(canal):
    mux_s0.value(canal & 0x01)
    mux_s1.value((canal >> 1) & 0x01)
    mux_s2.value((canal >> 2) & 0x01)
    mux_s3.value((canal >> 3) & 0x01)
    time.sleep_us(10)
    return mux_sig.read_u16()

def leer_todos_sensores():
    for i in range(15):
        regleta[i] = leer_sensor(i)
    return regleta

def calibrar():
    t0 = time.ticks_ms()
    while time.ticks_diff(time.ticks_ms(), t0) < 10000:
        LED.toggle()
        leer_todos_sensores()
        for i in range(15):
            if regleta[i] < minimos[i]: minimos[i] = regleta[i]
            if regleta[i] > maximos[i]: maximos[i] = regleta[i]
        time.sleep_ms(50)
    LED.off()
    print('Minimos:', minimos)
    print('Maximos:', maximos)
def normalizar():
    leer_todos_sensores()
    valores_normales = [0] * 15
    for i in range(15):
        if maximos[i] <= minimos[i]:          
            valores_normales[i] = 0
        else:                                 
            valores_normales[i] = (regleta[i] - minimos[i]) * 1000 // (maximos[i] - minimos[i])
        if valores_normales[i] < 0: valores_normales[i] = 0      # clamps (afuera, para todos)
        if valores_normales[i] > 1000: valores_normales[i] = 1000
    return valores_normales
def posicion():
    limite_peso = 80
    valores = normalizar()
    peso = sum(valores)
    if peso < limite_peso: return None
    centro = sum(i * valores[i] for i in range(15)) / peso
    return (centro - 7.0) / 7.0
def conducir(u):
    izq = velocidad_base + u
    der = velocidad_base - u
    if izq > velocidad_max: izq = velocidad_max
    if der > velocidad_max: der = velocidad_max
    motor(MOT_IZQ, izq)
    motor(MOT_DER, der)
def parar():
    motor(MOT_IZQ, 0)
    motor(MOT_DER, 0)
    
def correr():
    while True:
        error = posicion()
        u = kp * error
        conducir(u)