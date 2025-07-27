#ifndef BNO055_SENSOR_H
#define BNO055_SENSOR_H

#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <Arduino.h>

class BNO055Sensor {
private:
  Adafruit_BNO055 bno;
  bool initialized;

public:
  // İlkel constructor (initializer list yok)
  BNO055Sensor() {
    initialized = false;
  }

  // Başlatıcı
  bool begin(uint8_t addr = 0x28) {
    bno = Adafruit_BNO055(55, addr);  // Burada obje yeniden atanıyor
    if (!bno.begin()) {
      Serial.println("BNO055 başlatılamadı!");
      initialized = false;
      return false;
    }
    bno.setExtCrystalUse(true);
    Serial.println("BNO055 başlatıldı.");
    initialized = true;
    return true;
  }

  // Kalibrasyon bilgisi al
  void getCalibration(uint8_t* sys, uint8_t* gyro, uint8_t* accel, uint8_t* mag) {
    if (!initialized) return;
    bno.getCalibration(sys, gyro, accel, mag);
  }

  // Kalibrasyon tamamlanana kadar bekle
  bool waitForFullCalibration(uint16_t timeout_ms = 15000) {
    if (!initialized) return false;
    uint8_t sys, gyro, accel, mag;
    uint32_t start = millis();

    while (millis() - start < timeout_ms) {
      getCalibration(&sys, &gyro, &accel, &mag);
      Serial.print("Kalibrasyon -> SYS:");
      Serial.print(sys);
      Serial.print(" GYRO:");
      Serial.print(gyro);
      Serial.print(" ACC:");
      Serial.print(accel);
      Serial.print(" MAG:");
      Serial.println(mag);

      if (sys == 3 && gyro == 3 && accel == 3 && mag == 3) {
        Serial.println("✅ Tam kalibrasyon tamamlandı!");
        return true;
      }
      delay(500);
    }
    Serial.println("⚠️ Kalibrasyon tamamlanamadı, sensör yine de çalışabilir.");
    return false;
  }

  // Yönelim (Yaw, Pitch, Roll)
  void getOrientation(float* out) {
    if (!initialized) return;
    sensors_event_t data;
    bno.getEvent(&data);
    out[0] = data.orientation.x;  // Yaw
    out[1] = data.orientation.y;  // Pitch
    out[2] = data.orientation.z;  // Roll
  }

  void printOrientation() {
    if (!initialized) return;
    float ori[3];
    getOrientation(ori);
    Serial.print("Yaw: ");
    Serial.print(ori[0]);
    Serial.print(" | Pitch: ");
    Serial.print(ori[1]);
    Serial.print(" | Roll: ");
    Serial.println(ori[2]);
  }

  // Lineer ivme (kalkış tespiti)
  void getLinearAccel(float* out) {
    if (!initialized) return;
    imu::Vector<3> v = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
    out[0] = v.x();
    out[1] = v.y();
    out[2] = v.z();
  }

  bool detectLiftoff(float threshold = 2.0) {
    if (!initialized) return false;
    float lin[3];
    getLinearAccel(lin);
    float totalAccel = sqrt(lin[0] * lin[0] + lin[1] * lin[1] + lin[2] * lin[2]);
    return totalAccel > threshold;
  }
  float getYaw() {
    if (!initialized) return 0.0;
    sensors_event_t data;
    bno.getEvent(&data);
    return data.orientation.x;
  }

  // Yalnızca Pitch döndürür (y ekseni)
  float getPitch() {
    if (!initialized) return 0.0;
    sensors_event_t data;
    bno.getEvent(&data);
    return data.orientation.y;
  }

  // Yalnızca Roll döndürür (z ekseni)
  float getRoll() {
    if (!initialized) return 0.0;
    sensors_event_t data;
    bno.getEvent(&data);
    return data.orientation.z;
  }
};

#endif
