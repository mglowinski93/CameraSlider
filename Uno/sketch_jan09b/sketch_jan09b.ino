#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <CustomStepper.h>
 
const unsigned short int MAIN_DELAY = 1000;
const unsigned short int requiredStep = 2048;
const int CE = 6;
const int CSN = 7;
int joystickData[3] = {0};
int leftLimitSwitch = A1;
int rightLimitSwitch = A2;
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

void setup()
{
  pinMode(leftLimitSwitch, INPUT);
  digitalWrite(leftLimitSwitch, HIGH);
  pinMode(rightLimitSwitch, INPUT);
  digitalWrite(rightLimitSwitch, HIGH);
  Serial.begin(9600);
  stepper.setSPR(4075.7728395);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
}
void loop()
{
  leftEndSwitchState = digitalRead(leftLimitSwitch);
  rightEndSwitchState = digitalRead(rightLimitSwitch);
  Serial.println(leftEndSwitchState);
  Serial.println(rightEndSwitchState);
  Serial.println("==================");
 
  /*
    Start reading from NRF module
  */
  if ( radio.available())
  {
      radio.read(&joystickData, sizeof(joystickData)); //read data
      Serial.println(joystickData[0]); // X-Position
      Serial.println(joystickData[1]); // Y-position
      Serial.println(joystickData[2]); // SW Button
      Serial.println("@@@@@@@@@@@@@");
 
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
      
  }
   
  stepper.run();        
  if(MAIN_DELAY !=0)
    {delay(MAIN_DELAY);}
}
