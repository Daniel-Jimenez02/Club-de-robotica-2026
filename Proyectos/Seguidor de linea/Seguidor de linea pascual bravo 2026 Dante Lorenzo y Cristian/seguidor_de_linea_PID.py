# Codigo PID seguidor de linea pascual bravo 2026
# Dante, Lorenzo y Cristian(Los ganadores espero)
from machine import Pin, ADC, PWM
import time
#-------------------------------------------------------
#      Variables y pines(van hasta funciones)
#-------------------------------------------------------

Boton = Pin(6, Pin.IN, Pin.PULL_UP) # Boton para calibracion de sensores
LED = Pin("LED", Pin.OUT) # Led para calibracion de sensores, integrado de la pico
velocidad_base = 0.5
velocidad_max = 0.95
regleta = [0] * 15
minimos = [65535] * 15
maximos = [0] * 15

#-------------------------------------------------------
# Variables PID(Dani espero que este bueno, sino me pones comentario en el github para saber como hacerlo)
#-------------------------------------------------------

kp = 1
kd = 0.01
ki = 0


ultimo_error = 0.0 # Esta es por si se sale de la linea para que la vuelva a coger

#-------------------------------------------------------
#                     Motores
#-------------------------------------------------------

MOT_IZQ = (PWM(Pin(14)), PWM(Pin(15)))   
MOT_DER = (PWM(Pin(13)), PWM(Pin(12)))
for p in MOT_IZQ:
    p.freq(1000)
for p in MOT_DER:
    p.freq(1000)
def motor(m, v):
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
#-------------------------------------------------------
#                  Multiplexor
#-------------------------------------------------------

mux_s0 = Pin(2, Pin.OUT)    
mux_s1 = Pin(3, Pin.OUT)
mux_s2 = Pin(4, Pin.OUT)
mux_s3 = Pin(5, Pin.OUT)
mux_sig = ADC(Pin(26)) 

#-------------------------------------------------------
#                    Funciones
#-------------------------------------------------------


#-------------------------------------------------------
#                  Leer sensores
#-------------------------------------------------------

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

#-------------------------------------------------------
#          Calibracion, normalizacion y boton
#-------------------------------------------------------

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
    #print('Minimos:', minimos)
    #print('Maximos:', maximos)
    
    
def presionar_boton():
    while Boton.value() == 1:      
        LED.toggle()               
        time.sleep_ms(200)
    time.sleep_ms(20)              
    while Boton.value() == 0:      
        time.sleep_ms(10)
    LED.off()


def normalizar():
    leer_todos_sensores()
    valores_normales = [0] * 15
    for i in range(15):
        if maximos[i] <= minimos[i]:          
            valores_normales[i] = 0
        else:                                 
            valores_normales[i] = (regleta[i] - minimos[i]) * 1000 // (maximos[i] - minimos[i])
        if valores_normales[i] < 0: valores_normales[i] = 0      
        if valores_normales[i] > 1000: valores_normales[i] = 1000
    return valores_normales

#-------------------------------------------------------
#            Movimiento y calculo de posicion
#-------------------------------------------------------

def posicion():
    global ultimo_error
    limite_peso = 150
    valores = normalizar()
    peso = sum(valores)
    if peso < limite_peso: 
        return None
    centro = sum(i * valores[i] for i in range(15)) / peso
    error_calculado = (centro - 7.0) / 7.0
    ultimo_error = error_calculado
    return error_calculado


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

#-------------------------------------------------------
#                        PID
#-------------------------------------------------------

def correr():
    error_anterior = 0.0
    t_prev = time.ticks_us()
    integral = 0.0
    limite_i = 0.4
    while True:
        error = posicion()
        if error is None:      
            if ultimo_error > 0:
                error = 1 
            else:
                error = -1  
        t_ahora = time.ticks_us()
        dt = time.ticks_diff(t_ahora, t_prev) / 1000000.0
        t_prev = t_ahora
        derivada = (error - error_anterior) / dt
        integral = integral + error * dt
        if integral > limite_i:  integral = limite_i      
        if integral < -limite_i: integral = -limite_i    
        u = kp * error + kd * derivada + ki * integral
        error_anterior = error
        conducir(u)
        #print("Error:", error, "Derivada:", derivada, "u:", u)
        
#-------------------------------------------------------
#            Llamado de funciones y espera
#-------------------------------------------------------

presionar_boton()
calibrar()
for i in range(5, 0, -1):  
    LED.toggle()
    time.sleep(1)
LED.off()
correr()