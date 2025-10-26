#include <Arduino.h>

// function declarations
void moveTo(float x_target, float y_target);

// left and right stepper motors 
const int L_STEP = 5; 
const int L_DIR = 4;
const int R_STEP = 3; 
const int R_DIR = 2; 

//steps per mm
const float steps_per_mm = 80;

// tracking position 
float x_pos = 0; 
float y_pos = 0; 

void setup() {
  // define pins 
  pinMode(L_STEP, OUTPUT);
  pinMode(L_DIR, OUTPUT);
  pinMode(R_STEP, OUTPUT);
  pinMode(R_DIR, OUTPUT);

  digitalWrite(L_STEP, LOW);
  digitalWrite(R_STEP, LOW);
  
  // open serial 
  Serial.begin(9600);
  Serial.println("Running"); 
  
  /*
  for (int i = 0; i < 200; i++){
    digitalWrite(L_STEP, HIGH);
    delayMicroseconds(2000);
    digitalWrite(L_STEP, LOW);
    delayMicroseconds(4000);
    Serial.print(i);

  }
  */
  
}

void loop() {
  
  if (Serial.available() > 0) {
    float x_target = Serial.parseFloat();
    float y_target = Serial.parseFloat();
    Serial.print("moving to: ");
    Serial.print(x_target);
    Serial.print(" and ");
    Serial.println(y_target); 
    moveTo(x_target, y_target);
     
  }       
}

// put function definitions here:
void moveTo(float x_target, float y_target) {
  float dx = x_target - x_pos;
  float dy = y_target - y_pos;

  long l_steps = (dx + dy) * steps_per_mm;
  long r_steps = (dx - dy) * steps_per_mm;

  int max_steps = (abs(l_steps) > abs(r_steps)) ? abs(l_steps) : abs(r_steps);

  for (long i = 0; i < max_steps; i++) {
    if (i < abs(l_steps)) {
      digitalWrite(L_DIR, l_steps > 0 ? HIGH : LOW);
      digitalWrite(L_STEP, HIGH);
      delayMicroseconds(2000);
      digitalWrite(L_STEP, LOW);
    }

    if (i < abs(r_steps)) {
      digitalWrite(R_DIR, r_steps > 0 ? HIGH : LOW);
      digitalWrite(R_STEP, HIGH);
      delayMicroseconds(2000);
      digitalWrite(R_STEP, LOW);
    }

    delayMicroseconds(4000);  // Adjust for speed

    digitalWrite(L_STEP, LOW);
    digitalWrite(R_STEP, LOW);
  
  }

  x_pos = x_target;
  y_pos = y_target;
  Serial.print("Moved to: ");
  Serial.print(x_pos);
  Serial.print(", ");
  Serial.println(y_pos);
}
