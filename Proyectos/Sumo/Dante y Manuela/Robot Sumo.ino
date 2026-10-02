//Nombramos los pines a utilizar.
//Pines para los sensores de infrarrojo
#define delIzq 3  //IR delantero izquierdo
#define delDer 2  //IR delantero derecha
#define traIzq 5  //IR trasero izquierdo
#define traDer 4  //IR trasero derecho


//Pines para el sensor ultrasonido
#define echo 7
#define triger 6

//Pines para puente H (motor A - Motor B)
#define adelante1 9   //in1
#define atras1 10     //in2
#define adelante2 12  //in3
#define atras2 11     //in4
//Creamos las variables necesarias
float tiempo = 0;
float distancia = 0;
int ir1 = 0;  //Delantero izquierda
int ir2 = 0;  //Delantero derecha
int ir3 = 0;  //Trasero izquierda
int ir4 = 0;  //Trasero derecha

void setup() {
  //Configuración de los pines
  pinMode(delDer, INPUT);
  pinMode(delIzq, INPUT);
  pinMode(traDer, INPUT);
  pinMode(traIzq, INPUT);
  pinMode(echo, INPUT);
  pinMode(triger, OUTPUT);
  pinMode(adelante1, OUTPUT);
  pinMode(atras1, OUTPUT);
  pinMode(adelante2, OUTPUT);
  pinMode(atras2, OUTPUT);

  Serial.begin(9600);
  //Iniciamos con ambos motores apagados
  digitalWrite(adelante1, LOW);
  digitalWrite(adelante2, LOW);
  digitalWrite(atras1, LOW);
  digitalWrite(atras2, LOW);
  Serial.write('Todo OK');
  delay(5000);
}
void adelante() {
  digitalWrite(adelante1, HIGH);
  digitalWrite(adelante2, HIGH);
  digitalWrite(atras1, LOW);
  digitalWrite(atras2, LOW);
}
void atras() {
  digitalWrite(adelante1, LOW);
  digitalWrite(adelante2, LOW);
  digitalWrite(atras1, HIGH);
  digitalWrite(atras2, HIGH);
  delay(1000);
}
void buscar_enemigo() {
  digitalWrite(adelante1, 255 / 3);
  digitalWrite(atras1, LOW);
  digitalWrite(adelante2, LOW);
  digitalWrite(atras2, LOW);
}

void loop() {

  // Leer sensores IR
  ir1 = digitalRead(delIzq);
  ir2 = digitalRead(delDer);
  ir3 = digitalRead(traIzq);
  ir4 = digitalRead(traDer);


  // Si detecta la línea
  if (ir1 == 0 && ir2 == 0 && ir3 == 1 && ir4 == 1) {
    atras();
  }

  else if (ir1 == 0 && ir2 == 1 && ir3 == 1 && ir4 == 1) {
    atras();
  } else if (ir1 == 1 && ir2 == 0 && ir3 == 1 && ir4 == 1) {
    atras();
  } else if (ir1 == 1 && ir2 == 1 && ir3 == 0 && ir4 == 0) {
    adelante();
  } else if (ir1 == 1 && ir2 == 1 && ir3 == 1 && ir4 == 0) {
    adelante();
  } else if (ir1 == 1 && ir2 == 1 && ir3 == 0 && ir4 == 1) {
    adelante();
  }

  else if (ir1 == 1 && ir2 == 1 && ir3 == 1 && ir4 == 1) {
    // Medir distancia SIEMPRE
    digitalWrite(triger, LOW);
    delayMicroseconds(4);
    digitalWrite(triger, HIGH);
    delayMicroseconds(10);
    digitalWrite(triger, LOW);

    tiempo = pulseIn(echo, HIGH);
    distancia = tiempo / 58;
    Serial.println(distancia);
    if (distancia < 30.0) {
      adelante();
    } else {
      buscar_enemigo();
    }
  } else {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(3000);
    digitalWrite(LED_BUILTIN, LOW);
    delay(3000);
  }
}
