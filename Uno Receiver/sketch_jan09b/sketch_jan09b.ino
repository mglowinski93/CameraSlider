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
int joystickData[3] = {0};
bool joystickButtonPressed = 0;
const byte address[6] = "00001";
const int MAX_SPEED = 2000;
const int SPEED_LOW = 1000;
const int SPEED_FAST = 2000;

RF24 radio(CE, CSN); // CE, CSN
AccelStepper stepper_28BYJ_48(8, mtrPin1, mtrPin3, mtrPin2, mtrPin4);
//AccelStepper stepper_easy_driver(AccelStepper::FULL4WIRE, 6, 7, 8, 9);

void setup()
{
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
  stepper_28BYJ_48.setMaxSpeed(MAX_SPEED);
  stepper_28BYJ_48.setAcceleration(500.0);
  stepper_28BYJ_48.setSpeed(SPEED_LOW);
  //stepper_easy_driver.setMaxSpeed(MAX_SPEED);
  //stepper_easy_driver.setSpeed(SPEED_LOW);
}

void loop()
{
  //Serial.println("=============="); 
  /*
    Start reading from NRF module
  */
  if ( radio.available() )
  {
      bool done = false;
      
      while (radio.available())
      {
        radio.read(&joystickData, sizeof(joystickData));
      }
      
      /*
        Set speed dependently on SW button state
      */
      if(joystickData[2] == 1 && joystickButtonPressed == 0)
      {
        stepper_28BYJ_48.setSpeed(SPEED_FAST);
        //stepper_easy_driver.setSpeed(SPEED_FAST);
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
    stepper_28BYJ_48.stop();
  }
}
