#include <Servo.h>

// Servo motors
Servo servo1;
Servo servo2;

// Motor driver pins
int motor1Enable = 3;
int motor1Input1 = 5;
int motor1Input2 = 6;

int motor2Enable = 4;
int motor2Input1 = 7;
int motor2Input2 = 8;

// IR sensor
int irSensorPin = 2;

void setup() {
  servo1.attach(9); // Servo 1 pin
  servo2.attach(10); // Servo 2 pin

  pinMode(motor1Enable, OUTPUT);
  pinMode(motor1Input1, OUTPUT);
  pinMode(motor1Input2, OUTPUT);

  pinMode(motor2Enable, OUTPUT);
  pinMode(motor2Input1, OUTPUT);
  pinMode(motor2Input2, OUTPUT);
  
  pinMode(irSensorPin, INPUT);

  servo1.write(0); // Initial position
  servo2.write(0);
}

void loop() {
  if (digitalRead(irSensorPin) == LOW) { // Assuming LOW means paper detected
    delay(5000); // <-- Add 5 second delay after detecting paper

    // Activate servo motors to push paper
    servo1.write(90);
    servo2.write(90);
    delay(500); // Adjust based on your mechanism

    // Activate DC motors to spin rollers
    spinMotors();
    delay(2000); // Adjust this duration as needed

    // Stop motors and reset servos
    stopMotors();
    servo1.write(0);
    servo2.write(0);
  }
}

void spinMotors() {
  analogWrite(motor1Enable, 255);
  digitalWrite(motor1Input1, HIGH);
  digitalWrite(motor1Input2, LOW);

  analogWrite(motor2Enable, 255);
  digitalWrite(motor2Input1, HIGH);
  digitalWrite(motor2Input2, LOW);
}

void stopMotors() {
  analogWrite(motor1Enable, 0);
  digitalWrite(motor1Input1, LOW);
  digitalWrite(motor1Input2, LOW);

  analogWrite(motor2Enable, 0);
  digitalWrite(motor2Input1, LOW);
  digitalWrite(motor2Input2, LOW);
}
