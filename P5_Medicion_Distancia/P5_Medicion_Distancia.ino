// MOTORES
#define DIR_A 2 
#define PWM_A 5

#define DIR_B 4 
#define PWM_B 6

#define VELOCIDAD 30
#define DESVIO 20
#define DESVIO2 25

// SENSOR ULTRASONICO
#define TRIG 12
#define ECHO 13
#define velSonido 0.035103

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

float distancia() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  unsigned long dur = pulseIn(ECHO, HIGH);
  return (dur * velSonido) * 0.5f;
}

void setup() {
  pinMode(DIR_A, OUTPUT);
  pinMode(PWM_A, OUTPUT);
  pinMode(DIR_B, OUTPUT);
  pinMode(PWM_B, OUTPUT);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  delay(2000);
  Serial.begin(9600);
}

void loop() {  
  float d = distancia();
  Serial.println(d);

  if (d > 15.0f) {
    avanzar_recto2(70);
    Serial.println(" avanzar");
  } else {
    detenerse();
    Serial.println(" detenerse");
  }



  delay(100);
}