#ifndef CAMERA_H
#define CAMERA_H

#include <Arduino.h>
#include <ArduCAM.h>

class Camera {
private:
  const int CS = 17;
  const int CAM_POWER_ON = 10;
  int WIFI_TYPE = 0; // 0:Station  1:AP
public:
  ArduCAM myCAM;
  Camera();
  void setupCamera();
};

extern Camera cam;

#endif
