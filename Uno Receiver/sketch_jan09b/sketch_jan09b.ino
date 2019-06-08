#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <AccelStepper.h>
 
#define mtrPin1  2     // IN1 on the ULN2003 driver 1
#define mtrPin2  3     // IN2 on the ULN2003 driver 1
#define mtrPin3  4     // IN3 on the ULN2003 driver 1
#define mtrPin4  5     // IN4 on the ULN2003 driver 1
#define easyDriverStep 9
#define easyDriverDir 8
#define endSwitchLeft A0
#define endSwitchRight A1
 
const int CE = 6;
const int CSN = 7;
int joystickData[3] = {0, 0, 1};
bool joystickButtonPressed = 0;
const byte address[6] = "00001";
const int MAX_SPEED_28BYJ_48 = 1000;
const int SPEED_SLOW_28BYJ_48 = 50;
const int SPEED_FAST_28BYJ_48 = 400;
const int MAX_SPEED_EASY_DRIVER = 3000;
const int SPEED_SLOW_EASY_DRIVER = 50;
const int SPEED_FAST_EASY_DRIVER= 3000;
 
unsigned long actualTime = 0;
unsigned long storedTime = 0;
unsigned long radioRefreshValue = 100;
int currentSpeed_28BYJ_48 = 0;
int currentSpeed_EASY_DRIVER = 0;
bool stepper_28BYJ_48_direction_set = 0;
bool stepper_easy_driver_direction_set = 0;
 
RF24 radio(CE, CSN); // CE, CSN
AccelStepper stepper_28BYJ_48(AccelStepper::FULL4WIRE, mtrPin1, mtrPin3, mtrPin2, mtrPin4);
AccelStepper stepper_easy_driver(AccelStepper::DRIVER, easyDriverStep, easyDriverDir);
void setup()
{  
   radio.begin();
   radio.openReadingPipe(0, address);
   radio.setPALevel(RF24_PA_MIN);
   radio.startListening();
   stepper_28BYJ_48.setMaxSpeed(MAX_SPEED_28BYJ_48);
   stepper_28BYJ_48.setSpeed(SPEED_FAST_28BYJ_48);
   stepper_easy_driver.setMaxSpeed(MAX_SPEED_EASY_DRIVER);
   stepper_easy_driver.setSpeed(SPEED_FAST_EASY_DRIVER);
   currentSpeed_28BYJ_48 = stepper_28BYJ_48.speed();
   currentSpeed_EASY_DRIVER = stepper_easy_driver.speed();
   pinMode(endSwitchRight, INPUT);
   pinMode(endSwitchLeft, INPUT);
   digitalWrite(endSwitchRight, HIGH);
   digitalWrite(endSwitchLeft, HIGH);
}

void loop()
{  
    if ( radio.available() )
    {    
      actualTime = millis();
      if (actualTime - storedTime >= radioRefreshValue)
      {
        storedTime = actualTime;
        radio.read(&joystickData, sizeof(joystickData));
      }

      if(joystickData[2] == 0 && joystickButtonPressed == 0)
      {
        if( currentSpeed_28BYJ_48 == SPEED_FAST_28BYJ_48 )
        {
          currentSpeed_28BYJ_48 = SPEED_SLOW_28BYJ_48;
          currentSpeed_EASY_DRIVER = SPEED_SLOW_EASY_DRIVER;
        }else
        {
          currentSpeed_28BYJ_48 = SPEED_FAST_28BYJ_48;
          currentSpeed_EASY_DRIVER = SPEED_FAST_EASY_DRIVER; 
        }
        joystickButtonPressed = 1;
      }
      if(joystickData[2] == 1)
      {
        joystickButtonPressed = 0;  
      }

      if(joystickData[1] > 800)
      {
        if( !stepper_28BYJ_48_direction_set )
        {
          stepper_28BYJ_48.setSpeed(currentSpeed_28BYJ_48);
          stepper_28BYJ_48_direction_set = true;
        }
        stepper_28BYJ_48.runSpeed();
      }

      if(joystickData[1] < 300)
      {
        if( !stepper_28BYJ_48_direction_set )
        {
          stepper_28BYJ_48.setSpeed(-currentSpeed_28BYJ_48);
          stepper_28BYJ_48_direction_set = true;
        }
        stepper_28BYJ_48.runSpeed();
      }

      if( joystickData[1] >= 300 && joystickData[1] <= 800 && stepper_28BYJ_48_direction_set )
      {
        stepper_28BYJ_48_direction_set = false;
      }
      
      if(joystickData[0] > 800  && digitalRead(endSwitchLeft) == LOW )
      {
        if( !stepper_easy_driver_direction_set )
        {
          stepper_easy_driver.setSpeed(currentSpeed_EASY_DRIVER);
          stepper_easy_driver_direction_set = true;
        }
        stepper_easy_driver.runSpeed();
      }

      if(joystickData[0] < 300 && digitalRead(endSwitchRight) == LOW )
      {
        if( !stepper_easy_driver_direction_set )
        {
          stepper_easy_driver.setSpeed(-currentSpeed_EASY_DRIVER);
          stepper_easy_driver_direction_set = true;
        }
        stepper_easy_driver.runSpeed();
      }

      if( joystickData[0] >= 300 && joystickData[0] <= 800 && stepper_easy_driver_direction_set )
      {
       stepper_easy_driver_direction_set = false;
      }
    }
}
