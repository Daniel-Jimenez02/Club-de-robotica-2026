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
#define m1a 12
#define m1t 13
#define m2a 11
#define m2t 10
#define en2 6
#define s1 7
#define s2 8
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
  analogWrite(en1,255);
  analogWrite(en2,235);
  digitalWrite(m1a,HIGH);
  digitalWrite(m1t,LOW);
  digitalWrite(m2a,HIGH);
  digitalWrite(m2t,LOW);
}
void girar_der(){
  analogWrite(en1,255);
  analogWrite(en2,235);
  digitalWrite(m1a,LOW);
  digitalWrite(m1t,LOW);
  digitalWrite(m2a,HIGH);
  digitalWrite(m2t,LOW);
}
void girar_izq(){  
  analogWrite(en1,255);
  analogWrite(en2,235);
  digitalWrite(m1a,HIGH);
  digitalWrite(m1t,LOW);
  digitalWrite(m2a,LOW);
  digitalWrite(m2t,LOW);
}
void lectura_sensores(){
  vs1=digitalRead(s1);
  vs2=digitalRead(s2);
  vs3=digitalRead(s3);
  Serial.println(vs1);
  Serial.println(vs2);
  Serial.println(vs3);
  Serial.println("////////");
  
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
  else if(vs1==1 && vs2==0 && vs3==0){
    girar_izq();
  }
  else if(vs1==0 && vs2==0 && vs3==1){
    girar_der();
  }
  else if(vs1==1 && vs2==1 && vs3==0){
    girar_izq();
  }
  else if(vs1==0 && vs2==1 && vs3==1){
    girar_der();
  }
  else{
    girar_izq();
  }

}
