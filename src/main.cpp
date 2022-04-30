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

void panRightInt();
void panLeftInt();
void tiltUpInt();
void tiltDownInt();

String dir = "stop";

void setup()
{
  // init interupt pins and atach the Interupt function
  pinMode(intPanRight, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(intPanRight), panRightInt, RISING);
  pinMode(intPanLeft, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(intPanLeft), panLeftInt, RISING);
  // pinMode(intTiltUp, INPUT_PULLUP);
  // attachInterrupt(digitalPinToInterrupt(intTiltUp), tiltUpInt, FALLING);
  // pinMode(intTiltDown, INPUT_PULLUP);
  // attachInterrupt(digitalPinToInterrupt(intTiltDown), tiltDownInt, FALLING);

  // set pinmodes for joystick input
  pinMode(rightBtn, INPUT_PULLUP);
  pinMode(leftBtn, INPUT_PULLUP);
  pinMode(upBtn, INPUT_PULLUP);
  pinMode(downBtn, INPUT_PULLUP);
  Serial.begin(115200);
}

void loop()
{
  if (digitalRead(rightBtn) == 0 && dir != "right" && panRightStop == false)
  {
    panLeftStop = false;
    panMotorCtrl.Enable();
    dir = "right";
    Serial.println(dir);
    for (int speed = MIN_SPEED_PAN; speed < MAX_SPEED_PAN; speed += 3)
    {
      panMotorCtrl.TurnRight(speed);
      if (digitalRead(rightBtn) == 1 || panRightStop == true)
      {
        panMotorCtrl.Stop();
        panMotorCtrl.Disable();
        break;
      }
      delay(RAMP_UP_SPEED_PAN);
    }
  }
  else if (digitalRead(leftBtn) == 0 && dir != "left" && panLeftStop == false)
  {
    panRightStop = false;
    panMotorCtrl.Enable();
    dir = "left";
    Serial.println(dir);
    for (int speed = MIN_SPEED_PAN; speed < MAX_SPEED_PAN; speed += 3)
    {
      panMotorCtrl.TurnLeft(speed);
      if (digitalRead(leftBtn) == 1 || panLeftStop == true)
      {
        panMotorCtrl.Stop();
        panMotorCtrl.Disable();
        break;
      }
      delay(RAMP_UP_SPEED_PAN);
    }
  }
  else if (digitalRead(upBtn) == 0 && dir != "up")
  {
    tiltMotorCtrl.Enable();
    dir = "up";
    Serial.println(dir);
    for (int speed = MIN_SPEED_TILT; speed < MAX_SPEED_TILT; speed += 3)
    {
      tiltMotorCtrl.TurnLeft(speed);
      if (digitalRead(upBtn) == 1)
      {
        tiltMotorCtrl.Stop();
        tiltMotorCtrl.Disable();
        break;
      }
      delay(RAMP_UP_SPEED_TILT);
    }
  }
  else if (digitalRead(downBtn) == 0 && dir != "down")
  {
    tiltMotorCtrl.Enable();
    dir = "down";
    Serial.println(dir);
    for (int speed = MIN_SPEED_TILT; speed < MAX_SPEED_TILT; speed += 3)
    {
      tiltMotorCtrl.TurnRight(speed);
      if (digitalRead(downBtn) == 1)
      {
        tiltMotorCtrl.Stop();
        tiltMotorCtrl.Disable();
        break;
      }
      delay(RAMP_UP_SPEED_TILT);
    }
  }
  else if (dir != "stop" && digitalRead(rightBtn) == 1 && digitalRead(leftBtn) == 1 &&
           digitalRead(upBtn) == 1 && digitalRead(downBtn) == 1)
  {
    dir = "stop";
    Serial.println(dir);
    panMotorCtrl.Stop();
    panMotorCtrl.Disable();
    tiltMotorCtrl.Stop();
    tiltMotorCtrl.Disable();
    delay(100);
  }
}

void panRightInt()
{
  if (panRightStop == false)
  {
    Serial.println("pan Right Stop");
    panRightStop = true;
  }
}
void panLeftInt()
{
  if (panLeftStop == false)
  {
    Serial.println("pan Left Stop");
    panLeftStop = true;
  }
}
void tiltUpInt()
{
  Serial.println("tilt Up Stop");
  tiltUpStop = true;
}
void tiltDownInt()
{
  Serial.println("tilt Down Stop");
  tiltDownStop = true;
}