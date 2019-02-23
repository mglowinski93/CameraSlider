/*
Library include
*/
#include <HID.h>
#include <SPI.h>
#include <nRF24L01.h> //Library can be downloaded from here: https://github.com/nRF24/RF24
#include <RF24.h>

/*
Define global variables
*/
const int SW_pin = 2; // digital pin connected to switch output. In our case it will be used to interrupt move of stepper
const int X_pin = 0; // analog pin connected to X output
const int Y_pin = 1; // analog pin connected to Y output
const int CE = 7; //need for NRF24L01
const int CSN = 8; //need for NRF24L01
bool SW; //variable reserved for JoyStick Push State. WILL WE NEED THIS FOR SOMETHING?
int Xasis, Yasis; //JoyStick position

/*
Prepare NRF Object
More about NRF24L01: https://howtomechatronics.com/tutorials/arduino/arduino-wireless-communication-nrf24l01-tutorial/
*/
RF24 radio(CE, CSN); // CE, CSN you can define as you wish:)
const byte address[6] = "00001"; //address (pipe) through which two moduled will communicate. There is possibility to choose 5  letter string. This enables the choose to which receiver we will talk. It has to be the same on transmitter and receiver.  
int joystick_position[3] = {0,0,0}; //array with data to send via NRF24L01

/*
Read Joystick State
*/
void joyStick()
{
  SW = digitalRead(SW_pin);
  Xasis = analogRead(X_pin);
  Yasis = analogRead(Y_pin);
}

void setup() {
  //There is no need to initializate analog ports. By default there are inputs. That is why i'm not initializating 
  pinMode(SW_pin, INPUT);
  digitalWrite(SW_pin, HIGH);
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);//This function set the Power Amplifier Level, depends on distance between modules. It distance will be more than 1m, value needs to be changed
  radio.stopListening(); //This function set module as Transmitter
}

void loop() {
  joyStick();
  joystick_position[0] = X_pin;
  joystick_position[1] = Y_pin;
  joystick_position[2] = SW_pin;

  //I'm not sure if it will work. I've never used that in such a way.
  radio.write(&joystick_position, sizeof(joystick_position));
  delay(1000);
}
