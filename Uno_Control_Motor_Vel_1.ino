// Dr. Sergio Miguel Delfín Prieto
// FI-UAQ
// Para la materia de Ingenieria de Control
// Ingeniería Industrial y de Manufactura
// ESTE Script REALIZA: LA PRUEBA DE VELOCIDAD ANGULAR PARA IDENTIFICAR
//                      LOS PARÁMETROS DE UN MOTOR DE CD. 

#include <TimerOne.h>
int chA = 2; // Pin de interrupción externa
int chB = 4;

int in3 = 5;  //
int in4 = 9;  // puente h
int pwm6 = 6; //

const int swSerial = 8; // Switch para enviar datos por el serial
int swState = 0;
int flagTimer = 1;

volatile int posicionAnterior = 0;
volatile int posicion = 0;
float velocidad = 0.0;
float ts=0.005; // Tiempo de muestreo previamente definido en la config. del Timer1

int i=0;
int control=0;
int controlPWM=0;

void setup() {
  Serial.begin(115200);
  // Switch envio de datos por el serial
  pinMode(swSerial,INPUT);
  // Encoder
  pinMode(chA,INPUT);
  pinMode(chB,INPUT);
  // Puente H
  pinMode(in3,OUTPUT);
  pinMode(in4,OUTPUT);
  pinMode(pwm6,OUTPUT);
  // Servicio de interrupción
  attachInterrupt(digitalPinToInterrupt(chA),encoderISR,RISING);
  //-- Inicializar temporizador: 1000=1ms
  //Timer1.initialize(10000); // 10,000 = 10 ms = 0.010 s
  //-- Inicializar temporizador: 5000=5ms
  Timer1.initialize(5000); // 5,000 = 5 ms = 0.005 s
  Timer1.attachInterrupt(Muestreo);
}

void Muestreo(){
  velocidad = (posicion - posicionAnterior)/ts;
  flagTimer = 1;
  i++;
}

void loop() {
  if(i>=0 && i<=600)
    control = 0; 
  else if(i>600 && i<=1200)
    control = 64;
  else if(i>1200 && i<=1800)
    control = 128;
  else
    control = 0;

  // --- Generar el PWM
  controlPWM = abs(control);

  // --- Generar el cambio de giro del motor
  if(control >= 0){
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
    analogWrite(pwm6,controlPWM);
  }
  else{
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
    analogWrite(pwm6,controlPWM);
  }
  // --- 
  // Enviar datos solo cuando esté el swSerial1==1 && flagTimer==1
  swState = digitalRead(swSerial);
  //if(swState == HIGH && flagTimer==1){
  if(flagTimer==1){
    posicionAnterior =  posicion;
    Serial.print(i*ts,3);
    Serial.print("\t");
    Serial.print(velocidad,3);
    Serial.print("\t");
    Serial.println(control);
    flagTimer = 0;
  }
  // Limpia el búfer serial (de salida)
  Serial.flush();
}

void encoderISR(){
  if(digitalRead(chB) == HIGH){
    posicion ++;
  }
  else{
    posicion --;
  }
}
