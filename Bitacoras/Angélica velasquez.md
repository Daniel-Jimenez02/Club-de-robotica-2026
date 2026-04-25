Nombre del equipo: Robojammers
Integrantes: Victoria Ciro Gallego, Manuela Salazar, Angelica Bolivar
Categoria: Torneo Robojam Reto Skill Drive
Materiales: Chasis (dos ruedas con motor independiente y una rueda loca, Arduino (Uno o Nano), Un puente H (como el L298N o L293D), Un módulo Bluetooth (HC-05/HC-06),Portabaterias, 2 Baterias de 6v, cables conectores macho hembra y hembra macho.

Aplicar conocimiento en codificacion, Demostrar nuestros conocimientos en codigo arduino y construir Robot para competencia RobotJam, Skilldrive, Aprender a programar para conexion bluetooth 



Esquema/diagrama (foto o dibujo simple).
cuadro de 20x20 cm, altura sin limites. Colores que contrasten para mejor visibilidad. El robot no puede usar sensores ni pinzas para guiarse ni sostener el objeto a empujar, las guías que pueda usar solo pueden ser de 3,5 cm de largo. Tiene que ser abierto y dejar visibilidad para el objeto, Las guías del robot deben cubrir la mitad del objeto. 
<img width="727" height="454" alt="esquema skill drive" src="https://github.com/user-attachments/assets/fa63d22b-5dea-408e-b842-bd753fd5efe2" />


Código (versión y cambios): qué y por qué.
Declarar motores y conexión bluetooth: conexión constante y fuerte con el control remoto o mando físico. posible desde celular o mando especifico (conexión wifi, infrarojo) 
 velocidad, encedido y apagado.
 
 Botón conectado a la bateria 
 código: 
 
métricas clave: tiempos, error, consistencia.

Lecciones aprendidas y siguiente ajuste.

 25/04/26
 Cambiamos el Arduino Uno a SP32 para poder tener conexiones bluetooth a control remoto y poder manejar el robot, Cambiamos conexiones ya que en el SP32 no hay las mismas conexiones que en Arduino Uno, se ha dificultado un poco colocar el SP32 ya que es mas grande y la placa es mas pequeña, se cambiaron los Pines: 
 Motor izquierdo 
 ENA: 7
 IN1: 2
 IN2: 3
 Motor derecho
 ENB: 6
 IN3: 4
 IN4: 5 

 modulo bluetooth: VCC: V1 
                   GND: GND 1
