# Codigo PID seguidor de linea pascual bravo 2026
# Dante Lorenzo y Cristian (Los ganadores espero)
# V4: firmware de Dante + servidor de sintonia por WiFi (peticion 23 oct)
from machine import Pin, ADC, PWM
import time
import network
import socket
import json

# Variables y pines(van hasta funciones)
LED = Pin("LED", Pin.OUT) # Led para calibracion de sensores, integrado de la pico
velocidad_base = 0.5
velocidad_max = 0.95
regleta = [0] * 15
minimos = [65535] * 15
maximos = [0] * 15
# Variables PID(Dani espero que este bueno, sino me pones comentario en el github para saber como hacerlo)
kp = 1
kd = 1
ki = 0
debug = True   # pone False el dia de la carrera para no frenar el lazo con prints

# Sintonia por WiFi
ARCHIVO_CONFIG = 'config.json'
RED_NOMBRE = 'Velocista-Dante'
RED_CLAVE = 'velocista2026'

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
        if valores_normales[i] < 0: valores_normales[i] = 0
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
    error_anterior = 0.0
    t_prev = time.ticks_ms()
    integral = 0.0
    limite_i = 0.4
    while True:
        error = posicion()
        if error is None:
            error = 0.0
        t_ahora = time.ticks_ms()
        dt = time.ticks_diff(t_ahora, t_prev) / 1000.0
        t_prev = t_ahora
        derivada = error - error_anterior
        integral = integral + error * dt
        if integral > limite_i:  integral = limite_i
        if integral < -limite_i: integral = -limite_i
        u = kp * error + kd * derivada + ki * integral
        error_anterior = error
        conducir(u)
        if debug:
            print("Error:", error, "Derivada:", derivada, "u:", u)

# ================= SINTONIA POR WIFI (la app) =================
# La Pico crea su propia red WiFi. Te conectas con el celu,
# abres 192.168.4.1 y ajustas kp/kd/ki desde botones.

PAGINA = """<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Sintonia Velocista</title>
<style>
body{font-family:sans-serif;background:#111;color:#eee;text-align:center;margin:0;padding:10px}
h1{font-size:22px}
.ganancia{background:#1e1e1e;border-radius:12px;margin:12px auto;padding:10px;max-width:400px}
.valor{font-size:34px;font-weight:bold;margin:8px 0;color:#5bc8ff}
a.b{display:inline-block;background:#2d6cdf;color:#fff;text-decoration:none;border-radius:10px;padding:14px 10px;margin:4px;font-size:18px;font-weight:bold;min-width:60px}
a.grande{background:#1d9e4f;font-size:24px;padding:18px;display:block;max-width:400px;margin:14px auto;border-radius:12px;color:#fff;text-decoration:none;font-weight:bold}
a.guardar{background:#c9a227;display:block;max-width:400px;margin:10px auto;padding:14px;font-weight:bold;border-radius:10px;color:#111;text-decoration:none}
a.base{color:#888;font-size:14px;display:block;margin:8px}
p{color:#888;font-size:13px}
</style>
</head>
<body>
<h1>&#128667; Velocista Pascual Bravo</h1>
<div class="ganancia"><h3>KP (acelerador)</h3><div class="valor">%%KP%%</div>
<a class="b" href="/?kp=menosf">-0.02</a><a class="b" href="/?kp=menos">-0.1</a><a class="b" href="/?kp=mas">+0.1</a><a class="b" href="/?kp=masf">+0.02</a></div>
<div class="ganancia"><h3>KD (freno)</h3><div class="valor">%%KD%%</div>
<a class="b" href="/?kd=menosf">-0.02</a><a class="b" href="/?kd=menos">-0.1</a><a class="b" href="/?kd=mas">+0.1</a><a class="b" href="/?kd=masf">+0.02</a></div>
<div class="ganancia"><h3>KI (cuaderno, dormida si es 0)</h3><div class="valor">%%KI%%</div>
<a class="b" href="/?ki=menosf">-0.02</a><a class="b" href="/?ki=menos">-0.1</a><a class="b" href="/?ki=mas">+0.1</a><a class="b" href="/?ki=masf">+0.02</a></div>
<a class="guardar" href="/?accion=guardar">&#128190; GUARDAR SINTONIA</a>
<a class="base" href="/?accion=base">valores base (kp=1, kd=1, ki=0)</a>
<a class="grande" href="/?accion=correr">&#127937; &iexcl;CORRER!</a>
<p>Guardar escribe config.json en la Pico: sobrevive al apag&oacute;n.<br>Dante Lorenzo y Cristian</p>
</body>
</html>
"""

def cargar_sintonia():
    global kp, kd, ki
    try:
        f = open(ARCHIVO_CONFIG)
        c = json.load(f)
        f.close()
        kp = c.get('kp', kp)
        kd = c.get('kd', kd)
        ki = c.get('ki', ki)
        print('Sintonia cargada de', ARCHIVO_CONFIG, '-> kp:', kp, 'kd:', kd, 'ki:', ki)
    except OSError:
        print('Sin', ARCHIVO_CONFIG, ': arranco con los valores del codigo')

def guardar_sintonia():
    f = open(ARCHIVO_CONFIG, 'w')
    f.write(json.dumps({'kp': kp, 'kd': kd, 'ki': ki}))
    f.close()
    print('Sintonia guardada -> kp:', kp, 'kd:', kd, 'ki:', ki)

def pagina():
    p = PAGINA
    p = p.replace('%%KP%%', str(kp))
    p = p.replace('%%KD%%', str(kd))
    p = p.replace('%%KI%%', str(ki))
    return p

def sintonizar():
    global kp, kd, ki
    ap = network.WLAN(network.AP_IF)
    ap.config(essid=RED_NOMBRE, password=RED_CLAVE)
    ap.active(True)
    while not ap.active():
        time.sleep(0.2)
    LED.on()   # LED fijo encendido = "conectate ya"
    print('Red WiFi lista:', RED_NOMBRE, '| clave:', RED_CLAVE)
    print('Abre en el celu: http://192.168.4.1')
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.bind(('', 80))
    s.listen(2)
    while True:
        conn, _ = s.accept()
        try:
            peticion = conn.recv(1024).decode('utf-8', 'ignore')
            # KP (ojo: revisar masf/menosf ANTES que mas/menos)
            if 'kp=masf' in peticion: kp = round(kp + 0.02, 2)
            elif 'kp=mas' in peticion: kp = round(kp + 0.1, 2)
            elif 'kp=menosf' in peticion: kp = round(kp - 0.02, 2)
            elif 'kp=menos' in peticion: kp = round(kp - 0.1, 2)
            # KD
            if 'kd=masf' in peticion: kd = round(kd + 0.02, 2)
            elif 'kd=mas' in peticion: kd = round(kd + 0.1, 2)
            elif 'kd=menosf' in peticion: kd = round(kd - 0.02, 2)
            elif 'kd=menos' in peticion: kd = round(kd - 0.1, 2)
            # KI
            if 'ki=masf' in peticion: ki = round(ki + 0.02, 2)
            elif 'ki=mas' in peticion: ki = round(ki + 0.1, 2)
            elif 'ki=menosf' in peticion: ki = round(ki - 0.02, 2)
            elif 'ki=menos' in peticion: ki = round(ki - 0.1, 2)
            # Clamps de seguridad
            if kp < 0: kp = 0.0
            if kp > 5: kp = 5.0
            if kd < 0: kd = 0.0
            if kd > 5: kd = 5.0
            if ki < 0: ki = 0.0
            if ki > 1: ki = 1.0
            # Acciones
            if 'accion=guardar' in peticion:
                guardar_sintonia()
            if 'accion=base' in peticion:
                kp, kd, ki = 1.0, 1.0, 0.0
            # Responder SIEMPRE antes de revisar si hay que correr
            conn.send(pagina())
            conn.close()
            if 'accion=correr' in peticion:
                s.close()
                ap.active(False)   # apagar WiFi: toda la potencia para correr
                LED.off()
                print('¡Sintonia final! kp:', kp, 'kd:', kd, 'ki:', ki)
                return
        except OSError:
            try:
                conn.close()
            except:
                pass

# ================= MAIN =================
# 1) Carga la sintonia guardada (si existe)
cargar_sintonia()
# 2) Calibracion: 10s barriendo la regleta (LED parpadea)
calibrar()
# 3) Sintonia WiFi: el celu conecta a Velocista-Dante y toca ¡CORRER!
sintonizar()
# 4) A volar
correr()