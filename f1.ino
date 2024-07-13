#include <Wire.h>
#include <AFMotor.h>
#include <NewPing.h>
#include <Servo.h>

// Connect the left and right motors to the shield
AF_DCMotor motor1(1, MOTOR12_1KHZ);
AF_DCMotor motor2(2, MOTOR12_1KHZ);
AF_DCMotor motor3(3, MOTOR34_1KHZ);
AF_DCMotor motor4(4, MOTOR34_1KHZ);

// hc-sr04 sensor
#define TRIGGER_PIN A3
#define ECHO_PIN A4
#define MAX_DISTANCE 50

// IR sensors
const int irLeft = A0;
const int irCenter = A1;
const int irRight = A5;

//motor
#define MAX_SPEED 200
#define MAX_SPEED_OFFSET 20

Servo servo;  

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

int distance = 0;
int leftDistance;
int centerDistance; 
int rightDistance;


boolean object;

void setup() {
  Serial.begin(9600);
  pinMode(irLeft, INPUT);
  pinMode(irCenter, INPUT);  
  pinMode(irRight, INPUT);
  
  servo.attach(A2);  
  servo.write(90);

  motor1.setSpeed(60);
  motor2.setSpeed(60);
  motor3.setSpeed(60);
  motor4.setSpeed(60);
}


void loop() {

  int irLeftVal = analogRead(irLeft);
  int irCenterVal =  analogRead(irCenter);
  int irRightVal = analogRead(irRight);
  // 1 stop
  if (digitalRead(irLeftVal) == LOW && digitalRead(irCenterVal) == LOW && digitalRead(irRightVal) == LOW) {
    Serial.println("Stop");
    stop();
    while (digitalRead(irLeftVal) == LOW && digitalRead(irCenterVal) == LOW && digitalRead(irRightVal) == LOW) {
      moveBackward();

      irLeftVal = analogRead(irLeft);
      irCenterVal =  analogRead(irCenter);
      irRightVal = analogRead(irRight);
    }
    stop();
    delay(250);
    irLeftVal = analogRead(irLeft);
    irCenterVal =  analogRead(irCenter);
    irRightVal = analogRead(irRight);
    if(digitalRead(irLeftVal) == LOW && digitalRead(irCenterVal) == LOW && digitalRead(irRightVal) == LOW){
      while (digitalRead(irLeftVal) == LOW && digitalRead(irCenterVal) == LOW && digitalRead(irRightVal) == LOW){
        moveForward();
        irLeftVal = analogRead(irLeft);
        irCenterVal =  analogRead(irCenter);
        irRightVal = analogRead(irRight);
      }
    }
    stop();
    delay(150);
  }
  // 2 Right Turn
  else if (digitalRead(irLeft) == LOW && digitalRead(irCenter) == LOW && digitalRead(irRight) == HIGH) {
    objectAvoid();
    turnRight();
    Serial.println("TR");
  }
  // 3 Forward
  else if (digitalRead(irLeft) == LOW && digitalRead(irCenter) == HIGH && digitalRead(irRight) == LOW) {
    objectAvoid();
    Serial.println("Forward");
    moveForward();  
  }
  // 4 Right Turn in 90 degrees
  else if (digitalRead(irLeft) == LOW && digitalRead(irCenter) == HIGH && digitalRead(irRight) == HIGH) {
    objectAvoid();
    sharpTurnRight();
    Serial.println("TR in 90 degrees");
  }
  // 5 Left turn
  else if (digitalRead(irLeft) == HIGH && digitalRead(irCenter) == LOW && digitalRead(irRight) == LOW) {
    objectAvoid();
    Serial.println("TL");
    turnLeft();
  }
  // 6 Left Turn in 90 degrees
  else if (digitalRead(irLeft) == HIGH && digitalRead(irCenter) == HIGH && digitalRead(irRight) == LOW) {
    objectAvoid();
    sharpTurnLeft();
    Serial.println("TL in 90 degrees");
  }
  // 7 Turn 180 degrees
  else if (digitalRead(irLeft) == HIGH && digitalRead(irCenter) == HIGH && digitalRead(irRight) == HIGH) {
    objectAvoid();
    Serial.println("Turn 180 degrees");
    stop();
   
  }
}
//ultrasonic
void objectAvoid() {
  distance = getDistance();
  if (distance <= 15) {
    // stop
    stop();
    Serial.println("Stop");

    lookLeft();
    lookCenter();  
    lookRight();
    delay(100);

    if (rightDistance <= leftDistance && rightDistance <= centerDistance) {
      // left
      object = true;
      
      turnRight();
      Serial.println("moveRight");
    } else if (centerDistance <= leftDistance && centerDistance <= rightDistance) {
      // center
      object = true;
      Serial.println("moveForward");
    } else {
      // right
      object = false;
      
     turnRight();
      Serial.println("moveRight");
    }

    delay(100);
  }
  else {
    // forward
    Serial.println("moveForward");
    moveForward();
  }
}

int getDistance() {
  delay(50);
  int cm = sonar.ping_cm();
  if (cm == 0) {
    cm = 100;
  }
  return cm;
}

int lookLeft() {
  // look left
  servo.write(150);
  delay(500);
  leftDistance = getDistance();
  delay(100);
  servo.write(90);
  Serial.print("Left:");
  Serial.print(leftDistance);
  return leftDistance;
}

int lookCenter() {
  // look center
  servo.write(90);
  delay(500);
  centerDistance = getDistance();
  delay(100);
  servo.write(90);
  Serial.print("   ");
  Serial.print("Center:");
  Serial.println(centerDistance);
  return centerDistance;
}

int lookRight() {
  // look right
  servo.write(30);
  delay(500);
  rightDistance = getDistance();
  delay(100);
  servo.write(90);
  Serial.print("   ");
  Serial.print("Right:");
  Serial.println(rightDistance);
  return rightDistance;
}



void stop() {
  motor1.run(RELEASE);
  motor2.run(RELEASE);
  motor3.run(RELEASE);
  motor4.run(RELEASE);
}



void moveForward() {
  motor1.run(FORWARD);
  motor2.run(FORWARD);
  motor3.run(FORWARD);
  motor4.run(FORWARD);
  delay(300);
}

void moveBackward() {
  motor1.run(BACKWARD);
  motor2.run(BACKWARD);
  motor3.run(BACKWARD);
  motor4.run(BACKWARD);
  delay(300);
}

void turnLeft() {
  motor1.setSpeed(50);
  motor2.setSpeed(50);
  motor3.setSpeed(80);
  motor4.setSpeed(80);
  int irLeftVal = analogRead(irLeft);
  while (irLeftVal == HIGH) {
    motor1.run(FORWARD);
    motor2.run(FORWARD);
    motor3.run(FORWARD);
    motor4.run(FORWARD);
    irLeftVal = analogRead(irLeft);
  }
}

void turnRight() {
  motor1.setSpeed(70);
  motor2.setSpeed(70);
  motor3.setSpeed(50);
  motor4.setSpeed(50);
  int irRightVal = analogRead(irRight);
  while (irRightVal == HIGH) {
    motor1.run(FORWARD);
    motor2.run(FORWARD);
    motor3.run(FORWARD);
    motor4.run(FORWARD);
    irRightVal = analogRead(irRight);
  }
}

void sharpTurnRight() {
  stop();
  delay(300);

  int irCenterVal = analogRead(irCenter);

  if (irCenterVal == LOW) {
    while (irCenterVal == LOW) {
      moveBackward();
      irCenterVal = analogRead(irCenter);
    }
  }
  stop();
  delay(200);
  motor1.setSpeed(90);
  motor2.setSpeed(0);
  motor3.setSpeed(0);
  motor4.setSpeed(90);
  int irRightVal = analogRead(irRight);
  while (true) {
    motor1.run(FORWARD);
    motor4.run(FORWARD);
    irCenterVal = analogRead(irCenter);
    irRightVal = analogRead(irRight);
    if (irRightVal == LOW && irCenterVal > HIGH) {
      break;
    }
  }
  stop();
}

void sharpTurnLeft() {
  stop();
  delay(300);

  int irCenterVal = analogRead(irCenter);

  if (irCenterVal == LOW) {
    while (irCenterVal == LOW) {
      moveBackward();
      irCenterVal = analogRead(irCenter);
    }
  }
  stop();
  delay(200);
  motor1.setSpeed(0);
  motor2.setSpeed(90);
  motor3.setSpeed(90);
  motor4.setSpeed(0);

  int irLeftVal = analogRead(irLeft);

  while (true) {
    motor2.run(FORWARD);
    motor3.run(FORWARD);
    irCenterVal = analogRead(irCenter);
    irLeftVal = analogRead(irLeft);

    if (irLeftVal == LOW && irCenterVal == HIGH) {
      break;
    }
  }
  stop();
  
}


