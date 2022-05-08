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

#define intPanRight 1
#define intPanLeft 0
#define intTiltUp 3
#define intTiltDown 2

bool panRightStop = false;
bool panLeftStop = false;
bool tiltUpStop = false;
bool tiltDownStop = false;

// ######## CONFIGURATION ############################
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

void moveMotors(String dir, int speed);
void panLeftInt();
void tiltUpInt();
void tiltDownInt();

String dir = "stop";
int speed = 0;
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
    // todo setup data Parser
    if (Data_In == "whoRu")
    {
      Serial.println("SnowBlower-#");
    }
    else
    {
      dir = "stop";
      speed = 255;
    }
    if (Data_In == "pan")
    {
      dir = "up";
      moveMotors(dir, speed);
    }
    else if (Data_In == "down")
    {
      dir = "down";
      moveMotors(dir, speed);
    }
  }

} // END MAIN LOOP

void moveMotors(String dir, int speed)
{
  tiltMotorCtrl.Enable();
  panMotorCtrl.Enable();
  Serial.println(dir);
  if (dir == "tilt")
  {
    if (speed > 200)
    {
      tiltMotorCtrl.TurnLeft(speed);
      tiltDownStop = false;
    }
    else if (speed < -200)
    {
      tiltMotorCtrl.TurnRight(speed);
      tiltUpStop = false;
    }
    if (digitalRead(upBtn) == 1 || tiltUpStop == true)
    {
      tiltMotorCtrl.Stop();
      tiltMotorCtrl.Disable();
    }
  }

  else if (dir == "pan")
  {
    if (speed > 200)
    {
      panLeftStop = false;
      panMotorCtrl.TurnLeft(speed);
    }
    else if (speed < -200)
    {
      panMotorCtrl.TurnRight(speed);
      panRightStop = false;
    }
    if (digitalRead(leftBtn) == 1 || panLeftStop == true)
    {
      panMotorCtrl.Stop();
      panMotorCtrl.Disable();
    }
  }
  else if (dir == "stop")
  {
    tiltMotorCtrl.Stop();
    tiltMotorCtrl.Disable();
    panMotorCtrl.Stop();
    panMotorCtrl.Disable();
  }
}

void panRightInt()
{
  if (panRightStop == false && dir == "right")
  {
    Serial.println("pan Right Stop");
    panRightStop = true;
    panLeftStop = false;
    dir = "stop";
  }
}
void panLeftInt()
{
  if (panLeftStop == false && dir == "left")
  {
    Serial.println("pan Left Stop");
    panLeftStop = true;
    panRightStop = false;
    dir = "stop";
  }
}
void tiltUpInt()
{
  if (tiltUpStop == false && dir == "up")
  {
    Serial.println("tilt Up Stop");
    tiltUpStop = true;
    tiltDownStop = false;
    dir = "stop";
  }
}
void tiltDownInt()
{
  if (tiltDownStop == false && dir == "down")
  {

    Serial.println("tilt Down Stop");
    tiltDownStop = true;
    tiltUpStop = false;
    dir = "stop";
  }
}