//Pines a utilizar para los sensores IR y ultrasonido
const int del_izq=9;
const int del_der=8;
const int tra_izq=10;
const int tra_der=11;
const int echo=7;
const int triger=6;

//Variables necesarias
int ir1=0;
int ir2=0;
int ir3=0;
int ir4=0;
float tiempo=0;
float distancia=0;

void setup() {
  Serial.begin(9600);
  pinMode(del_izq,INPUT);
  pinMode(del_der,INPUT);
  pinMode(tra_izq,INPUT);
  pinMode(tra_der,INPUT);
  pinMode(echo,INPUT);
  pinMode(triger,OUTPUT);
}

void loop() {
  //Iniciamos con la función para medir el tiempo y calcular la distancia
  digitalWrite(triger,0);
  delayMicroseconds(4);
  digitalWrite(triger,1);
  delayMicroseconds(10);
  digitalWrite(triger,0);

  tiempo=pulseIn(echo,HIGH);
  distancia=tiempo/58;      //Lugar donde se almacena la distancia medida en cm  

  //Lectura de los senosres infrarrojos
  ir1=digitalRead(del_izq);
  ir2=digitalRead(del_der);
  ir3=digitalRead(tra_der);
  ir3=digitalRead(tra_izq);

  Serial.print("IR delantero izquierda: ");
  Serial.println(ir1);
  Serial.print("IR delantero derecha: ");
  Serial.println(ir2);  
  Serial.print("IR tracero izquierdo: ");
  Serial.println(ir3);  
  Serial.print("IR tracero derecho: ");
  Serial.println(ir4);  
  Serial.print("Distancia: ");
  Serial.print(distancia);
  Serial.println(" cm");
  Serial.println("___________________________________________");
  delay(500);  
}
