#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <AccelStepper.h>
 
#define mtrPin1  2     // IN1 on the ULN2003 driver 1
#define mtrPin2  3     // IN2 on the ULN2003 driver 1
#define mtrPin3  4     // IN3 on the ULN2003 driver 1
#define mtrPin4  5     // IN4 on the ULN2003 driver 1
 
const int CE = 6;
const int CSN = 7;
int joystickData[3] = {0, 0, 1};
bool joystickButtonPressed = 0;
const byte address[6] = "00001";
const int MAX_SPEED = 1000;
const int SPEED_LOW = 50;
const int SPEED_FAST = 500;
 
unsigned long actualTime = 0;
unsigned long storedTime = 0;
unsigned long radioRefreshValue = 200;
int currentSpeed = 0;
 
RF24 radio(CE, CSN); // CE, CSN
AccelStepper stepper_28BYJ_48(AccelStepper::FULL4WIRE, mtrPin1, mtrPin3, mtrPin2, mtrPin4);
//AccelStepper stepper_easy_driver(AccelStepper::FULL4WIRE, 6, 7, 8, 9);
 
void setup()
{
  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
  stepper_28BYJ_48.setMaxSpeed(MAX_SPEED);
  stepper_28BYJ_48.setSpeed(SPEED_FAST);
  currentSpeed = SPEED_FAST;
  //stepper_easy_driver.setMaxSpeed(MAX_SPEED);
  //stepper_easy_driver.setSpeed(SPEED_LOW);
}
 
void loop()
{
  /*
    Start reading from NRF module
  */
  if ( radio.available() )
  {    
      actualTime = millis();
      if (actualTime - storedTime >= radioRefreshValue)
      {
        storedTime = actualTime;
        radio.read(&joystickData, sizeof(joystickData));
      }
      /*
        Set speed dependently on SW button state
      */
      if(joystickData[2] == 0 && joystickButtonPressed == 0)
      {
        if( currentSpeed == SPEED_FAST )
        {
          currentSpeed = SPEED_LOW;  
        }else
        {
          currentSpeed = SPEED_FAST;  
        }
        Serial.print("Setting speed to: ");
        Serial.println(currentSpeed);
        stepper_28BYJ_48.setSpeed(currentSpeed);
        //stepper_easy_driver.setSpeed(currentSpeed);
        joystickButtonPressed = 1;
      }
      if(joystickData[2] == 1)
      {
        joystickButtonPressed = 0;  
      }
 
      /*
        Move 28BYJ-48
      */
      if(joystickData[1] > 800)
      {
        stepper_28BYJ_48.runSpeed();
      } else if(joystickData[1] < 300)
      {
        stepper_28BYJ_48.runSpeed();
      }else
      {
        stepper_28BYJ_48.stop();
      }
 
       /*
        Move EasyDriver
      */
      if(joystickData[0] > 800)
      {
        //stepper_easy_driver.runSpeed();
      } else if(joystickData[0] < 300)
      {
        //stepper_easy_driver.runSpeed();
      }else
      {
        //stepper_easy_driver.stop();
      }
 
  } else
  {
    //stepper_easy_driver.stop();
    //stepper_28BYJ_48.stop();
  }
}
