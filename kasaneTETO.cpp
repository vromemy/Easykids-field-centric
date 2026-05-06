#include <EasyKids3in1.h>

#define RX 26
#define TX 13

float pvYaw = 0;
uint8_t rxCnt = 0, rxBuf[8];

bool readIMU() {
  while (Serial2.available()) {
    rxBuf[rxCnt] = Serial2.read();
    if (rxCnt == 0 && rxBuf[0] != 0xAA) return false;
    rxCnt++;
    if (rxCnt == 8) { // package is complete
      rxCnt = 0;
      if (rxBuf[0] == 0xAA && rxBuf[7] == 0x55) { // data package is correct
        pvYaw = (int16_t)(rxBuf[1] << 8 | rxBuf[2]) / 100.f;
        return true;
      }
    }
  }
  return false;
}

float Power_FL, Power_FR, Power_BL, Power_BR;
float yawOffset = 0;
int spd = 30;

void setup() {
  EasyKids_Setup();
  JoyController_Setup();
  welcomeSong();
  Serial2.begin(115200,SERIAL_8N1,RX,TX);
}


double getYaw() {

  if (readIMU()) {
    return pvYaw;
  }

}


double getYawRad() {

  float yaw = getYaw() - yawOffset;

  // if (yaw > 180) yaw -= 360;
  // if (yaw < -180) yaw += 360;

  return yaw * PI / 180.0;
}


void loop() {

  JoyController();

  float yawRad = -getYawRad();

  float strafe = joyLX / 100.0;
  float forward = joyLY / 100.0;
  float turn = joyRX / 100.0;

  float rotX = strafe * cos(yawRad) - forward * sin(yawRad);
  float rotY = strafe * sin(yawRad) + forward * cos(yawRad);

  Power_FL = rotY + rotX + turn;
  Power_FR = rotY - rotX - turn;
  Power_BL = rotY - rotX + turn;
  Power_BR = rotY + rotX - turn;

  // float maxVal = max(
  //   max(abs(Power_FL), abs(Power_FR)),
  //   max(abs(Power_BL), abs(Power_BR))
  // );

  motor(1,Power_FL * spd);
  motor(2,Power_FR * spd);
  motor(3,Power_BL * spd);
  motor(4,Power_BR * spd);

  if (joyCircle == 1) {
    yawOffset = getYaw();
  }

}
