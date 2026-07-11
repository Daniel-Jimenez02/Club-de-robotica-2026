/*/
                                                 Competencia Seguidor de linea velocista
Codigo de funcionamiento del Robot
Equipo: Alejo, Victoria, Lorenzo
Ultima fecha de modificación: 20/06/26

/*/


//<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>--DEFINIR--<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>

  //MOTORES
#define MD1 3
#define MD2 4
#define MI1 5
#define MI2 6

  //SENSORES
#define s1 10
#define s2 9
#define s3 8

  //PWM
#define ENA 2
#define ENB 7

  //VARIABLES
int vs1=0;
int vs2=0;
int vs3=0;


//<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>--ACCIONES--<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>><><><><><
void detener(){
  digitalWrite(MD1,LOW);
  digitalWrite(MD2,LOW);
  digitalWrite(MI1,LOW);
  digitalWrite(MI2,LOW);
}
void avanzar(){
  analogWrite(ENA,200);
  analogWrite(ENB,200);
  digitalWrite(MD1,HIGH);
  digitalWrite(MD2,LOW);
  digitalWrite(MI1,HIGH);
  digitalWrite(MI2,LOW);
}
void retroceder(){
  analogWrite(ENA,200);
  analogWrite(ENB,200);
  digitalWrite(MD1,LOW);
  digitalWrite(MD2,HIGH);
  digitalWrite(MI1,LOW);
  digitalWrite(MI2,HIGH);
}
void girar_der(){
  analogWrite(ENA,200);
  analogWrite(ENB,200);
  digitalWrite(MD1,LOW);
  digitalWrite(MD2,LOW);
  digitalWrite(MI1,HIGH);
  digitalWrite(MI2,LOW);
}
void girar_izq(){  
  analogWrite(ENA,80);
  analogWrite(ENB,80);
  digitalWrite(MD1,HIGH);
  digitalWrite(MD2,LOW);
  digitalWrite(MI1,LOW);
  digitalWrite(MI2,LOW);
}
void lectura_sensores(){
  vs1=digitalRead(s1);
  vs2=digitalRead(s2);
  vs3=digitalRead(s3);
  Serial.println(vs1);
  Serial.println(vs2);
  Serial.println(vs3);
  Serial.println("////////");
  delay(100);
} 

//<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>--BASE--<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>><><><><>

void setup() {
  pinMode(MD1,OUTPUT);
  pinMode(MD2,OUTPUT);
  pinMode(MI1,OUTPUT);
  pinMode(MI2,OUTPUT);
  pinMode(ENA,OUTPUT);
  pinMode(ENB,OUTPUT);
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
    girar_der();
  }
  else if(vs1==0 && vs2==0 && vs3==1){
    girar_izq();
  }
  else{
    girar_izq();
  }

}
