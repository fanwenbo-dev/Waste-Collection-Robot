//Library Inclusions
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);

//Pin Definitions
#define Hit_aPin 4
#define Hit_bPin 2
#define TrigPin1 11   
#define EchoPin1 10   
#define IR1Pin  A0  
#define motor_board_input_pin_IN2 6      
#define motor_board_input_pin_IN1 9       
#define motor_board_input_pin_IN4 3 
#define motor_board_input_pin_IN3 5

//Variable Declarations
int red = 7;
int flag = 1;
int green = 8;
int counter = 0;
int IR1_Val = 0;
bool isForward = true;
bool robotPaused = false;
int LEDBlinkDelay = 100;
int batteryPercentage = 100;
unsigned long lastUpdateTime = 0;
const unsigned long updateInterval = 10000; 



void setup() {
  //Setup Function
  Serial.begin(9600);  
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  Serial.print("Digital Input value from IR1.\n");
  pinMode(TrigPin1, OUTPUT);
  pinMode(EchoPin1, INPUT);
  lcd.init();
  lcd.setCursor(0,0);
  lcd.begin(16, 2); 
  lcd.backlight();
  pinMode(motor_board_input_pin_IN2, OUTPUT); 
  pinMode(motor_board_input_pin_IN1, OUTPUT); 
  pinMode(motor_board_input_pin_IN4, OUTPUT); 
  pinMode(motor_board_input_pin_IN3, OUTPUT);   
}

void loop(){
  //runs repeatedly
  handleTargetAHit();
  handleTargetBHit();
  int Distance = ultrasonicDistance(); 
  Serial.print("Distance = ");
  Serial.print(Distance);
  Serial.println(" cm");
  //Lcd first row
  lcd.clear(); 
  lcd.setCursor(0, 0); 
  lcd.print("battery: ");
  lcd.print(batteryPercentage);
  lcd.print("%");
  //Lcd second row
  lcd.setCursor(0, 1); 
  lcd.print("trash picked: ");
  lcd.print(counter);

  //update battery status every 10 seconds
  if (millis() - lastUpdateTime >= updateInterval) {
     batteryPercentage--;
     if(batteryPercentage < 0){
       batteryPercentage = 0;
     }
     lastUpdateTime = millis();
  }
     delay(100);

    if (batteryPercentage < 20) {
    blinkRed();
  }

  int object = digitalRead(IR1Pin); 
  Serial.print(object);         
  Serial.print("\n");
  Serial.print(IR1_Val);
  Serial.print("Input Value:");

 
  if (!robotPaused) {
    // Only move the robot if it's not paused
    if (object < 1) {
      stopmotor();
      if(batteryPercentage > 19 ){
      blinkGreen();
      }
      if (flag == 1) {
        counter++;
        flag = 0;
      }
    
    } else {
      flag = 1;
      if (Distance < 30) {
        turnRight();
        LEDColor(255, 0); // LED turn Red
        delay(500);
      } 
      else {
        if (isForward) {
        forward();
        } else {
        forwardFaster();
     }   
        if(batteryPercentage > 19 ){
        LEDColor(0, 255); // LED turn Green
        delay(100);
        }
      }
    }
  }
}

int ultrasonicDistance(){
  long pulseDuration; 
  int Distance; 
  digitalWrite(TrigPin1, LOW);
  delayMicroseconds(2);
  digitalWrite(TrigPin1, HIGH);
  delayMicroseconds(5);
  digitalWrite(TrigPin1, LOW);
  pulseDuration = pulseIn(EchoPin1, HIGH);
  Distance = pulseDuration / 58;
  return Distance;
}
void turnRight(){
  digitalWrite(motor_board_input_pin_IN2, HIGH);
  analogWrite(motor_board_input_pin_IN1, 150);
  digitalWrite(motor_board_input_pin_IN4, HIGH);
  analogWrite(motor_board_input_pin_IN3, 150);
}
void forward(){
  digitalWrite(motor_board_input_pin_IN2, HIGH);
  analogWrite(motor_board_input_pin_IN1, 140);
  digitalWrite(motor_board_input_pin_IN4, LOW);
  analogWrite(motor_board_input_pin_IN3, 130);
}
void forwardFaster(){
  digitalWrite(motor_board_input_pin_IN2, HIGH);
  analogWrite(motor_board_input_pin_IN1, 100);
  digitalWrite(motor_board_input_pin_IN4, LOW);
  analogWrite(motor_board_input_pin_IN3, 170);
}
void stopmotor(){
  digitalWrite(motor_board_input_pin_IN2, HIGH);
  analogWrite(motor_board_input_pin_IN1, 255);
  digitalWrite(motor_board_input_pin_IN4, LOW);
  analogWrite(motor_board_input_pin_IN3, 0);
}
void LEDColor(int R, int G){
  analogWrite(red, R);
  analogWrite(green, G); 
}
void blinkGreen() {
    LEDColor(0, 255); // Green
    delay(LEDBlinkDelay);
    LEDColor(255, 255); // 
    delay(LEDBlinkDelay);
}
void blinkRed() {
    LEDColor(255, 0); // Red
    delay(LEDBlinkDelay);
    LEDColor(255, 255); 
    delay(LEDBlinkDelay);
}
void handleTargetAHit() {
  if (!digitalRead(Hit_aPin)) {
    if (robotPaused) {
      // Resume the robot
      robotPaused = false;
    } else {
      // Pause the robot
      robotPaused = true;
      stopmotor();
    }
    delay(500);
  }
}

void handleTargetBHit() {
  if (!digitalRead(Hit_bPin)) {
     //Toggle between regular forward and forwardFaster
     isForward = !isForward;
     if (isForward) {
      forward();
     } else {
     forwardFaster();
     }
     delay(100);  
  }
}
