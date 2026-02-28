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
3.
 PRUEBA DE MOTORES:
    // Motor A
const int IN1 = 8;
const int IN2 = 9;

// Motor B
const int IN3 = 10;
const int IN4 = 11;

void setup() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
}

void avanzar() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void retroceder() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void girarDerecha() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void girarIzquierda() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void detener() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}


void loop() {
  avanzar();
  delay(3000);

  detener();
  delay(1000);

  retroceder();
  delay(3000);

  detener();
  delay(1000);

  girarDerecha();
  delay(2000);

  PRUEBA TCRT5000:
  const int tcrt= 2;

void setup() {
  pinMode(tcrt, INPUT);
  Serial.begin(9600);
}

void loop() {
  int estado = digitalRead(tcrt);

  if (estado == LOW) {
    Serial.println("NEGRO");
  } else {
    Serial.println("BLANCO");
  }

  delay(500);
}

  girarIzquierda();
  delay(2000);

  detener();
  delay(4000);
}
