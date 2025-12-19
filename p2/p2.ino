#define DIR_A 2 
#define PWM_A 5

#define DIR_B 4 
#define PWM_B 6

#define VELOCIDAD 200
#define TIEMPO_AVANCE 666
#define TIEMPO_GIRO 350
#define DESVIO 20

void mover(int dirDerecha, int pwmDerecha, int dirIzquierda, int pwmIzquierda) {
  digitalWrite(DIR_A, dirDerecha);
  analogWrite(PWM_A, pwmDerecha);

  digitalWrite(DIR_B, dirIzquierda ^ 1);
  analogWrite(PWM_B, pwmIzquierda);
}

void avanzar_recto() {
  mover(1, VELOCIDAD - DESVIO, 1, VELOCIDAD + DESVIO);
}

void detenerse() {
  mover(0, 0, 0, 0);
  delay(1000);
}

void girar_derecha() {
  mover(0, VELOCIDAD - DESVIO, 1, VELOCIDAD + DESVIO);
}

void girar_izquierda() {
  mover(1, VELOCIDAD + DESVIO, 0, VELOCIDAD - DESVIO);
}

void circuito() {
    // Avanza
  avanzar_recto();
  delay(TIEMPO_AVANCE);
  detenerse();

  girar_derecha();
  delay(TIEMPO_GIRO);
  detenerse();

  avanzar_recto();
  delay(TIEMPO_AVANCE);
  detenerse();

  girar_izquierda();
  delay(TIEMPO_GIRO - 50);
  detenerse();

  avanzar_recto();
  delay(TIEMPO_AVANCE);
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

  avanzar_recto();
  delay(TIEMPO_AVANCE);
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
  circuito();

  girar_derecha();
  delay(TIEMPO_GIRO);
  detenerse();

  girar_derecha();
  delay(TIEMPO_GIRO);
  detenerse();

  circuito();

  while(true) {
  }
}