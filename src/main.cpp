#include <Arduino.h>
#include "BTS7960.h"

const uint8_t pEN = 4;
const uint8_t pL_PWM = 5;
const uint8_t pR_PWM = 6;

BTS7960 panMotorCtrl(pEN, pL_PWM, pR_PWM);

const uint8_t tEN = 16;
const uint8_t tL_PWM = 9;
const uint8_t tR_PWM = 10;

BTS7960 tiltMotorCtrl(tEN, tL_PWM, tR_PWM);

#define rightBtn 8
#define leftBtn 7
#define upBtn 14
#define downBtn 15

#define intPanRight 0
#define intPanLeft 1
#define intTiltUp 2
#define intTiltDown 3

bool panRightStop = false;
bool panLeftStop = false;
bool tiltUpStop = false;
bool tiltDownStop = false;

// ######## CONFIGURATION ############################
#define DEADZONE 0
#define DEBOUNCE_VAL 7500
#define RAMP_UP_SPEED_PAN 10
// #define RAMP_DOWN_SPEED_PAN 30
#define MIN_SPEED_PAN 0
#define MAX_SPEED_PAN 255

#define RAMP_UP_SPEED_TILT 10
// #define RAMP_DOWN_SPEED_TILT 2
#define MIN_SPEED_TILT 0
#define MAX_SPEED_TILT 255
//####################################################

//############# FUNCTION DEFINITIONS ##############################

void moveMotors(int pan, int tilt);
void panLeftInt();
void tiltUpInt();
void tiltDownInt();

int pan = 0;
int tilt = 0;
String Data_In = "";

void setup()
{
  // init interupt pins and atach the Interupt function

  // pinMode(intPanRight, INPUT_PULLUP);
  // attachInterrupt(digitalPinToInterrupt(intPanRight), panRightInt, RISING);
  // pinMode(intPanLeft, INPUT_PULLUP);
  // attachInterrupt(digitalPinToInterrupt(intPanLeft), panLeftInt, RISING);

  pinMode(intTiltUp, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(intTiltUp), tiltUpInt, RISING);
  pinMode(intTiltDown, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(intTiltDown), tiltDownInt, RISING);

  // set pinmodes for joystick input
  pinMode(rightBtn, INPUT_PULLUP);
  pinMode(leftBtn, INPUT_PULLUP);
  pinMode(upBtn, INPUT_PULLUP);
  pinMode(downBtn, INPUT_PULLUP);
  Serial.begin(115200);
}

void loop()
{
  if (Serial.available() > 0)
  {
    Data_In = Serial.readStringUntil('#');
    if (Data_In == "whoRu")
    {
      Serial.println("SnowBlower_#");
    }
    else
    { // PARSE DATA
      // Serial.println(Data_In);//used for debug
      String tempPan = Data_In.substring(0, Data_In.indexOf(","));
      String tempTilt = Data_In.substring(Data_In.indexOf(",") + 1, Data_In.indexOf("#"));
      pan = tempPan.toInt();
      tilt = tempTilt.toInt();
      moveMotors(pan, tilt);
    }
  }
} // END MAIN LOOP

void moveMotors(int pan, int tilt)
{
  tiltMotorCtrl.Enable();
  panMotorCtrl.Enable();
  if (tilt > 0)
  {
    tilt = map(tilt, 0, 1023, 0, 254),
    tiltMotorCtrl.TurnLeft(tilt);
    tiltDownStop = false;
  }
  else if (tilt < 0)
  {
    tilt = (tilt * -1);
    tilt = map(tilt, 0, 1023, 0, 254),
    tiltMotorCtrl.TurnRight(tilt);
    tiltUpStop = false;
  }
  else if (tilt == 0)
  {
    tiltMotorCtrl.Stop();
    tiltMotorCtrl.Disable();
  }

  if (pan < 0)
  {
    pan = (pan * -1);
    pan = map(pan, 0, 1023, 0, 254);
    panMotorCtrl.TurnLeft(pan);
    panRightStop = false;
  }
  else if (pan > 0)
  {
    pan = map(pan, 0, 1023, 0, 254);
    panMotorCtrl.TurnRight(pan);
    panLeftStop = false;
  }
  else if (pan == 0)
  {
    panMotorCtrl.Stop();
    panMotorCtrl.Disable();
  }
  if (tiltDownStop == true || tiltUpStop == true)
  {
    tiltMotorCtrl.Stop();
    tiltMotorCtrl.Disable();
  }
  // if (panRightStop == true || panLeftStop == true)
  // {
  //   panMotorCtrl.Stop();
  //   panMotorCtrl.Disable();
  // }
}

// INTERUPT FUNCTIONS
void panRightInt()
{
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  // If interrupts come faster than 200ms, assume it's a bounce and ignore
  if (interrupt_time - last_interrupt_time > DEBOUNCE_VAL)
  {
    if (panRightStop == false && pan > 0)
    {
      Serial.println("pan Right Stop");
      panRightStop = true;
      panLeftStop = false;
      moveMotors(0, 0);
    }
    last_interrupt_time = interrupt_time;
  }
}
void panLeftInt()
{
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  // If interrupts come faster than 200ms, assume it's a bounce and ignore
  if (interrupt_time - last_interrupt_time > DEBOUNCE_VAL)
  {
    if (panLeftStop == false && pan < 0)
    {
      Serial.println("pan Left Stop");
      panLeftStop = true;
      panRightStop = false;
      moveMotors(0, 0);
    }
    last_interrupt_time = interrupt_time;
  }
}
void tiltUpInt()
{
  unsigned long interrupt_time = millis();
  static unsigned long last_interrupt_time = interrupt_time;
  // If interrupts come faster than 200ms, assume it's a bounce and ignore
  if (interrupt_time - last_interrupt_time > DEBOUNCE_VAL)
  {
    if (tiltUpStop == false && tilt > 0)
    {
      Serial.println("tilt Up Stop");
      Serial.println(interrupt_time);
      Serial.println(last_interrupt_time);
      tiltUpStop = true;
      tiltDownStop = false;
      moveMotors(0, 0);
    }
    last_interrupt_time = interrupt_time;
  }
}
void tiltDownInt()
{
  unsigned long interrupt_time = millis();
  static unsigned long last_interrupt_time = interrupt_time;
  // If interrupts come faster than 200ms, assume it's a bounce and ignore
  if (interrupt_time - last_interrupt_time > DEBOUNCE_VAL)
  {
    if (tiltDownStop == false && tilt < 0)
    {
      Serial.println("tilt Down Stop");
      tiltDownStop = true;
      tiltUpStop = false;
      moveMotors(0, 0);
    }
    last_interrupt_time = interrupt_time;
  }
}