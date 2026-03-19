#include <Servo.h>

// Pini motoare
const int motorL_dir = 2; 
const int motorL_pwm = 5;
const int motorR_dir = 4;
const int motorR_pwm = 6;

// Pini Senzori
const int trigPin = 12;
const int echoPin = 13;
const int irLeft = A1;
const int irRight = A2;

Servo myservo;
unsigned long lastTelemetryTime = 0;
const int telemetryInterval = 200; // Trimitem date de 5 ori pe secundă

void setup() {
  Serial.begin(9600); // Comunicare cu ESP32
  
  pinMode(motorL_dir, OUTPUT);
  pinMode(motorL_pwm, OUTPUT);
  pinMode(motorR_dir, OUTPUT);
  pinMode(motorR_pwm, OUTPUT);
  
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(irLeft, INPUT);
  pinMode(irRight, INPUT);

  myservo.attach(10);
  myservo.write(90); // Capul drept înainte
}

float getDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  float duration = pulseIn(echoPin, HIGH, 30000); // Timeout după 30ms (aprox 5 metri)
  if (duration == 0) return 400.0; // Dacă nu vede nimic, raportăm distanță maximă
  return duration / 58.0;
}

void sendTelemetry() {
  float dist = getDistance();
  int irL = digitalRead(irLeft);
  int irR = digitalRead(irRight);

  // Format compact pentru ESP32
  Serial.print("T|");
  Serial.print(dist);
  Serial.print("|");
  Serial.print(irL);
  Serial.print("|");
  Serial.println(irR);
}

void loop() {
  // 1. Gestionare Telemetrie
  if (millis() - lastTelemetryTime >= telemetryInterval) {
    sendTelemetry();
    lastTelemetryTime = millis();
  }

  // 2. Gestionare Comenzi de la ESP32
  if (Serial.available() > 0) {
    char cmd = Serial.read();
    handleCommand(cmd);
  }
}

void handleCommand(char c) {
  int speed = 100; // Viteza redusă (era 150). Poți pune și 80 dacă e încă prea rapidă.
  
  switch (c) {
    case 'F': // Forward
      digitalWrite(motorL_dir, HIGH); analogWrite(motorL_pwm, speed);
      digitalWrite(motorR_dir, LOW);  analogWrite(motorR_pwm, speed);
      break;
    case 'B': // Backward
      digitalWrite(motorL_dir, LOW);  analogWrite(motorL_pwm, speed);
      digitalWrite(motorR_dir, HIGH); analogWrite(motorR_pwm, speed);
      break;
    case 'L': // Left
      digitalWrite(motorL_dir, LOW);  analogWrite(motorL_pwm, speed);
      digitalWrite(motorR_dir, LOW);  analogWrite(motorR_pwm, speed);
      break;
    case 'R': // Right
      digitalWrite(motorL_dir, HIGH); analogWrite(motorL_pwm, speed);
      digitalWrite(motorR_dir, HIGH); analogWrite(motorR_pwm, speed);
      break;
    case 'S': // Stop
      analogWrite(motorL_pwm, 0);
      analogWrite(motorR_pwm, 0);
      break;
  }
}