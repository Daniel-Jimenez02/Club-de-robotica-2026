//Nombramos los pines a utilizar.
//Pines para los sensores de infrarrojo
#define delIzq 2  //IR delantero izquierdo
#define delDer 3  //IR delantero derecha
#define traIzq 4  //IR trasero izquierdo
#define traDer 5  //IR trasero derecho


//Pines para el sensor ultrasonido
#define echo 7
#define triger 6

//Pines para puente H (motor A - Motor B)
#define adelante1 12  //in1
#define atras1 11     //in2
#define adelante2 10  //in3
#define atras2 9      //in4

//Creamos las variables necesarias
float tiempo=0;
float distancia=0;
int ir1=0;      //Delantero izquierda
int ir2=0;      //Delantero derecha
int ir3=0;      //Trasero izquierda
int ir4=0;      //Trasero derecha

void setup() {
  //Configuración de los pines 
  pinMode(delDer,INPUT);
  pinMode(delIzq,INPUT);
  pinMode(traDer,INPUT);
  pinMode(traIzq,INPUT);
  pinMode(echo,INPUT);
  pinMode(triger,OUTPUT);
  pinMode(adelante1,OUTPUT);
  pinMode(atras1,OUTPUT);  
  pinMode(adelante2,OUTPUT);
  pinMode(atras2,OUTPUT);  

  Serial.begin(9600);
  //Iniciamos con ambos motores apagados
  digitalWrite(adelante1,LOW);
  digitalWrite(adelante2,LOW);
  digitalWrite(atras1,LOW);
  digitalWrite(atras2,LOW);
  delay(5000); 
}

void loop() {
  //Iniciamos con la función para medir la distancia con el ultrasonido
  digitalWrite(triger,LOW);
  delayMicroseconds(4);
  digitalWrite(triger,HIGH);
  delayMicroseconds(10);
  digitalWrite(triger,LOW);

  tiempo=pulseIn(echo,HIGH);
  distancia=tiempo/58;      //Lugar donde se almacena la distancia medida en cm

  //Hacemos la lectura de cada uno de los sensores infrarrojos
  ir1=digitalRead(delIzq);
  ir2=digitalRead(delDer);
  ir3=digitalRead(traIzq);
  ir4=digitalRead(traDer);
  
  //Código principal del robot sumo
  //Condición en la que los 4 sensores de infrarrojo estan sobre la pista negra
  if (ir1==0 && ir2==0 && ir3==0 && ir4==0){
      if (distancia<30.0){                    //Condición que evalua la distancia del otro robot sumo
        digitalWrite(adelante1,HIGH);
        digitalWrite(adelante2,HIGH);
        digitalWrite(atras1,LOW);
        digitalWrite(atras2,LOW);    
      }
      else{
        digitalWrite(adelante1,LOW);         //Si no tiene un objetivo en la distancia que le definimos da vueltas para encontrarlo
        analogWrite(adelante2,255/3);
        digitalWrite(atras1,LOW);
        digitalWrite(atras2,LOW);      
      }
    }
    else if(ir1==1 && ir2==1 && ir3==0 && ir4==0){
        digitalWrite(adelante1,LOW);
        digitalWrite(adelante2,LOW);
        digitalWrite(atras1,HIGH);
        digitalWrite(atras2,HIGH);
        delay(700); 
    }
    else if(ir1==0 && ir2==0 && ir3==1 && ir4==1){
        digitalWrite(adelante1,HIGH);
        digitalWrite(adelante2,HIGH);
        digitalWrite(atras1,LOW);
        digitalWrite(atras2,LOW);
        delay(700);
    }       
}
