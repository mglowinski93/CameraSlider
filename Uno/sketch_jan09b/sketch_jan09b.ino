
#include "AccelStepper.h" 
#include "MultiStepper.h" 
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>


RF24 radio(7, 8); // CE, CSN
const byte address[6] = "00001";

AccelStepper stepper(1, 3, 4);   // 1 = Easy Driver interface
                                  // UNO Pin 3 connected to STEP pin of Easy Driver
                                  // UNO Pin 4 connected to DIR pin of Easy Driver

MultiStepper StepperControl;  // Create instance of MultiStepper
                                  
// Analog Joystick setup
#define JoyX A0  // Joystick X pin connected to A0 on the UNO
#define JoyY A1  // Joystick Y pin connected to A1 on the UNO
#define JoySwitch 2 // Joystick switch connected to interrupt Pin 2 on UNO

int joystepX=0; // Used to move steppers when using the Joystick
int joystepY=0; // Used to move steppers when using the Joystick
int joystick_status[3] = {0,0,0};

// Variables used to store the IN and OUT points
volatile long XInPoint=0;
volatile long ZInPoint=0;
volatile long XOutPoint=0;
volatile long ZOutPoint=0;

int InandOut=0;  // Used to detect if IN and OUT points have been set

long gotoposition[2];  // Used to move steppers using MultiStepper Control



void joyswitchclick ()  {  //  Interrupt runs when clicking the Joystick Switch

  static unsigned long last_call_interrupt = 0;  // Used to debounce to Joystick Switch
  unsigned long interrupt_length = millis();
  
  if (interrupt_length - last_call_interrupt > 300) {  // Check if enough time has passed since clicking switch
    
    switch (InandOut) {  // Keep track of number of times the switch has been clicked
      case 0:
        InandOut=1;
        XInPoint=stepperX.currentPosition();  //  Set the IN points for both steppers
        ZInPoint=stepperZ.currentPosition();
        Serial.print("IN Points set at:  ");
        Serial.print(XInPoint);
        Serial.print(" , ");
        Serial.println(ZInPoint);
        Serial.println("");
        Serial.println("Use Joystick to set OUT Points: ");
        break;

      case 1:
        InandOut=2;
        XOutPoint=stepperX.currentPosition();  //  Set the OUT Points for both steppers
        ZOutPoint=stepperZ.currentPosition();
        Serial.print("OUT Points set at:  ");
        Serial.print(XOutPoint);
        Serial.print(" , ");
        Serial.println(ZOutPoint);
        Serial.println("");
        Serial.println("Enter Stepper Travel Speed: ");
        Serial.println("OR");
        Serial.println("Press Joystick Switch again to Reset IN and OUT Points");
        break;

      case 2:
        Serial.println("");
        Serial.println("IN and OUT Points have been Reset...");
        Serial.println("");
        Serial.println("");
        Serial.println("Set IN points for Steppers using the Joystick ");
        InandOut=0;
        break;

       case 3:
        Serial.println("");
        Serial.println("IN and OUT Points have been Reset...");
        Serial.println("");
        Serial.println("");
        Serial.println("Set IN points for Steppers using the Joystick ");
        InandOut=0;  
        joystepY=stepperZ.currentPosition();  // Reset Positions
        joystepX=stepperX.currentPosition();
        stepperX.setMaxSpeed(350.0);      // Reset Max Speed of X axis
        break;
    }
  }
  last_call_interrupt = interrupt_length;
} 


void setup() {
  
  Serial.begin(9600);  // Start the Serial monitor with speed of 9600 Bauds
  
// Print out Instructions on the Serial Monitor at Start
  Serial.println("Set IN points for Steppers using the Joystick ");

  pinMode(JoySwitch, INPUT_PULLUP);

// Set Max Speed and Acceleration of each Steppers
  stepperX.setMaxSpeed(350.0);      // Set Max Speed of X axis
  stepperX.setAcceleration(5000.0);  // Acceleration of X axis
  
  stepperZ.setMaxSpeed(150.0);      // Set Max Speed of Z axis slower for rotation
  stepperZ.setAcceleration(1000.0);  // Acceleration of Y axis

// Create instances for MultiStepper
  StepperControl.addStepper(stepperX);  // Add Stepper #1 to MultiStepper Control
  StepperControl.addStepper(stepperZ);  // Add Stepper #2 to MultiStepper Control

  attachInterrupt (0,joyswitchclick,FALLING); // interrupt 0 always connected to pin 2 
  
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();

}


void loop() {
    if (radio.available()) 
    {

      radio.read(&joystick_status, sizeof(joystick_status));
      Serial.println(text);
      joystepX=joystick_status[0];
      joystepY=joystick_status[1];
      bool=joystick_status[2];
    }

  while ((InandOut != 2) && (InandOut !=3)) {  // Enable moving of steppers using the Joystick

    if (analogRead(JoyX) < 200) {         
        joystepX=stepperX.currentPosition();         
        joystepX=joystepX-20;     }     

    if (analogRead(JoyX) > 900) {
        joystepX=stepperX.currentPosition();
        joystepX=joystepX+20;
    }

    if (analogRead(JoyY) < 200) {         
        joystepY=stepperZ.currentPosition();         
        joystepY=joystepY-20;     }          

    if (analogRead(JoyY) > 900) {
        joystepY=stepperZ.currentPosition();
        joystepY=joystepY+20;
    }

    stepperX.moveTo(joystepX);
    stepperZ.moveTo(joystepY);

    while ((stepperX.distanceToGo() !=0) || (stepperZ.distanceToGo() !=0)) {
      stepperX.runSpeedToPosition();
      stepperZ.runSpeedToPosition();
    }
  }
  
  if (InandOut == 2) {

    while (Serial.available() > 0)  { // Check if values are available in the Serial Buffer
  
      int setspeed = Serial.parseInt();  //  Read entered Speed from Serial Buffer
    
       if (setspeed > 0) {
         stepperX.setMaxSpeed(setspeed);
         Serial.println("");
         Serial.println("Speed set at:  ");
         Serial.println(setspeed);
         Serial.println("");
         Serial.println("Type 'I' to go to the IN points or 'O' to go to the OUT points");
         Serial.println("OR");
         Serial.println("Press Joystick Switch again to Reset IN and OUT Points");
         setspeed=0;
         InandOut=3;
      }
    }
  }
    
  if (InandOut == 3) {

    while (Serial.available() > 0)  { // Check if values are available in the Serial Buffer
  
      char checkinput = Serial.read();  //  Read Character entered in Serial Monitor
      Serial.println(checkinput);
    
    if (checkinput == 'I') {  // Move both steppers to IN points
      gotoposition[0]=XInPoint;
      gotoposition[1]=ZInPoint;
      StepperControl.moveTo(gotoposition);
      StepperControl.runSpeedToPosition(); // Blocks until all are in position
      Serial.println("Steppers are at the IN Position...");
      Serial.println("");
      Serial.println("Type 'I' to go to the IN points or 'O' to go to the OUT points");
      Serial.println("OR");
      Serial.println("Press Joystick Switch again to Reset IN and OUT Points");
    }
    
    if (checkinput == 'O') {  //  Move both steppers to OUT points
      gotoposition[0]=XOutPoint;
      gotoposition[1]=ZOutPoint;
      StepperControl.moveTo(gotoposition);
      StepperControl.runSpeedToPosition(); // Blocks until all are in position
      Serial.println("Steppers are at the OUT Position...");
      Serial.println("");
      Serial.println("Type 'I' to go to the IN points or 'O' to go to the OUT points");
      Serial.println("OR");
      Serial.println("Press Joystick Switch again to Reset IN and OUT Points");
    }

  }
}
    joystepX=0;
    joystepY=0;
}
