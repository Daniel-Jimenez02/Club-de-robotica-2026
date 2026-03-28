//ROBOT DANIEL.COM
//PRUEBA UNITARIA DE MOTORES
//ULTIMA MODIFICACIÓN: 28-02-26

//---------------------------------------------PINES-----------------------------------------------------
int M1_A = 25;
int M1_B = 33;
int M2_A = 32;
int M2_B = 35;
int ENA = 26;
int ENB = 34;
//------------------------------------------CONFIGURACIÓN--------------------------------------------------
void setup() {
pinMode(M1_A, OUTPUT);
pinMode(M1_B, OUTPUT);
pinMode(M2_A, OUTPUT);
pinMode(M2_B, OUTPUT);
pinMode(PWM_1, OUTPUT);
pinMode(PWM_2, OUTPUT);
}


//---------------------------------------------FUNCIONES-----------------------------------------------------

void Avanzar() {
digitalWrite(M1_A,1);
digitalWrite(M1_B,0);
digitalWrite(M2_A,1);
digitalWrite(M2_B,0);
}

void Retroceder() {
digitalWrite(M1_A,0);
digitalWrite(M1_B,1);
digitalWrite(M2_A,0);
digitalWrite(M2_B,1);
}

void GiroDerecha() {
digitalWrite(M1_A,1);
digitalWrite(M1_B,0);
digitalWrite(M2_A,0);
digitalWrite(M2_B,1);
}

void GiroIzquierda() {
digitalWrite(M1_A,0);
digitalWrite(M1_B,1);
digitalWrite(M2_A,1);
digitalWrite(M2_B,0);
}

//---------------------------------PROCEDIMIENTOS-------------------------------------
void loop() {
GiroIzquierda();
delay(2000);
GiroDerecha();
delay(2000);
Retroceder();
delay(500);
Avanzar();
delay(2000);
}
