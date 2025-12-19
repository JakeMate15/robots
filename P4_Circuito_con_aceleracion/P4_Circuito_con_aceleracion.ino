#define DIR_A 2 
#define PWM_A 5

#define DIR_B 4 
#define PWM_B 6

#define VELOCIDAD 145
#define TIEMPO_AVANCE 500
#define TIEMPO_GIRO 1175
#define DESVIO 20
#define DESVIO2 25

void mover(int dirDerecha, int pwmDerecha, int dirIzquierda, int pwmIzquierda) {
  digitalWrite(DIR_A, dirDerecha);
  analogWrite(PWM_A, pwmDerecha);

  digitalWrite(DIR_B, dirIzquierda ^ 1);
  analogWrite(PWM_B, pwmIzquierda);
}

void avanzar_recto() {
  mover(1, VELOCIDAD - DESVIO, 1, VELOCIDAD + DESVIO);
}

void avanzar_recto2(int velocidad) {
  mover(1, velocidad - DESVIO2, 1, velocidad + DESVIO2);
}

void detenerse() {
  mover(0, 0, 0, 0);
  delay(1000);
}

void girar_derecha() {
  mover(0, 75, 1, 75);
}

void girar_izquierda() {
  mover(1, 75, 0, 75);
}

void circuito() {
    // Avanza
  avanzar_recto2(210);
  delay(210);
  detenerse();

  girar_derecha();
  delay(TIEMPO_GIRO - 10);
  detenerse();

  avanzar_recto();
  delay(TIEMPO_AVANCE);
  detenerse();

  girar_izquierda();
  delay(TIEMPO_GIRO);
  detenerse();

  avanzar_recto();
  delay(TIEMPO_AVANCE + 500);
  detenerse();
  
  girar_izquierda();
  delay(TIEMPO_GIRO - 50);
  detenerse();
  
  avanzar_recto();
  delay(TIEMPO_AVANCE);
  detenerse();

  girar_derecha();
  delay(TIEMPO_GIRO);
  detenerse();

  avanzar_recto2(210);
  delay(210);
  detenerse();
}

void setup() {
  pinMode(DIR_A, OUTPUT);
  pinMode(PWM_A, OUTPUT);
  pinMode(DIR_B, OUTPUT);
  pinMode(PWM_B, OUTPUT);
  delay(2000);
}

void loop() {  
  // girar_derecha();
  // girar_izquierda();
  // delay(1175);

   // avanzar_recto2(210);
   // delay(250);


   // detenerse();
   // delay(500);

  circuito();

  girar_derecha();
  delay(TIEMPO_GIRO * 2 + 200);
  detenerse();

  // girar_derecha();
  // delay(TIEMPO_GIRO);
  // detenerse();

  circuito();
  detenerse();

  while(true) {
  }
}