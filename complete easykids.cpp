#include <EasyKids3in1.h>

#define RX 13
#define TX 33
#define DOWN_LIMIT 26
#define UP_LIMIT 27
#define Linear1 14
#define Linear2 15

float pvYaw = 0;
uint8_t rxCnt = 0, rxBuf[8];


float Power_FL, Power_FR, Power_BL, Power_BR;
float yawOffset = 0;
float spd = 40;


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

void setup() {
  EasyKids_Setup();
  JoyController_Setup();
  Serial2.begin(115200,SERIAL_8N1,RX,TX);

  pinMode(DOWN_LIMIT, INPUT);
  pinMode(UP_LIMIT, INPUT);
  pinMode(Linear1, OUTPUT);
  pinMode(Linear2, OUTPUT);

  servo(1, 140);

  displayClear();
  display.setTextSize(3);
  display.print(Vbattery());
  display.setCursor(0, 100);
  display.print(batteryPercent());


  welcomeSong();
}


void loop() {

  JoyController();

  float yawRad = -getYawRad();

  float strafe = (joyLX / 100.0) * 0.4;
  float forward = (joyLY / 100.0) * 0.4;
  float turn = (joyRX / 100.0) * 0.4;

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

  // if (maxVal > 100) {
  //   Power_FL /= 100;
  //   Power_FR /= 100;
  //   Power_BL /= 100;
  //   Power_BR /= 100;
  // }

  motor(1,Power_FL * 100);
  motor(4,Power_FR * 100);
  motor(2,Power_BL * 100);
  motor(3,Power_BR * 100);

  if (joyCircle == 1) {
    yawOffset = getYaw();
  }

  lift();
  grab();

  // char buffer[256];
  // sprintf(buffer, "%f, %f", Vbattery(), batteryPercent());
  // Serial.println(buffer);

  // display.print(batteryPercent());
  // displayClear();
  
  // delay(10);
}



void lift() {

  if (joyR2 == 1 && digitalRead(UP_LIMIT)==HIGH ) {
    digitalWrite(Linear1, HIGH);
    digitalWrite(Linear2, HIGH);
  }
  else if (joyL2 == 1 > 0.1 && digitalRead(DOWN_LIMIT)==HIGH) {
    digitalWrite(Linear1, HIGH);
    digitalWrite(Linear2, HIGH);
  }
  else if (joyR2 == 1) {
    digitalWrite(Linear1, HIGH);
    digitalWrite(Linear2, LOW);
  // Serial.println("am rotating");
  }
  else if (joyL2 == 1) {
    digitalWrite(Linear1, LOW);
    digitalWrite(Linear2, HIGH);
  // Serial.println("am rotating");
  }
  else {
    digitalWrite(Linear1, HIGH);
    digitalWrite(Linear2, HIGH);
  // Serial.println("am stopping");
  }

}

void grab() {

    if (joyCross == 1) {
      servo(1, 105);
    }
    else if (joyR1 || joyTriangle == 1) {
      servo(1, 140);
    }
}
