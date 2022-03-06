#include <Arduino.h>
#include "BTS7960.h"

const uint8_t pEN = 4;
const uint8_t pL_PWM = 5;
const uint8_t pR_PWM = 6;

BTS7960 panMotorCtrl(pEN, pL_PWM, pR_PWM);

const uint8_t tEN = 11;
const uint8_t tL_PWM = 9;
const uint8_t tR_PWM = 10;

BTS7960 tiltMotorCtrl(tEN, tL_PWM, tR_PWM);

#define rightBtn 2
#define leftBtn 3
#define upBtn 7
#define downBtn 8

int dir = 0;

void setup()
{
  pinMode(rightBtn, INPUT_PULLUP);
  pinMode(leftBtn, INPUT_PULLUP);
  pinMode(upBtn, INPUT_PULLUP);
  pinMode(downBtn, INPUT_PULLUP);
  // Serial.begin(9600);
}

void loop()
{
  if (digitalRead(rightBtn) == 0 && dir != 1)
  {
    panMotorCtrl.Enable();
    // Serial.println("right");
    dir = 1;
    for (int speed = 0; speed < 255; speed += 30)
    {
      panMotorCtrl.TurnRight(speed);
      if (digitalRead(rightBtn) == 1)
      {
        panMotorCtrl.Stop();
        panMotorCtrl.Disable();
        break;
      }
      delay(10);
    }
  }
  else if (digitalRead(leftBtn) == 0 && dir != 2)
  {
    panMotorCtrl.Enable();
    // Serial.println("left");
    dir = 2;
    for (int speed = 0; speed < 255; speed += 30)
    {
      panMotorCtrl.TurnLeft(speed);
      if (digitalRead(leftBtn) == 1)
      {
        panMotorCtrl.Stop();
        panMotorCtrl.Disable();
        break;
      }
      delay(10);
    }
  }
  else if (digitalRead(upBtn) == 0 && dir != 3)
  {
    tiltMotorCtrl.Enable();
    // Serial.println("up");
    dir = 3;
    for (int speed = 0; speed < 255; speed += 30)
    {
      tiltMotorCtrl.TurnLeft(speed);
      if (digitalRead(upBtn) == 1)
      {
        tiltMotorCtrl.Stop();
        tiltMotorCtrl.Disable();
        break;
      }
      delay(10);
    }
  }
  else if (digitalRead(downBtn) == 0 && dir != 4)
  {
    tiltMotorCtrl.Enable();
    // Serial.println("down");
    dir = 4;
    for (int speed = 0; speed < 255; speed += 30)
    {
      tiltMotorCtrl.TurnRight(speed);
      if (digitalRead(downBtn) == 1)
      {
        tiltMotorCtrl.Stop();
        tiltMotorCtrl.Disable();
        break;
      }
      delay(10);
    }
  }
  else if (dir != 0 && digitalRead(rightBtn) == 1 && digitalRead(leftBtn) == 1 &&
           digitalRead(upBtn) == 1 && digitalRead(downBtn) == 1)
  {
    // Serial.println("stop");
    dir = 0;
    panMotorCtrl.Stop();
    panMotorCtrl.Disable();
  }
}