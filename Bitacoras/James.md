OBJETIVO DE COMPETENCIA:
  El robot debe completar una secuencia de una linea negra sin salirse, el equipo que complete en el menor tiempo posible ganara la competencia.

MATERIALES:
  Arduino Nano
  2 motores
  puente h
  3 sensores inflarrojo
  2 llantas
  lipo
  interruptor

CIRCUITO:
 <img width="1342" height="1172" alt="607635522-58ea5a85-73d3-4689-bcb5-e018a735ce3d" src="https://github.com/user-attachments/assets/9478824d-bbce-4d6c-9603-ec93e9c5b00f" />

CODIGO:

  /*
Equipo de robotica Biblioteca EPM
codigo seguidor de linea 30/10/2025
puente H conectado con los enable activos   
l298N--Arduino
  en1--6
  in1--13
  in2--12
  in3--11
  in4--10
  en2--5
Sensores de CRT-5000
s1--9 izquierda
s2--8 centro
s3--7 derecha
*/

#define en1 5
#define m1a 13
#define m1t 12
#define m2a 10
#define m2t 11
#define en2 6
#define s1 8
#define s2 4
#define s3 9
int vs1=0;
int vs2=0;
int vs3=0;

void detener(){
  digitalWrite(m1a,LOW);
  digitalWrite(m1t,LOW);
  digitalWrite(m2a,LOW);
  digitalWrite(m2t,LOW);
}
void avanzar(){
  analogWrite(en1,200);
  analogWrite(en2,200);
  digitalWrite(m1a,HIGH);
  digitalWrite(m1t,LOW);
  digitalWrite(m2a,HIGH);
  digitalWrite(m2t,LOW);
}
void retroceder(){
  analogWrite(en1,200);
  analogWrite(en2,200);
  digitalWrite(m1a,LOW);
  digitalWrite(m1t,HIGH);
  digitalWrite(m2a,LOW);
  digitalWrite(m2t,HIGH);
}
void girar_der(){
  analogWrite(en1,200);
  analogWrite(en2,200);
  digitalWrite(m1a,LOW);
  digitalWrite(m1t,LOW);
  digitalWrite(m2a,HIGH);
  digitalWrite(m2t,LOW);
}
void girar_izq(){  
  analogWrite(en1,200);
  analogWrite(en2,200);
  digitalWrite(m1a,HIGH);
  digitalWrite(m1t,LOW);
  digitalWrite(m2a,LOW);
  digitalWrite(m2t,LOW);
}
void lectura_sensores(){
  vs1=digitalRead(s1);
  vs2=digitalRead(s2);
  vs3=digitalRead(s3);
  Serial.println(vs1); //Izquierda
  Serial.println(vs2); // centro
  Serial.println(vs3); // derecha
  Serial.println("////////");
  delay(100);
  
}
void setup() {
  pinMode(m1a,OUTPUT);
  pinMode(m1t,OUTPUT);
  pinMode(m2a,OUTPUT);
  pinMode(m2t,OUTPUT);
  pinMode(en1,OUTPUT);
  pinMode(en2,OUTPUT);
  pinMode(s1,INPUT);
  pinMode(s2,INPUT);
  pinMode(s3,INPUT);
  Serial.begin(9600);
  detener();
  
}
void loop() {
  lectura_sensores();
  
  if(vs1==0 && vs2==1 && vs3==0){
    avanzar();
  }
  else if(vs1==0 && vs2==1 && vs3==1){
    analogWrite(en1,200);
    analogWrite(en2,200);
    digitalWrite(m1a,LOW);
    digitalWrite(m1t,LOW);
    digitalWrite(m2a,HIGH);
    digitalWrite(m2t,LOW);
  }
  else if(vs1==1 && vs2==1 && vs3==0){
    girar_der();
  }
  else{
    girar_izq();
  }

}
