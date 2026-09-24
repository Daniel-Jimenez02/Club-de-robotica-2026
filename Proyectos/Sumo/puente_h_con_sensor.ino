const int Trigger = 7;   //Pin digital 7 para el Trigger del sensor
const int Echo = 6;   //Pin digital 6 para el Echo del sensor
const int Adelante1=3; //los  pines del puente h
const int Atras1=5;
const int Adelante2=2; 
const int Atras2=4;

void setup() {
  Serial.begin(9600);//iniciailzamos la comunicación
  pinMode(Trigger, OUTPUT); //pin como salida
  pinMode(Echo, INPUT);  //pin como entrada
  digitalWrite(Trigger, LOW);//Inicializamos el pin con 0
  pinMode(Adelante1,OUTPUT);  //pines del puente h como salida 
  pinMode(Atras1,OUTPUT);
  pinMode(Adelante2,OUTPUT);   
  pinMode(Atras2,OUTPUT);
  //inicializamos las salidas del puente h en un nivel logico bajo (0)
  digitalWrite(Adelante1,LOW);   
  digitalWrite(Atras1,LOW);
  digitalWrite(Adelante2,LOW);   
  digitalWrite(Atras2,LOW);
}

void loop()
{

  long t; //timepo que demora en llegar el eco
  long d; //distancia en centimetros

  digitalWrite(Trigger, HIGH);
  delayMicroseconds(10);          //Enviamos un pulso de 10us
  digitalWrite(Trigger, LOW);
  
  t = pulseIn(Echo, HIGH); //obtenemos el ancho del pulso
  d = t/59;             //escalamos el tiempo a una distancia en cm
  
  Serial.print("Distancia: ");
  Serial.print(d);      //Enviamos por monitor seria el valor de la distancia
  Serial.print("cm");
  Serial.println();
  delay(100);          //Hacemos una pausa de 100ms

  if(d<=10){
  // pin 1 del puente h en 1
  digitalWrite(Adelante1,HIGH);
  // pin 2 del puente h en 0 
  digitalWrite(Atras1,LOW);   
  
  // pin 3 del puente h en 1
  digitalWrite(Adelante2,HIGH);
  // pin 4 del puente h en 0 
  digitalWrite(Atras2,LOW);
    }
  else{
  // pin 1 del puente h en 0
  digitalWrite(Adelante1,LOW); 
  // pin 2 del puente h en 0
  digitalWrite(Atras1,LOW);    
  // pin 3 del puente h en 0
  digitalWrite(Adelante2,LOW); 
  // pin 4 del puente h en 0
  digitalWrite(Atras2,LOW);  
    }
}
