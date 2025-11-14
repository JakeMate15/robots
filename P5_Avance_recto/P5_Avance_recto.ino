// === MOTORES ===
#define DIR_A 2 
#define PWM_A 5

#define DIR_B 4 
#define PWM_B 6

#define CERO 39              // PWM mínimo útil para vencer inercia

const bool MOTOR_B_INVERTIDO = true;

static inline int pwmConUmbral(int v) {
  // 0 debe ser 0; sólo eleva a CERO si es positivo y menor a CERO
  if (v <= 0) return 0;
  if (v < CERO) return CERO;
  if (v > 255) return 255;
  return v;
}

void avanza(int velA, int velB, int dirA, int dirB) {
  const int vA = pwmConUmbral(velA);
  const int vB = pwmConUmbral(velB);

  const int dirB_real = MOTOR_B_INVERTIDO ? (dirB ^ 1) : dirB;

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
#define TRIG_IZQ 7
#define ECHO_IZQ 11
#define TRIG_DER 9
#define ECHO_DER 8
#define TRIG_CENTRO 12
#define ECHO_CENTRO 13    // ideal: mover a otro pin si notas lecturas inestables

// ~0.0172 cm/us (distancia ida y vuelta ya compensada)
#define CM_POR_US 0.01723f

float distanciaCM(int trig, int echo) {
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  // timeout: 25ms ≈ ~4.3m; evita bloqueos
  unsigned long dur = pulseIn(echo, HIGH, 25000UL);
  if (dur == 0UL) return NAN;
  return dur * CM_POR_US;
}

float distanciaPared = 12.0f;
const int   velocidad   = 110;
const float K           = 3.0f;
const float limErr      = 25.0f;
const int   maxPWM      = 200;
const float tolerancia  = 0.5f;

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

  // Intento de calibración inicial (si falla, usa 12 cm)
  float d = distanciaCM(TRIG_DER, ECHO_DER);
  distanciaPared = (isnan(d) || !isfinite(d)) ? 12.0f : d;
}

void loop() {
  float dC   = distanciaCM(TRIG_CENTRO, ECHO_CENTRO);
  delay(10);
  float dDer = distanciaCM(TRIG_DER, ECHO_DER);
  delay(10);
  float dIz  = distanciaCM(TRIG_IZQ, ECHO_IZQ);

  // Freno por obstáculo al frente si la lectura es válida
  if (!isnan(dC) && isfinite(dC) && dC <= 15.0f) {
    detenerse();
    Serial.println("Obstaculo al frente -> STOP");
    delay(30);
    return;
  }

  int vA = velocidad;
  int vB = velocidad;

  // Control P usando pared derecha si la medida es válida
  if (!isnan(dDer) && isfinite(dDer)) {
    float error = dDer - distanciaPared;

    if (fabsf(error) < tolerancia) error = 0.0f;
    if (error >  limErr) error =  limErr;
    if (error < -limErr) error = -limErr;

    float u = K * error; // controlador P

    vA = constrain((int)(velocidad + u), 0, maxPWM);
    vB = constrain((int)(velocidad - u), 0, maxPWM);
  }

  // (Opcional) si también quieres usar la izquierda para centrarte, hazlo
  // sólo si AMBAS lecturas son válidas. Descomenta y ajusta pesos.
  /*
  if (!isnan(dDer) && isfinite(dDer) && !isnan(dIz) && isfinite(dIz)) {
    const float KR = 0.3f;
    float uL = KR * (dIz - distanciaPared);
    float uR = KR * (dDer - distanciaPared);
    vA = constrain((int)(velocidad + uR - uL), 0, maxPWM);
    vB = constrain((int)(velocidad - uR + uL), 0, maxPWM);
  }
  */  

  avanza(vA, vB, 1, 1);

  Serial.print("Frente: ");   Serial.print(dC);
  Serial.print("  Der: ");    Serial.print(dDer);
  Serial.print("  Izq: ");    Serial.print(dIz);
  Serial.print("  vA: ");     Serial.print(vA);
  Serial.print("  vB: ");     Serial.println(vB);

  delay(30);
}
