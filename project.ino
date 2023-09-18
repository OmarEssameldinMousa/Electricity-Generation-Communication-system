#include <AccelStepper.h>
#include <LiquidCrystal.h>

// Water flow sensor pins & variables
const int water = 2;
volatile int Pulse_Count;
double flow_rate;
unsigned long Current_Time, Loop_Time;

// LCD pins
LiquidCrystal lcd(11, 8, 4, 5, 6, 7);

// Ultrasonic pins & variables
const int trigPin = 9;
const int echoPin = 10;
long duration;
int distanceCm;

// Stepper motor pins & variables
int startStep = 1;
#define motorPin1 22
#define motorPin2 24
#define motorPin3 26
#define motorPin4 28

AccelStepper stepper1(AccelStepper::FULL4WIRE, motorPin1, motorPin3, motorPin2, motorPin4, true);

void setup() {
  // Water flow sensor
  pinMode(water, INPUT);
  Serial.begin(9600);
  attachInterrupt(0, Detect_Rising_Edge, RISING);
  Current_Time = millis();
  Loop_Time = Current_Time;

  // LCD setup
  Serial.begin(9600);
  lcd.begin(16, 2);

  // Ultrasonic sensor
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Stepper motor adjustments
  stepper1.setMaxSpeed(1000);
  stepper1.setSpeed(400);
  stepper1.setAcceleration(400);
  stepper1.setCurrentPosition(0);
}

void loop() {
  // Stepper motor control
  if (distanceCm > 15 && distanceCm < 25) {
    stepperAdjustments(0);
  } else if (distanceCm <= 15) {
    stepperAdjustments(250);
  } else if (distanceCm >= 25) {
    stepperAdjustments(-250);
  }

  // Water flow sensor
  if (Current_Time >= (Loop_Time + 1000)) {
    Loop_Time = Current_Time;
    flow_rate = Pulse_Count * 2.5;
    flow_rate = flow_rate / 1000;
    Pulse_Count = 0;
  }

  // Ultrasonic sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distanceCm = duration * 0.034 / 2;

  // LCD display
  lcd.setCursor(0, 0);
  lcd.print("Distance: ");
  lcd.print(distanceCm);
  lcd.print(" cm");
  delay(10);
  lcd.setCursor(0, 1);
  lcd.print("Flow: ");
  lcd.print(flow_rate);
  lcd.print(" L/S");
  delay(10);

  // Bluetooth module
  int water_level = 56 - distanceCm;
  if (Serial.available()) {
    Serial.print("Water level with respect to the sensor: ");
    Serial.println(water_level);
    delay(1000);
    Serial.print("Water flow rate: ");
    Serial.println(flow_rate);
  }
}

// Interrupt handler for water flow sensor
void Detect_Rising_Edge() {
  Pulse_Count++;
}

// Stepper motor control function
void stepperAdjustments(int steps) {
  while (startStep != 0) {
    stepper1.run();
    stepper1.moveTo(steps);
    startStep = stepper1.distanceToGo();
    delay(1);
  }
  startStep = 1;
}
