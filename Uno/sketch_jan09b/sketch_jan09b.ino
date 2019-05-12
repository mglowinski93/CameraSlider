#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <CustomStepper.h>
 
const unsigned short int MAIN_DELAY = 1000;
const unsigned short int requiredStep = 2048;
const int ENABLE_EASY_DRIVER = 0;
const int STEP_PIN_EASY_DRIVER = 4;
const int DIR_PIN_EASY_DRIVER = 5;
const int CE = 6;
const int CSN = 7;
int joystickData[3] = {0};
int leftLimitSwitch = 2;
int rightLimitSwitch = 3;
int leftEndSwitchState = 0;
int rightEndSwitchState = 0;
const int speedSetpoint1 = 1;
const int speedSetpoint2 = 2;
unsigned int speedValue = 1;
int rotateStepperSetpointSlow = 12; // RPM can be set from 1RPM to 15RPM
int rotateStepperSetpointFast = 20; // RPM can be set from 1RPM to 15RPM
bool joystickButtonPressed = 0;
CustomStepper stepper(8, 9, 10, 11, (byte[]){8, B1000, B1100, B0100, B0110, B0010, B0011, B0001, B1001}, 4075.7728395, 12, STOP); //more: https://playground.arduino.cc/Main/CustomStepper
 
RF24 radio(CE, CSN); // CE, CSN
const byte address[6] = "00001";
 
void set_speed()
{
  if(speedValue == speedSetpoint1)
  {
    stepper.setRPM(rotateStepperSetpointSlow);
    speedValue = speedSetpoint2;
  }
  else
  {
    stepper.setRPM(rotateStepperSetpointFast);
    speedValue = speedSetpoint1;
  }
}

void moveEasyDriverRight()
{        
  digitalWrite(ENABLE_EASY_DRIVER, LOW);
  digitalWrite(DIR_PIN_EASY_DRIVER, LOW);
  digitalWrite(STEP_PIN_EASY_DRIVER, HIGH);
}

void moveEasyDriverLeft()
{
  digitalWrite(ENABLE_EASY_DRIVER, LOW);
  digitalWrite(DIR_PIN_EASY_DRIVER, HIGH);
  digitalWrite(STEP_PIN_EASY_DRIVER, HIGH);
}

void stopEasyDriver()
{
   digitalWrite(ENABLE_EASY_DRIVER, HIGH);
   digitalWrite(DIR_PIN_EASY_DRIVER, LOW);
   digitalWrite(STEP_PIN_EASY_DRIVER, LOW);  
}

void setup()
{
  Serial.println("Setting up device");
  pinMode(ENABLE_EASY_DRIVER, OUTPUT);
  digitalWrite(ENABLE_EASY_DRIVER, HIGH);
  pinMode(STEP_PIN_EASY_DRIVER, OUTPUT);
  pinMode(DIR_PIN_EASY_DRIVER, OUTPUT);
  digitalWrite(ENABLE_EASY_DRIVER, HIGH);
  pinMode(leftLimitSwitch, INPUT);
  digitalWrite(leftLimitSwitch, HIGH);
  pinMode(rightLimitSwitch, INPUT);
  digitalWrite(rightLimitSwitch, HIGH);
  attachInterrupt(digitalPinToInterrupt(leftLimitSwitch), moveEasyDriverRight, LOW);
  attachInterrupt(digitalPinToInterrupt(rightLimitSwitch), moveEasyDriverLeft, LOW);
  Serial.begin(9600);
  stepper.setSPR(4075.7728395);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
  Serial.println("Setup done");
}
void loop()
{
  leftEndSwitchState = digitalRead(leftLimitSwitch);
  rightEndSwitchState = digitalRead(rightLimitSwitch);
  Serial.println("==============");
  Serial.print("Left end state switch state: ");
  Serial.println(leftEndSwitchState);
  Serial.print("Right end state switch state: ");
  Serial.println(rightEndSwitchState);
 
  /*
    Start reading from NRF module
  */
  if ( radio.available())
  {
      radio.read(&joystickData, sizeof(joystickData)); //read data
      Serial.println(joystickData[0]); // X-Position
      Serial.println(joystickData[1]); // Y-position
      Serial.println(joystickData[2]); // SW Button 
      /*
        Set speed dependently on SW button state
      */
      if(joystickData[2] == 1 && joystickButtonPressed == 0)
      {
        set_speed();
        joystickButtonPressed = 1;
      }
      if(joystickData[2] == 0)
      {
        joystickButtonPressed = 0;  
      }
 
      /*
        Move 28BYJ-48
      */
      
      if(joystickData[1] > 800)
      {
        Serial.println("Rotating 28BYJ-48 ClockWise");
        if (stepper.isDone())
        {
          stepper.setDirection(CW);
          stepper.rotate();
        }
      } else if(joystickData[1] < 300)
      {
        Serial.println("Rotating 28BYJ-48 CounterClockWise");
        if (stepper.isDone())
        {
          stepper.setDirection(CCW);
          stepper.rotate();
        }
      }else
      {
        Serial.print("No 28BYJ-48 action. Value of Y-position is:");
        Serial.println(joystickData[1]);
        if (stepper.isDone())
        {
          stepper.setDirection(STOP);
          stepper.rotate();
        }
      }
       /*
        Easy Driver
        ENABLE_EASY_DRIVER = 2;
        STEP_PIN_EASY_DRIVER = 3;
        DIR_PIN_EASY_DRIVER = 4;
      */
      
      if(joystickData[2] > 800)
      {
        Serial.println("Rotating Easy Driver ClockWise");
        moveEasyDriverLeft();
        
      } else if(joystickData[2] < 300)
      {
        Serial.println("Rotating EasyDriver CounterClockWise");
        moveEasyDriverRight();
      }else
      {
        Serial.print("No EasyDriver action. Value of Y-position is:");
        Serial.println(joystickData[2]);
        stopEasyDriver();
      }
      
  } else 
  {
    Serial.println("No data received from transmitter");
    Serial.println("Stopping Easy Driver");
    stopEasyDriver();
    Serial.println("Easy Driver Stopped");
    Serial.println("Stopping 28BYJ-48");
    stepper.setDirection(STOP);
    stepper.rotate();  
    Serial.println("28BYJ-48  stopped");
  }
  Serial.println("==============");
  Serial.println();
  Serial.println();
  stepper.run();
  if(MAIN_DELAY !=0)
    {delay(MAIN_DELAY);}
}
