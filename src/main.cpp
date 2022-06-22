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

#define rightBtn 7
#define leftBtn 8
#define upBtn 15
#define downBtn 14

#define intPanRight 1
#define intPanLeft 0
#define intTiltUp 2
#define intTiltDown 3

bool panRightStop = false;
bool panLeftStop = false;
bool tiltUpStop = false;
bool tiltDownStop = false;

// ######## CONFIGURATION ############################
#define DEBOUNCE_VAL 500
#define RAMP_UP_SPEED_PAN 2
// #define RAMP_DOWN_SPEED_PAN 30
#define MIN_SPEED_PAN 0
#define MAX_SPEED_PAN 100

#define RAMP_UP_SPEED_TILT 2
// #define RAMP_DOWN_SPEED_TILT 2
#define MIN_SPEED_TILT 0
#define MAX_SPEED_TILT 150
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

  // pinMode(intPanRight, INPUT_PULLUP);
  // attachInterrupt(digitalPinToInterrupt(intPanRight), panRightInt, FALLING);
  // pinMode(intPanLeft, INPUT_PULLUP);
  // attachInterrupt(digitalPinToInterrupt(intPanLeft), panLeftInt, FALLING);

  pinMode(intTiltUp, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(intTiltUp), tiltUpInt, HIGH);
  pinMode(intTiltDown, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(intTiltDown), tiltDownInt, HIGH);

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
    for (int speed = MIN_SPEED_PAN; speed < MAX_SPEED_PAN; speed += RAMP_UP_SPEED_PAN)
    {
      panMotorCtrl.TurnLeft(speed);
      if (digitalRead(rightBtn) == 1 || panRightStop == true)
      {
        panMotorCtrl.Stop();
        panMotorCtrl.Disable();
        break;
      }
      delay(10);
    }
  }
  else if (digitalRead(leftBtn) == 0 && dir != "left" && panLeftStop == false)
  {
    panRightStop = false;
    panMotorCtrl.Enable();
    dir = "left";
    Serial.println(dir);
    for (int speed = MIN_SPEED_PAN; speed < MAX_SPEED_PAN; speed += RAMP_UP_SPEED_PAN)
    {
      panMotorCtrl.TurnRight(speed);
      if (digitalRead(leftBtn) == 1 || panLeftStop == true)
      {
        panMotorCtrl.Stop();
        panMotorCtrl.Disable();
        break;
      }
      delay(10);
    }
  }
  else if (digitalRead(upBtn) == 0 && dir != "up" && tiltUpStop == false)
  {
    tiltDownStop = false;
    tiltMotorCtrl.Enable();
    dir = "up";
    Serial.println(dir);
    for (int speed = MIN_SPEED_TILT; speed < MAX_SPEED_TILT; speed += RAMP_UP_SPEED_TILT)
    {
      tiltMotorCtrl.TurnRight(speed);
      if (digitalRead(upBtn) == 1 || tiltUpStop == true)
      {
        tiltMotorCtrl.Stop();
        tiltMotorCtrl.Disable();
        break;
      }
      delay(10);
    }
  }
  else if (digitalRead(downBtn) == 0 && dir != "down" && tiltDownStop == false)
  {
    tiltUpStop = false;
    tiltMotorCtrl.Enable();
    dir = "down";
    Serial.println(dir);
    for (int speed = MIN_SPEED_TILT; speed < MAX_SPEED_TILT; speed += RAMP_UP_SPEED_TILT)
    {
      tiltMotorCtrl.TurnLeft(speed);
      if (digitalRead(downBtn) == 1 || tiltDownStop == true)
      {
        tiltMotorCtrl.Stop();
        tiltMotorCtrl.Disable();
        break;
      }
      delay(10);
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
  if (tiltDownStop == true || tiltUpStop == true ||
      panLeftStop == true || panRightStop == true)
  {
    if (dir != "stop")
    {
      dir = "stop";
      Serial.println(dir);
    }
    panMotorCtrl.Stop();
    panMotorCtrl.Disable();
    tiltMotorCtrl.Stop();
    tiltMotorCtrl.Disable();
    delay(100);
  }
} // END MAIN LOOP

// INTERUPT FUNCTIONS
void panRightInt()
{
  static unsigned long last_interrupt_time_R = 0;
  unsigned long interrupt_time_R = millis();
  // If interrupts come faster than DEBOUNCE_VALms, assume it's a bounce and ignore
  if (interrupt_time_R - last_interrupt_time_R > DEBOUNCE_VAL)
  {
    if (panRightStop == false && dir == "right")
    {
      Serial.println("pan Right Stop");
      panRightStop = true;
      dir = "stop";
    }
    last_interrupt_time_R = interrupt_time_R;
  }
}
void panLeftInt()
{
  static unsigned long last_interrupt_time_L = 0;
  unsigned long interrupt_time_L = millis();
  // If interrupts come faster than DEBOUNCE_VALms, assume it's a bounce and ignore
  if (interrupt_time_L - last_interrupt_time_L > DEBOUNCE_VAL)
  {
    if (panLeftStop == false && dir == "left")
    {
      Serial.println("pan Left Stop");
      panLeftStop = true;
      dir = "stop";
    }
    last_interrupt_time_L = interrupt_time_L;
  }
}
void tiltUpInt()
{
  unsigned long interrupt_time = millis();
  static unsigned long last_interrupt_time = interrupt_time;
  // If interrupts come faster than DEBOUNCE_VALms, assume it's a bounce and ignore
  if (interrupt_time - last_interrupt_time > DEBOUNCE_VAL)
  {
    if (tiltUpStop == false && dir == "up")
    {
      Serial.println("tilt Up Stop");
      tiltUpStop = true;
      tiltDownStop = false;
      dir = "stop";
    }
    last_interrupt_time = interrupt_time;
  }
}
void tiltDownInt()
{
  unsigned long interrupt_time = millis();
  static unsigned long last_interrupt_time = interrupt_time;
  // If interrupts come faster than DEBOUNCE_VALms, assume it's a bounce and ignore
  if (interrupt_time - last_interrupt_time > DEBOUNCE_VAL)
  {
    if (tiltDownStop == false && dir == "down")
    {

      Serial.println("tilt Down Stop");
      tiltDownStop = true;
      tiltUpStop = false;
      dir = "stop";
    }
    last_interrupt_time = interrupt_time;
  }
}