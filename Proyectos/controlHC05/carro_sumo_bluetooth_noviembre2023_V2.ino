/*
  Código para controlar un robot por Bluetooth con una placa Innobot por medio de una aplicación desarrollada en App Inventor llamada Control Robluetooth FEPM.
  El robot espera recibir las letras w, a, s, d o p para avanzar, girar a izquierda, girar a derecha, retroceder o detenerse respectivamente.
  Los motores del robot deben estar conectados a las borneras M1 y M2 de la placa Innobot.

  Desarrollado por el equipo de robótica de la Fundación EPM en el laboratorio de Robóticafe, para uso libre.

  7/11/2023
*/

/*
  Componentes electrónicos requeridos:
  1 Arduino (Placa Innobot)
  1 Antena Bluetooth HC-05 (También compatible con el módulo Bluetooth HC-06)
  Cables para las conexiones
*/

/*
  Conexiones de los componentes

  Arduino-----------HC-05
  5V----------------Vcc
  GND---------------GND
  D8----------------Rx
  D9----------------Tx
*/

#include <SoftwareSerial.h>//Librería para configurar otra comunicación serial del Arduino
//Se definen los pines digitales del Arduino para el Rx y Tx que se configurará
#define Rx 9
#define Tx 8
#define adelante1 2
#define atras1  3
#define adelante2 4
#define atras2 5

SoftwareSerial Bluetooth(Rx, Tx);//Se configuran los pines Rx y Tx para la comunicación serial

void setup() {
  Bluetooth.begin(9600);//Inicia la comunicación serial
  pinMode(adelante1,OUTPUT);
  pinMode(adelante2,OUTPUT);
  pinMode(atras1,OUTPUT);
  pinMode(atras2,OUTPUT);
  Serial.begin(9600);
  }

void loop() {
  if(Bluetooth.available()){//Mientras esté habilitada la comunicación serial
  char dato = Bluetooth.read();//Se crea variable en la que se almacenará la lectura de la letra recibida por Bluetooth de la app 
  Serial.println(dato);
  
 if(dato == 'w'){
  digitalWrite(adelante1,HIGH);
  digitalWrite(atras1,LOW);
  digitalWrite(adelante2,HIGH);
  digitalWrite(atras2,LOW);
  Serial.println("AVANZA");
  }
  
  else if(dato=='a'){
    digitalWrite(adelante1,LOW);
    digitalWrite(atras1,HIGH);
    digitalWrite(adelante2,HIGH);
    digitalWrite(atras2,LOW);
    Serial.println("gira izquierda");
    }

   else if(dato=='s'){
    digitalWrite(adelante1,LOW);
    digitalWrite(atras1,HIGH);
    digitalWrite(adelante2,LOW);
    digitalWrite(atras2,HIGH);
    Serial.println("retrocede");
    }
  else if(dato == 'd'){
    digitalWrite(adelante1,HIGH);
    digitalWrite(atras1,LOW);
    digitalWrite(adelante2,LOW);
    digitalWrite(atras2,HIGH);
    Serial.println("gira derecha");
    }
    else if(dato == 'p'){
    digitalWrite(adelante1,LOW);
    digitalWrite(atras1,LOW);
    digitalWrite(adelante2,LOW);
    digitalWrite(atras2,LOW); 
    Serial.println(" parar");
    }
    else{    
      digitalWrite(adelante1,LOW);
      digitalWrite(atras1,LOW);
      digitalWrite(adelante2,LOW);
      digitalWrite(atras2,LOW);
  }
 }
}
