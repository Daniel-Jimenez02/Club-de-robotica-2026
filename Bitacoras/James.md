                                                                                                                                                      21/02/2026
BUMPERBOT EVOLUTION

1. Crear y programar un robot que retire una serie de 10 obstáculos(Vasos) de una pista, en un tiempo de 2 minutos, sin salirse de la pista mientras lo hace.

2.
Materiales:
 electronica:
   - Puente H
   - Bateria
   - Sensores infrarrojo
   - Motores
   - Switch electrico
   - jumper wires
   - Ultrasonido
 Objetos:
  - Rueda giratoria
  - Ruedas
  <img width="2664" height="2325" alt="EPM_bb" src="https://github.com/user-attachments/assets/375fd555-426a-46d1-9e05-8d340d934e7b" />
  <img width="1024" height="1024" alt="image" src="https://github.com/user-attachments/assets/df6ef289-3a2d-48ac-b9e9-f58c74670a5b" />


3.
// Pines del puente H
#define IN1 8
#define IN2 9
#define ENA 10
#define ENA2 11   // segundo ENA

// Encoder
#define encoderA 2
#define encoderB 3

volatile long pulsos = 0;

int velocidad = 200;

// Ajustar según tu motor
float pulsos_por_vuelta = 330;

// Radio de la rueda en cm
float radio_rueda = 3.0;

void setup() {

Serial.begin(9600);

pinMode(IN1, OUTPUT);
pinMode(IN2, OUTPUT);
pinMode(ENA, OUTPUT);
pinMode(ENA2, OUTPUT);

pinMode(encoderA, INPUT);
pinMode(encoderB, INPUT);

attachInterrupt(digitalPinToInterrupt(encoderA), contar, RISING);

Serial.println("Sistema listo");
Serial.println("Comandos:");
Serial.println("F velocidad → Adelante");
Serial.println("B velocidad → Atras");
Serial.println("S → Stop");
Serial.println("R → Reset encoder");
Serial.println("D → Distancia");
Serial.println("V → Vueltas");

}

void loop() {

if (Serial.available()) {

String comando = Serial.readStringUntil('\n');

char orden = comando.charAt(0);

int vel = comando.substring(1).toInt();

switch (orden) {

case 'F':
adelante(vel);
Serial.println("Motor adelante");
break;

case 'B':
atras(vel);
Serial.println("Motor atras");
break;

case 'S':
parar();
Serial.println("Motor detenido");
break;

case 'R':
pulsos = 0;
Serial.println("Encoder reiniciado");
break;

case 'V':
mostrarVueltas();
break;

case 'D':
mostrarDistancia();
break;

}

}

}

// Interrupción del encoder
void contar() {

if (digitalRead(encoderB) == HIGH)
pulsos++;
else
pulsos--;

}

// Motor adelante
void adelante(int vel) {

digitalWrite(IN1, HIGH);
digitalWrite(IN2, LOW);
analogWrite(ENA, vel);
analogWrite(ENA2, vel);

}

// Motor atrás
void atras(int vel) {

digitalWrite(IN1, LOW);
digitalWrite(IN2, HIGH);
analogWrite(ENA, vel);
analogWrite(ENA2, vel);

}

// Stop
void parar() {

digitalWrite(IN1, LOW);
digitalWrite(IN2, LOW);
analogWrite(ENA, 0);
analogWrite(ENA2, 0);

}

// Mostrar vueltas
void mostrarVueltas() {

float vueltas = pulsos / pulsos_por_vuelta;

Serial.print("Vueltas: ");
Serial.println(vueltas);

}

// Mostrar distancia
void mostrarDistancia() {

float vueltas = pulsos / pulsos_por_vuelta;

float distancia = vueltas * 2 * 3.1416 * radio_rueda;

Serial.print("Distancia recorrida (cm): ");
Serial.println(distancia);

}
