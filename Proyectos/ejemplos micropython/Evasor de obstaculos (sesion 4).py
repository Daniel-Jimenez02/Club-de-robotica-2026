import machine        # Importa la librería para manejar los pines y periféricos
import utime          # Importa la librería para manejo de tiempo

# Configuración de pines del sensor ultrasónico
TRIG = machine.Pin(3, machine.Pin.OUT)   # Pin TRIG como salida
ECHO = machine.Pin(2, machine.Pin.IN)    # Pin ECHO como entrada

# Configuración de pines del módulo L298N para los motores
IN1 = machine.Pin(10, machine.Pin.OUT)   # Motor A - dirección 1
IN2 = machine.Pin(11, machine.Pin.OUT)   # Motor A - dirección 2
IN3 = machine.Pin(12, machine.Pin.OUT)   # Motor B - dirección 1
IN4 = machine.Pin(13, machine.Pin.OUT)   # Motor B - dirección 2

# Función para mover el robot hacia adelante
def avanzar():
    IN1.high()
    IN2.low()
    IN3.high()
    IN4.low()

# Función para mover el robot hacia atrás
def retroceder():
    IN1.low()
    IN2.high()
    IN3.low()
    IN4.high()

# Función para detener ambos motores
def detener():
    IN1.low()
    IN2.low()
    IN3.low()
    IN4.low()

# Función para girar hacia la derecha
def girar_derecha():
    IN1.high()
    IN2.low()
    IN3.low()
    IN4.high()

# Función para girar hacia la izquierda
def girar_izquierda():
    IN1.low()
    IN2.high()
    IN3.high()
    IN4.low()

# Función que mide la distancia usando el sensor ultrasónico
def medir_distancia():
    TRIG.low()                # Asegura que el TRIG esté en bajo
    utime.sleep_us(2)         # Espera 2 microsegundos
    TRIG.high()               # Enciende el TRIG
    utime.sleep_us(10)        # Espera 10 microsegundos (pulso)
    TRIG.low()                # Apaga el TRIG

    while ECHO.value() == 0:
        inicio = utime.ticks_us()    # Marca el tiempo cuando ECHO se pone alto

    while ECHO.value() == 1:
        fin = utime.ticks_us()       # Marca el tiempo cuando ECHO se pone bajo

    duracion = utime.ticks_diff(fin, inicio)   # Calcula la duración del pulso
    distancia = (duracion * 0.0343) / 2        # Convierte a centímetros
    return distancia                           # Devuelve la distancia

# Bucle principal del programa, recuerden que si está fuera del while solo se ejecuta una vez, como en el loop de arduino
print("Iniciando sistema...")

while True:
    distancia = medir_distancia()    # Llama la función para medir la distancia
    print("Distancia:", round(distancia, 2), "cm")  # Muestra la distancia redondeada, round redondea el dato a un numero entero mas cercano

    if distancia < 20:
        detener()               # Si hay un objeto cerca, menos de 20cm se detiene
        utime.sleep(0.5)        
        retroceder()            # Retrocede un poco
        utime.sleep(0.5)
        girar_derecha()         # Gira para esquivar
        utime.sleep(0.5)
    else:
        avanzar()               # Si no hay obstáculos, sigue avanzando

    utime.sleep(0.1)            # Pequeña pausa para evitar sobrecarga de lecturas
