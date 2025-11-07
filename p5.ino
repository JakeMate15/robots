#define TRIG 12
#define ECHO 13

float velSonido = 0.0343f;
const float dReal = 40.0f;

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
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  Serial.begin(9600);
}

float distanciaPromediada(uint8_t n) {
  (void)distancia();
  delay(50);
  float sum = 0.0f;
  uint8_t ok = 0;
  for (uint8_t i = 0; i < n; i++) {
    float d = distancia();
    if (d > 0.1f) { sum += d; ok++; }
    delay(60);
  }
  return ok ? (sum / ok) : 0.0f;
}

void loop() {
  float lo = 0.030f, hi = 0.040f;
  for (int iter = 0; iter < 30; iter++) {
    velSonido = (lo + hi) * 0.5f;
    float d = distanciaPromediada(6);
    Serial.print(F("Iter "));
    Serial.print(iter);
    Serial.print(F(" | v="));
    Serial.print(velSonido, 6);
    Serial.print(F(" cm/us | d="));
    Serial.print(d, 2);
    Serial.print(F(" cm | objetivo="));
    Serial.println(dReal, 2);
    if (d > dReal) hi = velSonido;
    else lo = velSonido;
  }
  Serial.print(F("V final: "));
  Serial.print(velSonido, 6);
  Serial.print(F(" cm/us | d medida: "));
  Serial.println(distancia(), 2);

  while(true);
  
  for (;;) {
    // float d = distanciaPromediada(5);
    float d = distancia();
    Serial.print(F("Distancia: "));
    Serial.print(d, 1);
    Serial.println(F(" cm"));
    delay(200);
  } 
}
