#define DIR_A 2 
#define PWM_A 5
#define DIR_B 4 
#define PWM_B 6

#define CERO 39
#define VELOCIDAD 200
#define TIEMPO_AVANCE 666
#define TIEMPO_GIRO 350
#define DESVIO 20

void avanza(int velA, int velB, int dirA, int dirB) {
  digitalWrite(DIR_A, dirA);
  digitalWrite(DIR_B, dirB ^ 1);
  analogWrite(PWM_A, velA);
  analogWrite(PWM_B, velB);
  // Serial.print("Velocidad: ");
  // Serial.println(vel);
  delay(10);
}

void delante(int vel) {
  avanza(vel, vel, 1, 1);
}

void retroceder(int vel) {
  avanza(vel, vel, 0, 0);
}

void abiertoDerecha(int vel) {
  avanza(vel, 0, 1, 1);
}

void abiertoIzquierda(int vel) {
  avanza(0, vel, 1, 1);
}

void cerradoDerecho(int vel) {
  avanza(vel, vel, 1, 0);
}

void cerradoIzquierda(int vel) {
  avanza(vel, vel, 0, 1);
}

void detenerse() {
  avanza(0, 0, 0, 0);
}

void setup() {
  pinMode(DIR_A, OUTPUT);
  pinMode(PWM_A, OUTPUT);
  pinMode(DIR_B, OUTPUT);
  pinMode(PWM_B, OUTPUT);

  Serial.begin(115200);
}

void loop() {  
  for (int i = CERO; i < 255; i++) {
    delante(i);
  }
  
  detenerse();
  delay(1000);

  for (int i = CERO; i < 255; i++) {
    retroceder(i);
  } 

  detenerse();
  delay(1000);

  for (int i = CERO; i < 255; i++) {
    abiertoDerecha(i);
  }

  detenerse();
  delay(1000);

  for (int i = CERO; i < 255; i++) {
    abiertoIzquierda(i);
  }

  detenerse();
  delay(1000);

  for (int i = CERO; i < 255; i++) {
    cerradoDerecho(i);
  } 

  detenerse();
  delay(1000);

  for (int i = CERO; i < 255; i++) {
    cerradoIzquierda(i);
  }   
}