#include <Ps3Controller.h>
#include <ESP32Servo.h>

// ---------------- MOTORES ----------------
const int Adelante = 23;
const int Atras = 22;

const int Adelante2 = 17;
const int Atras2 = 16;

// ---------------- SERVOS ----------------
Servo servo1; // normal
Servo servo2; // 360

int servoPin1 = 25;
int servoPin2 = 27;

int servo1Pos = 90;

// ----------------------------------------

void onConnect() {
    Serial.println("Control conectado.");
}

void setup()
{
    Serial.begin(115200);

    // -------- MOTORES --------
    pinMode(Adelante, OUTPUT);
    pinMode(Atras, OUTPUT);

    pinMode(Adelante2, OUTPUT);
    pinMode(Atras2, OUTPUT);

    detener();

    // -------- SERVOS --------
    servo1.attach(servoPin1);
    servo2.attach(servoPin2);

    servo1.write(servo1Pos);

    // servo 360 detenido
    servo2.write(90);

    // -------- PS3 --------
    Ps3.attachOnConnect(onConnect);
    Ps3.begin("20:00:00:00:42:89");

    Serial.println("Listo.");
}

// ----------------------------------------
// MOTORES
// ----------------------------------------

void adelante() {

    digitalWrite(Adelante, HIGH);
    digitalWrite(Atras, LOW);

    digitalWrite(Adelante2, HIGH);
    digitalWrite(Atras2, LOW);
}

void atras() {

    digitalWrite(Adelante, LOW);
    digitalWrite(Atras, HIGH);

    digitalWrite(Adelante2, LOW);
    digitalWrite(Atras2, HIGH);
}

void izquierda() {

    digitalWrite(Adelante, HIGH);
    digitalWrite(Atras, LOW);

    digitalWrite(Adelante2, LOW);
    digitalWrite(Atras2, HIGH);
}

void derecha() {

    digitalWrite(Adelante, LOW);
    digitalWrite(Atras, HIGH);

    digitalWrite(Adelante2, HIGH);
    digitalWrite(Atras2, LOW);
}

void detener() {

    digitalWrite(Adelante, LOW);
    digitalWrite(Atras, LOW);

    digitalWrite(Adelante2, LOW);
    digitalWrite(Atras2, LOW);
}

// ----------------------------------------
// LOOP
// ----------------------------------------

void loop()
{
    if(!Ps3.isConnected())
        return;

    // -------- MOVIMIENTO --------

    if(Ps3.data.button.up) {
        adelante();
    }

    else if(Ps3.data.button.down) {
        atras();
    }

    else if(Ps3.data.button.left) {
        izquierda();
    }

    else if(Ps3.data.button.right) {
        derecha();
    }

    else {
        detener();
    }

    // ----------------------------------------
    // SERVO NORMAL (L1/L2)
    // ----------------------------------------

    if(Ps3.data.button.l1) {
        servo1Pos += 1;
    }

    if(Ps3.data.button.l2) {
        servo1Pos -= 1;
    }

    servo1Pos = constrain(servo1Pos, 0, 180);

    servo1.write(servo1Pos);

    // ----------------------------------------
    // SERVO 360 MG995
    // ----------------------------------------

    // R1 = girar adelante
    if(Ps3.data.button.r1) {
        servo2.write(180);
    }

    // R2 = girar atras
    else if(Ps3.data.button.r2) {
        servo2.write(0);
    }

    // nada = detener
    else {
        servo2.write(90);
    }

    delay(10);
}