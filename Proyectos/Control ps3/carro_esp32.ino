#include <Ps3Controller.h>

const int Adelante=18; 
const int Atras=19;
const int Adelante2=23; 
const int Atras2=22;

void notify()
{
    // Control de motores con flechas del D-pad (solo digital, sin analog)
    
    // Flecha arriba: Avanzar (ambos motores adelante)
    if(Ps3.event.button_down.up) {
        Serial.println("Avanzar");
        digitalWrite(Adelante, HIGH);
        digitalWrite(Atras, LOW);
        digitalWrite(Adelante2, HIGH);
        digitalWrite(Atras2, LOW);
    }
    if(Ps3.event.button_up.up) {
        Serial.println("Parar");
        digitalWrite(Adelante, LOW);
        digitalWrite(Atras, LOW);
        digitalWrite(Adelante2, LOW);
        digitalWrite(Atras2, LOW);
    }

    // Flecha abajo: Retroceder (ambos motores atrás)
    if(Ps3.event.button_down.down) {
        Serial.println("Retroceder");
        digitalWrite(Adelante, LOW);
        digitalWrite(Atras, HIGH);
        digitalWrite(Adelante2, LOW);
        digitalWrite(Atras2, HIGH);
    }
    if(Ps3.event.button_up.down) {
        Serial.println("Parar");
        digitalWrite(Adelante, LOW);
        digitalWrite(Atras, LOW);
        digitalWrite(Adelante2, LOW);
        digitalWrite(Atras2, LOW);
    }

    // Flecha izquierda: Girar izquierda (motor derecho adelante, izquierdo atrás)
    if(Ps3.event.button_down.left) {
        Serial.println("Girar Derecha");
        digitalWrite(Adelante, HIGH);
        digitalWrite(Atras, LOW);   // Motor 1 adelante
        digitalWrite(Adelante2, LOW);
        digitalWrite(Atras2, HIGH); // Motor 2 atrás
    }
    if(Ps3.event.button_up.left) {
        Serial.println("Parar");
        digitalWrite(Adelante, LOW);
        digitalWrite(Atras, LOW);
        digitalWrite(Adelante2, LOW);
        digitalWrite(Atras2, LOW);
    }

    // Flecha derecha: Girar derecha (motor izquierdo adelante, derecho atrás)
    if(Ps3.event.button_down.right) {
        Serial.println("Girar Izquierda");
        digitalWrite(Adelante, LOW);
        digitalWrite(Atras, HIGH);  // Motor 1 atrás
        digitalWrite(Adelante2, HIGH);
        digitalWrite(Atras2, LOW);  // Motor 2 adelante
    }
    if(Ps3.event.button_up.right) {
        Serial.println("Parar");
        digitalWrite(Adelante, LOW);
        digitalWrite(Atras, LOW);
        digitalWrite(Adelante2, LOW);
        digitalWrite(Atras2, LOW);
    }
}

void onConnect(){
    Serial.println("Control conectado.");
}

void setup()
{
    Serial.begin(115200);

    // Configurar pines de motores como salidas
    pinMode(Adelante, OUTPUT);
    pinMode(Atras, OUTPUT);
    pinMode(Adelante2, OUTPUT);
    pinMode(Atras2, OUTPUT);

    // Apagar motores al inicio
    digitalWrite(Adelante, LOW);
    digitalWrite(Atras, LOW);
    digitalWrite(Adelante2, LOW);
    digitalWrite(Atras2, LOW);

    Ps3.attach(notify);
    Ps3.attachOnConnect(onConnect);
    Ps3.begin("20:00:00:00:42:86");  // Cambia por la MAC de tu PS3 si es necesario

    Serial.println("Listo. Presiona las flechas para controlar el carro.");
}

void loop()
{
    if(!Ps3.isConnected())
        return;
    
    delay(10);  // Reducido para respuesta más rápida
}