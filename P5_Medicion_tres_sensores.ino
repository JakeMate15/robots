// === MOTORES ===
#define DIR_A 2 
#define PWM_A 5

#define DIR_B 4 
#define PWM_B 6

const bool MOTOR_B_INVERTIDO = true;

int normPWM(int v) {
  if (v < 0)   return 0;
  if (v > 255) return 255;
  return (int)v;
}

void avanza(int velA, int velB, int dirA, int dirB) {
  int vA = normPWM(velA);
  int vB = normPWM(velB);

  int dirB_real = MOTOR_B_INVERTIDO ? (dirB ^ 1) : dirB;

  digitalWrite(DIR_A, dirA);
  digitalWrite(DIR_B, dirB_real);
  analogWrite(PWM_A, vA);
  analogWrite(PWM_B, vB);
}

void delante(int vel)            { avanza(vel, vel, 1, 1); }
void retroceder(int vel)         { avanza(vel, vel, 0, 0); }
void abiertoDerecha(int vel)     { avanza(vel,   0, 1, 1); } // rueda izq avanza
void abiertoIzquierda(int vel)   { avanza(0,   vel, 1, 1); } // rueda der avanza
void cerradoDerecha(int vel)     { avanza(vel, vel, 1, 0); } // giro sobre eje
void cerradoIzquierda(int vel)   { avanza(vel, vel, 0, 1); }
void detenerse()                 { avanza(0, 0, 0, 0); }

// === ULTRASÓNICOS (HC-SR04) ===
#define TRIG_IZQ 8
#define ECHO_IZQ 9
#define TRIG_DER 10
#define ECHO_DER 11
#define TRIG_CENTRO 12
#define ECHO_CENTRO 13
#define VEL_SONIDO 0.035103f

float distanciaCM(int trig, int echo) {
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  unsigned long dur = pulseIn(echo, HIGH);
  if (dur == 0) return NAN;
  return (dur * VEL_SONIDO_CM_US) * 0.5f;
}

void setup() {
  // Motores
  pinMode(DIR_A, OUTPUT);
  pinMode(PWM_A, OUTPUT);
  pinMode(DIR_B, OUTPUT);
  pinMode(PWM_B, OUTPUT);

  // Sensores
  pinMode(TRIG_CENTRO, OUTPUT);
  pinMode(ECHO_CENTRO, INPUT);

  pinMode(TRIG_IZQ, OUTPUT);
  pinMode(ECHO_IZQ, INPUT);

  pinMode(TRIG_DER, OUTPUT);
  pinMode(ECHO_DER, INPUT);

  digitalWrite(TRIG_CENTRO, LOW);
  digitalWrite(TRIG_IZQ, LOW);
  digitalWrite(TRIG_DER, LOW);

  Serial.begin(9600);
  delay(300);
}

void loop() {  
  float dC = distanciaCM(TRIG_CENTRO, ECHO_CENTRO);
  float dL = distanciaCM(TRIG_IZQ, ECHO_IZQ);
  float dR = distanciaCM(TRIG_DER, ECHO_DER);

  Serial.print(F("L: ")); Serial.print(dL);
  Serial.print(F("  C: ")); Serial.print(dC);
  Serial.print(F("  R: ")); Serial.println(dR);
}