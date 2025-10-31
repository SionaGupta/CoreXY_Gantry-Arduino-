#include <Arduino.h>
#include <Servo.h>

// Function declarations
void moveTo(float x_target, float y_target);
void parseGCode(String command);

// Left and right stepper motors
const int L_STEP = 5;
const int L_DIR = 4;
const int R_STEP = 3;
const int R_DIR = 2;
const int SERVO_P = 6;

// Steps per mm
const float steps_per_mm = 120;
const float INCH_TO_MM = 25.4;

// Tracking position (lower left corner, needs to be changed for homing)
float x_pos = 0;
float y_pos = 0;

Servo myServo;              


void setup() {
  // Define pins
  pinMode(L_STEP, OUTPUT);
  pinMode(L_DIR, OUTPUT);
  pinMode(R_STEP, OUTPUT);
  pinMode(R_DIR, OUTPUT);

  digitalWrite(L_STEP, LOW);
  digitalWrite(R_STEP, LOW);

  myServo.attach(SERVO_P);

  // Open serial
  
  Serial.begin(9600);
  Serial.println("Ready for G-code");
  
  
}

void loop() {

  // Read a full line from serial
  if (Serial.available() > 0) {
    String line = Serial.readStringUntil('\n');
    line.trim(); // Remove any whitespace/newline
    if (line.length() > 0) {
      // expected example line: G00 X3 Y3 -> moves to (3, 3)
      parseGCode(line);
    }
  }
}

// G00 X9 Y-3




// Parse a G-code line
void parseGCode(String command) {
  command.toUpperCase(); // consistency

  if (command.startsWith("G00") || command.startsWith("G01")) { // Normal and Linear move (will make seperate later)
    float x_target = x_pos;
    float y_target = y_pos;

    int xIndex = command.indexOf('X');
    if (xIndex != -1) {
      x_target = command.substring(xIndex + 1).toFloat();
    }

    int yIndex = command.indexOf('Y');
    if (yIndex != -1) {
      y_target = command.substring(yIndex + 1).toFloat();
    }

    Serial.print("Moving to: X");
    Serial.print(x_target);
    Serial.print(" Y");
    Serial.println(y_target);

    moveTo(x_target, y_target);
  }
  else if (command.startsWith("M114")) { // Report current position
    Serial.print("Current position: X");
    Serial.print(x_pos);
    Serial.print(" Y");
    Serial.println(y_pos);
  }
  else if (command.startsWith("M03")) { // Pick Place Drill     
    myServo.write(100);
    delay(1000);

    // Move back to 0°
    myServo.write(30);
    delay(2000);
  }
  else {
      int spaceIndex = command.indexOf(' ');
    if (spaceIndex != -1) {
      String first = command.substring(0, spaceIndex);
      String second = command.substring(spaceIndex + 1);

      if (first.length() > 0 && second.length() > 0) {
        float x_target = first.toFloat();
        float y_target = second.toFloat();

        Serial.print("Moving to: X");
        Serial.print(x_target);
        Serial.print(" Y");
        Serial.println(y_target);

        moveTo(x_target, y_target);
        return;  
      }
    }
  }
}

// Move function 
void moveTo(float x_target, float y_target) {
  float dx = x_target - x_pos;
  float dy = y_target - y_pos;
 

  long l_steps = ((dx + dy) ) * steps_per_mm;
  long r_steps = ((dx - dy) ) * steps_per_mm;

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
  }

  x_pos = x_target;
  y_pos = y_target;

  Serial.print("Moved to: X");
  Serial.print(x_pos);
  Serial.print(" Y");
  Serial.println(y_pos);
}
