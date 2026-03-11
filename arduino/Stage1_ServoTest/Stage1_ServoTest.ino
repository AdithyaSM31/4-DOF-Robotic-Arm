#include <Servo.h>

// ── Servo Objects ──────────────────────────────────────────
Servo baseServo;      // Joint 1 - Base rotation
Servo shoulderServo;  // Joint 2 - Shoulder
Servo elbowServo;     // Joint 3 - Elbow
Servo gripperServo;   // Joint 4 - Gripper

// ── Pin Definitions ────────────────────────────────────────
#define BASE_PIN      3
#define SHOULDER_PIN  4   // Avoid pins 5 & 6 (timer conflict)
#define ELBOW_PIN     7
#define GRIPPER_PIN   9

// ── Speed Settings ─────────────────────────────────────────
#define JOINT_SPEED    10   // ms per degree - base/shoulder/elbow
#define GRIPPER_SPEED  20   // Slower = less current spike = no base twitch

// ── Setup ──────────────────────────────────────────────────
void setup() {
  Serial.begin(9600);

  baseServo.attach(BASE_PIN);
  shoulderServo.attach(SHOULDER_PIN);
  elbowServo.attach(ELBOW_PIN);
  gripperServo.attach(GRIPPER_PIN);

  Serial.println("Robotic Arm - Stage 1 Test");
  Serial.println("Moving to home...");
  moveToHome();
  delay(2000);
  Serial.println("Starting joint test sequence...");
}

// ── Loop ───────────────────────────────────────────────────
void loop() {
  testAllJoints();
}

// ── Smooth Move ────────────────────────────────────────────
void smoothMove(Servo &servo, int from, int to, int speed) {
  if (from < to) {
    for (int pos = from; pos <= to; pos++) {
      servo.write(pos);
      delay(speed);
    }
  } else {
    for (int pos = from; pos >= to; pos--) {
      servo.write(pos);
      delay(speed);
    }
  }
}

// ── Home Position ──────────────────────────────────────────
void moveToHome() {
  smoothMove(gripperServo,  gripperServo.read(),   0, GRIPPER_SPEED); // Open gripper first
  delay(150);
  smoothMove(elbowServo,    elbowServo.read(),    90, JOINT_SPEED);
  delay(150);
  smoothMove(shoulderServo, shoulderServo.read(), 90, JOINT_SPEED);
  delay(150);
  smoothMove(baseServo,     baseServo.read(),     90, JOINT_SPEED);
  delay(150);
  Serial.println("Home reached.");
}

// ── Test All Joints ────────────────────────────────────────
void testAllJoints() {
  // Base
  Serial.println("Testing Base (Joint 1)...");
  smoothMove(baseServo, 90, 0,   JOINT_SPEED);
  smoothMove(baseServo, 0,  180, JOINT_SPEED);
  smoothMove(baseServo, 180, 90, JOINT_SPEED);
  delay(500);

  // Shoulder
  Serial.println("Testing Shoulder (Joint 2)...");
  smoothMove(shoulderServo, 90, 60,  JOINT_SPEED);
  smoothMove(shoulderServo, 60, 120, JOINT_SPEED);
  smoothMove(shoulderServo, 120, 90, JOINT_SPEED);
  delay(500);

  // Elbow
  Serial.println("Testing Elbow (Joint 3)...");
  smoothMove(elbowServo, 90, 60,  JOINT_SPEED);
  smoothMove(elbowServo, 60, 120, JOINT_SPEED);
  smoothMove(elbowServo, 120, 90, JOINT_SPEED);
  delay(500);

  // Gripper - full 0 to 180
  Serial.println("Testing Gripper (Joint 4)...");
  smoothMove(gripperServo, 0,   180, GRIPPER_SPEED); // Fully close
  delay(400);
  smoothMove(gripperServo, 180, 0,   GRIPPER_SPEED); // Fully open
  delay(400);

  Serial.println("Test complete! Waiting 3 seconds...");
  delay(3000);
}
