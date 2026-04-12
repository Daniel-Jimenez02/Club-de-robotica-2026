1. COMPETENCIA BUMPERBOT EVOLUTION
  Se debe crear y programar un robot que retire ciertos obstáculos(10 Vasos) de una pista, en un tiempo de 2 minutos, sin salirse de la pista mientras lo hace.
2. MATERIALES ELECTRONICA:
 - 1 Puente H
 -1 Bateria lipo
 -4 Sensores infrarrojo
 -2 Motores (TT-MOT-350RPM-ENCO)
 -1 Switch electrico
 -jumper wires
 -1 Ultrasonido
 - Giroscopio Arduino(MPU-6050)
3. OBJETOS:
 - 1 Rueda giratoria
-  2 Ruedas
4. DIAGRAMA DE CIRCUITO:
 <img width="2664" height="2325" alt="EPM_bb" src="https://github.com/user-attachments/assets/375fd555-426a-46d1-9e05-8d340d934e7b" />


5. CODIGO:
  // Pinos del puente H #definir IN1 8 #definir IN2 9 #definir ENA 10 #definir ENA2 11 // segundo ENA
  
  // Codificador #define encoderA 2 #define encoderB 3
  
  pulsos largos volátiles = 0;
  
  int velocidad = 200;
  
  // Ajustar según tu motor float pulsos_por_vuelta = 330;

  // Radio de la rueda en cm float radio_rueda = 3.0;
  
  configuración vacía() {
  
  Serie.begin(9600);
  
  pinMode(IN1, SALIDA); pinMode(IN2, SALIDA); pinMode(ENA, SALIDA); pinMode(ENA2, SALIDA);
  
  pinMode(codificadorA, INPUT); pinMode(codificadorB, INPUT);
  
  attachInterrupt(digitalPinToInterrupt(encoderA), contar, RISING);
  
  Serial.println("Sistema listo"); Serial.println("Comandos:"); Serial.println("F velocidad → Adelante"); Serial.println("B velocidad → Atras"); Serial.println("S → Detener"); Serial.println("R → Restablecer codificador"); Serial.println("D → Distancia"); Serial.println("V → Vueltas");
  
  }
  
  void loop() {
  
  si (Serial.disponible()) {
  
  String comando = Serial.readStringUntil('\n');
  
  char orden = comando.charAt(0);
  
  int vel = comando.substring(1).toInt();
  
  interruptor (orden) {
  
  caso 'F': adelante(vel); Serial.println("Motor adelante"); romper;
  
  caso 'B': atras(vel); Serial.println("Atras del motor"); romper;
  
  caso 'S': parar(); Serial.println("Motor detenido"); romper;
  
  caso 'R': pulsos = 0; Serial.println("Codificador reiniciado"); romper;
  
  case 'V': mostrarVueltas(); romper;
  
  case 'D': mostrarDistancia(); romper;
  
  }
  
  }
  
  }
  
  // Interrupción del codificador void contar() {
  
  if (digitalRead(encoderB) == HIGH) pulsos++; else pulsos--;
  
  }
  
  // Motor adelante void adelante(int vel) {
  
  escritura digital (IN1, ALTA); escritura digital (IN2, BAJO); escrituraanalógica(ENA, vel); escrituraanalógica(ENA2, vel);
  
  }
  
  // Motor atrás void atras(int vel) {
  
  escritura digital (IN1, BAJO); escritura digital (IN2, ALTA); escrituraanalógica(ENA, vel); escrituraanalógica(ENA2, vel);
  
  }
  
  // Detener void parar() {
  
  escritura digital (IN1, BAJO); escritura digital (IN2, BAJO); escritura analógica(ENA, 0); escritura analógica(ENA2, 0);
  
  }
  
  // Mostrar vueltas void mostrarVueltas() {
  
  float vueltas = pulsos / pulsos_por_vuelta;
  
  Serial.print("Vueltas: "); Serial.println(vueltas);
  
  }
  
  // Mostrar distancia void mostrarDistancia() {
  
  float vueltas = pulsos / pulsos_por_vuelta;

  
  float distancia = vueltas * 2 * 3.1416 * radio_rueda;
  
  Serial.print("Distancia recorrida (cm): "); Serial.println(distancia);
  
  }
DISEÑO 3D:

