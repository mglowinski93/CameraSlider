#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
const int CE = 6;
const int CSN = 7;
int joystickData[3] = {0};
bool joystickButtonPressed = 0;
const byte address[6] = "00001";
const int smallSteperCW = 8; //28BYJ-48 ClockWise
const int smallSteperCCW = 9; //28BYJ-48 CounterClockWise
const int easyDriverCW = 10; //EasyDriver CounterClockWise
const int easyDriverCCW = 11; //EasyDriver CounterClockWise
const int speedChange = 12; //Choose Speed Value

RF24 radio(CE, CSN); // CE, CSN

void setup()
{
  Serial.begin(9600);
  Serial.println("Setting up radio module");
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
  Serial.println("Setup done");
}

void loop()
{
  Serial.println("=============="); 
  /*
    Start reading from NRF module
  */
  if ( radio.available() )
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
        digitalWrite(speedChange, HIGH);
        joystickButtonPressed = 1;
      }
      if(joystickData[2] == 0)
      {
        digitalWrite(speedChange, LOW);
        joystickButtonPressed = 0;  
      }

 
      /*
        Move 28BYJ-48
      */
      if(joystickData[1] > 800)
      {
        Serial.println("Rotating 28BYJ-48 ClockWise");
        digitalWrite(smallSteperCW, HIGH);
        digitalWrite(smallSteperCCW, LOW);
      } else if(joystickData[1] < 300)
      {
        Serial.println("Rotating 28BYJ-48 CounterClockWise");
        digitalWrite(smallSteperCW, LOW);
        digitalWrite(smallSteperCCW, HIGH);
      }else
      {
        Serial.print("No 28BYJ-48 action. Value of Y-position is:");
        Serial.println(joystickData[1]);
        digitalWrite(smallSteperCW, LOW);
        digitalWrite(smallSteperCCW, LOW);
      }

       /*
        Move EasyDriver
      */
      if(joystickData[0] > 800)
      {
        Serial.println("Rotating EasyDriver ClockWise");
        digitalWrite(easyDriverCW, HIGH);
        digitalWrite(easyDriverCCW, LOW);
      } else if(joystickData[0] < 300)
      {
        Serial.println("Rotating EasyDriver CounterClockWise");
        digitalWrite(easyDriverCW, LOW);
        digitalWrite(easyDriverCCW, HIGH);
      }else
      {
        Serial.print("No EasyDriver action. Value of Y-position is:");
        Serial.println(joystickData[0]);
        digitalWrite(easyDriverCW, LOW);
        digitalWrite(easyDriverCCW, LOW);
      }
  
  } else 
  {
    Serial.println("No data received from transmitter");
    Serial.println("Stopping Easy Driver");
    digitalWrite(easyDriverCW, LOW);
    digitalWrite(easyDriverCCW, LOW);
    Serial.println("Easy Driver Stopped");
    Serial.println("Stopping 28BYJ-48");
    digitalWrite(smallSteperCW, LOW);
    digitalWrite(smallSteperCCW, LOW);
    Serial.println("28BYJ-48  stopped");
  }
  Serial.println("==============");
  Serial.println();
  Serial.println();
}
