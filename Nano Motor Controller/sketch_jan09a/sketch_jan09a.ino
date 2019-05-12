#include <CustomStepper.h>
CustomStepper stepper(8, 9, 10, 11, (byte[]){8, B1000, B1100, B0100, B0110, B0010, B0011, B0001, B1001}, 4075.7728395, 12, CW);

const int smallSteperCWPin = 2; //28BYJ-48 ClockWise
const int smallSteperCCWPin = 3; //28BYJ-48 CounterClockWise
const int easyDriverCWPin = 4; //EasyDriver CounterClockWise
const int easyDriverCCWPin = 5; //EasyDriver CounterClockWise
const int speedChangePin = 6; //Choose Speed Value

void setup()
{
  Serial.begin(9600);
  pinMode(smallSteperCWPin, INPUT);
  pinMode(smallSteperCCWPin, INPUT);
  pinMode(easyDriverCWPin, INPUT);
  pinMode(easyDriverCCWPin, INPUT);
  pinMode(speedChangePin, INPUT);
  stepper.setRPM(12);
  stepper.setSPR(4075.7728395);
}

void loop()
{
    if(digitalRead(smallSteperCWPin))
    {
      Serial.println("Starting 28BYJ-48 CW");
      stepper.setDirection(CW);
      Serial.println("28BYJ-48 started CW");
    }else if (digitalRead(smallSteperCCWPin))
    {
      Serial.println("Starting 28BYJ-48 CCW");
      stepper.setDirection(CCW);
      Serial.println("28BYJ-48 started CCW");
    }else
    {
      Serial.println("Stopping 28BYJ-48");
      stepper.setDirection(STOP);
      Serial.println("28BYJ-48 stopped");
    }
    stepper.rotate();
    stepper.run();
}
