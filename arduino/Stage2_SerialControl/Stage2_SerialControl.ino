#include <Servo.h>

// ── Servo Objects ──────────────────────────────────────────
Servo baseServo;
Servo shoulderServo;
Servo elbowServo;
Servo gripperServo;

// ── Pin Definitions ────────────────────────────────────────
#define BASE_PIN      3
#define SHOULDER_PIN  4   // Avoid pins 5 & 6 (timer conflict with Servo lib)
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

  moveToHome();

  Serial.println("================================");
  Serial.println("    Robotic Arm Controller      ");
  Serial.println("================================");
  Serial.println("B<angle>  Base      (0-180)");
  Serial.println("S<angle>  Shoulder  (0-180)");
  Serial.println("E<angle>  Elbow     (0-180)");
  Serial.println("G<angle>  Gripper   (0=open, 180=closed)");
  Serial.println("H         Home Position");
  Serial.println("P         Pick & Place Demo");
  Serial.println("================================");
}

// ── Loop ───────────────────────────────────────────────────
void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    if (command.length() > 0) {
      processCommand(command);
    }
  }
}

// ── Command Processor ──────────────────────────────────────
void processCommand(String cmd) {
  char joint = cmd.charAt(0);
  int  angle = cmd.substring(1).toInt();

  delay(50); // Settle time - prevents signal interference between servos

  switch (joint) {

    case 'B': case 'b':
      angle = constrain(angle, 0, 180);
      smoothMove(baseServo, baseServo.read(), angle, JOINT_SPEED);
      Serial.println("Base -> " + String(angle) + " deg");
      break;

    case 'S': case 's':
      angle = constrain(angle, 0, 180);
      smoothMove(shoulderServo, shoulderServo.read(), angle, JOINT_SPEED);
      Serial.println("Shoulder -> " + String(angle) + " deg");
      break;

    case 'E': case 'e':
      angle = constrain(angle, 0, 180);
      smoothMove(elbowServo, elbowServo.read(), angle, JOINT_SPEED);
      Serial.println("Elbow -> " + String(angle) + " deg");
      break;

    case 'G': case 'g':
      angle = constrain(angle, 0, 180); // Full 0-180 range
      smoothMove(gripperServo, gripperServo.read(), angle, GRIPPER_SPEED);
      Serial.println("Gripper -> " + String(angle) + " deg");
      break;

    case 'H': case 'h':
      moveToHome();
      break;

    case 'P': case 'p':
      pickAndPlaceDemo();
      break;

    default:
      Serial.println("Unknown command: " + cmd);
      Serial.println("Use B/S/E/G + angle, H for home, P for demo");
      break;
  }
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
  Serial.println("Returning Home...");
  smoothMove(gripperServo,  gripperServo.read(),   0, GRIPPER_SPEED); // Open gripper first
  delay(150);
  smoothMove(elbowServo,    elbowServo.read(),    90, JOINT_SPEED);
  delay(150);
  smoothMove(shoulderServo, shoulderServo.read(), 90, JOINT_SPEED);
  delay(150);
  smoothMove(baseServo,     baseServo.read(),     90, JOINT_SPEED);
  delay(150);
  Serial.println("Home Position OK");
}

// ── Pick & Place Demo ──────────────────────────────────────
void pickAndPlaceDemo() {
  Serial.println("--- Pick & Place Demo ---");

  // 1. Open gripper fully before moving
  Serial.println("1. Opening gripper");
  smoothMove(gripperServo, gripperServo.read(), 0, GRIPPER_SPEED);
  delay(300);

  // 2. Move to pick position
  Serial.println("2. Moving to pick position");
  smoothMove(baseServo,     90,  45, JOINT_SPEED);
  delay(150);
  smoothMove(shoulderServo, 90, 120, JOINT_SPEED);
  delay(150);
  smoothMove(elbowServo,    90,  60, JOINT_SPEED);
  delay(500);

  // 3. Lower to object
  Serial.println("3. Lowering to object");
  smoothMove(shoulderServo, 120, 150, JOINT_SPEED);
  delay(500);

  // 4. Close gripper to grip
  Serial.println("4. Gripping object");
  smoothMove(gripperServo, 0, 120, GRIPPER_SPEED); // ~2/3 closed for grip
  delay(400);

  // 5. Lift up
  Serial.println("5. Lifting");
  smoothMove(shoulderServo, 150, 90, JOINT_SPEED);
  delay(400);

  // 6. Rotate to drop zone
  Serial.println("6. Rotating to drop zone");
  smoothMove(baseServo, 45, 135, JOINT_SPEED);
  delay(300);

  // 7. Lower to place
  Serial.println("7. Lowering to place");
  smoothMove(shoulderServo, 90, 130, JOINT_SPEED);
  delay(400);

  // 8. Release object
  Serial.println("8. Releasing object");
  smoothMove(gripperServo, 120, 0, GRIPPER_SPEED);
  delay(400);

  // 9. Return home
  Serial.println("9. Returning home");
  moveToHome();

  Serial.println("--- Demo Complete! ---");
}
