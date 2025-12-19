#include <PID_v1.h>
#include <PID_AutoTune_v0.h>
#include <EEPROM.h> // Librería para guardar en memoria

// ==========================================
// === CONFIGURACIÓN DE USUARIO ===
// ==========================================

// ¡CAMBIA ESTO!
// true  = El robot hará el AutoTune, guardará valores y luego funcionará.
// false = El robot leerá la memoria y funcionará directo.
const bool FORZAR_CALIBRACION = true; 

double setpoint = 10.0; // OBJETIVO: 10 cm
int velocidadBase = 120; // Velocidad de crucero (0-255)

// ==========================================
// === HARDWARE ===
// ==========================================
#define DIR_A 2 
#define PWM_A 5
#define DIR_B 4 
#define PWM_B 6
const bool MOTOR_B_INVERTIDO = true;

#define TRIG_IZQ 7
#define ECHO_IZQ 11
#define TRIG_DER 9
#define ECHO_DER 8
#define TRIG_CENTRO 12
#define ECHO_CENTRO 13
#define VEL_SONIDO 0.035103f

// ==========================================
// === VARIABLES PID Y MEMORIA ===
// ==========================================

double input, output;
double kp = 2, ki = 0.5, kd = 2; // Valores por defecto por si acaso

PID myPID(&input, &output, &setpoint, kp, ki, kd, DIRECT);
PID_ATune aTune(&input, &output);

bool tuning = false; // Estado actual

// Estructura para guardar en memoria
struct PID_Config {
  double p;
  double i;
  double d;
  bool valido; // Para saber si hay datos guardados reales
};

// Dirección de memoria donde guardaremos (posición 0)
int eeAddress = 0;

// ==========================================
// === FUNCIONES AUXILIARES ===
// ==========================================

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

void detenerMotores() {
  avanza(0, 0, 0, 0);
}

float distanciaCM(int trig, int echo) {
  digitalWrite(trig, LOW); delayMicroseconds(2);
  digitalWrite(trig, HIGH); delayMicroseconds(10);
  digitalWrite(trig, LOW);
  unsigned long dur = pulseIn(echo, HIGH, 20000); 
  if (dur == 0) return NAN;
  return (dur * VEL_SONIDO) * 0.5f;
}

// ==========================================
// === SETUP ===
// ==========================================

void setup() {
  // Configuración de Pines
  pinMode(DIR_A, OUTPUT); pinMode(PWM_A, OUTPUT);
  pinMode(DIR_B, OUTPUT); pinMode(PWM_B, OUTPUT);
  pinMode(TRIG_CENTRO, OUTPUT); pinMode(ECHO_CENTRO, INPUT);
  pinMode(TRIG_IZQ, OUTPUT); pinMode(ECHO_IZQ, INPUT);
  pinMode(TRIG_DER, OUTPUT); pinMode(ECHO_DER, INPUT);
  
  // LED integrado para avisos visuales
  pinMode(LED_BUILTIN, OUTPUT); 

  // Inicializar Sensores
  digitalWrite(TRIG_CENTRO, LOW); digitalWrite(TRIG_IZQ, LOW); digitalWrite(TRIG_DER, LOW);

  // --- LÓGICA DE MEMORIA ---
  PID_Config datosGuardados;
  EEPROM.get(eeAddress, datosGuardados);

  if (FORZAR_CALIBRACION == false && datosGuardados.valido == true) {
    // CASO 1: Usar datos guardados
    kp = datosGuardados.p;
    ki = datosGuardados.i;
    kd = datosGuardados.d;
    tuning = false;
    
    // Parpadeo lento (3 veces) para indicar "DATOS CARGADOS OK"
    for(int i=0; i<3; i++) { digitalWrite(LED_BUILTIN, HIGH); delay(500); digitalWrite(LED_BUILTIN, LOW); delay(500); }
    
  } else {
    // CASO 2: Calibrar (o no había datos)
    tuning = true;
    
    // Configurar AutoTune
    input = distanciaCM(TRIG_DER, ECHO_DER);
    if(isnan(input)) input = setpoint; 
    
    aTune.SetOutputStep(40);
    aTune.SetNoiseBand(2);
    aTune.SetLookbackSec(0.05);
    
    // Parpadeo rápido (10 veces) para indicar "MODO APRENDIZAJE"
    for(int i=0; i<10; i++) { digitalWrite(LED_BUILTIN, HIGH); delay(100); digitalWrite(LED_BUILTIN, LOW); delay(100); }
    
    // Espera 3 seg para ponerlo en el suelo
    delay(3000);
  }

  // Configurar PID final (ya sea con datos cargados o por defecto)
  myPID.SetMode(AUTOMATIC);
  myPID.SetOutputLimits(-255, 255);
  myPID.SetTunings(kp, ki, kd);
}

// ==========================================
// === LOOP ===
// ==========================================

void loop() {
  // 1. LEER SENSOR (Con filtro)
  float lectura = distanciaCM(TRIG_DER, ECHO_DER);
  if (!isnan(lectura) && lectura > 0 && lectura < 400) {
    input = lectura;
  }

  // 2. LÓGICA DE CONTROL
  if (tuning) {
    // --- MODO TUNING ---
    byte val = (aTune.Runtime());
    
    if (val != 0) {
      // ¡CALIBRACIÓN TERMINADA!
      tuning = false;
      
      // Obtener nuevos valores
      kp = aTune.GetKp();
      ki = aTune.GetKi();
      kd = aTune.GetKd();
      myPID.SetTunings(kp, ki, kd);
      
      // GUARDAR EN EEPROM
      PID_Config nuevosDatos = {kp, ki, kd, true};
      EEPROM.put(eeAddress, nuevosDatos);
      
      // SEÑAL VISUAL: DETENERSE 3 SEGUNDOS
      detenerMotores();
      digitalWrite(LED_BUILTIN, HIGH); // Luz fija indicando "Guardado"
      delay(3000); 
      digitalWrite(LED_BUILTIN, LOW);
      
      // El robot continuará automáticamente en modo normal
    }
  } else {
    // --- MODO NORMAL PID ---
    myPID.Compute();
  }

  // 3. MOTORES
  // Si estamos parados por la pausa de guardado, el loop se detuvo arriba con delay.
  // Si llegamos aquí, aplicamos movimiento.
  
  if (!tuning || (tuning && output != 0)) { // Seguridad
     int velIzq = velocidadBase - output;
     int velDer = velocidadBase + output;
     avanza(velIzq, velDer, 1, 1);
  }
}