                                                                                                                                                      21/02/2026
BUMPERBOT EVOLUTION

1. Crear y programar un robot que retire una serie de 10 obstáculos(Vasos) de una pista, en un tiempo de 2 minutos, sin salirse de la pista mientras lo hace.

2.
Materiales:
 electronica:
   - Puente H
   - Bateria
   - Sensores infrarrojo
   - Motores(TT-MOT-350RPM-ENCO)
   - Switch electrico
   - jumper wires
   - Ultrasonido
   - Giroscopio Arduino(MPU-6050)
 Objetos:
  - Rueda giratoria
  - Ruedas
  <img width="2664" height="2325" alt="EPM_bb" src="https://github.com/user-attachments/assets/375fd555-426a-46d1-9e05-8d340d934e7b" />


3.
// Motor A
#define IN1 8
#define IN2 9
#define ENA 10

// Motor B
#define IN3 6
#define IN4 7
#define ENA2 11   // segundo enable

// Encoder
#define encoderA 2
#define encoderB 3

volatile long pulsos = 0;

float pulsos_por_vuelta = 330;
float radio_rueda = 3.0;

void setup() {

  Serial.begin(9600);

  // Motor A
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);

  // Motor B
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA2, OUTPUT);

  // Encoder
  pinMode(encoderA, INPUT);
  pinMode(encoderB, INPUT);

  attachInterrupt(digitalPinToInterrupt(encoderA), contar, RISING);

  Serial.println("Sistema listo");
}

void loop() {

  if (Serial.available()) {

    String comando = Serial.readStringUntil('\n');

    char orden = comando.charAt(0);
    int vel = comando.substring(1).toInt();

    switch (orden) {

      case 'F':
        adelante(vel);
        Serial.println("Motores adelante");
        break;

      case 'B':
        atras(vel);
        Serial.println("Motores atras");
        break;

      case 'S':
        parar();
        Serial.println("Motores detenidos");
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

// Encoder
void contar() {
  if (digitalRead(encoderB) == HIGH)
    pulsos++;
  else
    pulsos--;
}

// Adelante
void adelante(int vel) {

  // Motor A
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, vel);

  // Motor B
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA2, vel);
}

// Atrás
void atras(int vel) {

  // Motor A
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  analogWrite(ENA, vel);

  // Motor B
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA2, vel);
}

// Stop
void parar() {

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 0);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENA2, 0);
}

// Vueltas
void mostrarVueltas() {
  float vueltas = pulsos / pulsos_por_vuelta;
  Serial.print("Vueltas: ");
  Serial.println(vueltas);
}

// Distancia
void mostrarDistancia() {
  float vueltas = pulsos / pulsos_por_vuelta;
  float distancia = vueltas * 2 * 3.1416 * radio_rueda;
  Serial.print("Distancia (cm): ");
  Serial.println(distancia);
}
