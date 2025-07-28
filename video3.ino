#include <Servo.h>
#include "BNO055.h"
// #include "BME280.h"

// ----------- SERVO NESNELERİ -----------
Servo servo1;    // ESC (itici motor)
Servo servo2;    // Pitch yukarı
Servo servo3;    // Yaw sağ
Servo servo4;    // Pitch aşağı
Servo servo5;    // Yaw sol
Servo servoLED;  // PWM LED çıkışı

// ----------- PWM SABİTLERİ ------------
const int MOTOR_STOP_PWM = 1200;
const int MOTOR_MAX_PWM = 1400;
const int MOTOR_DONUS_PWM = 1300;

const int LED_PWM_ACIK = 1900;
const int LED_PWM_KAPALI = 1100;

// ----------- PINLER -------------------
const int ledPin = 11;  // PWM LED pin

// ----------- ZAMANLAR -----------------
const int SISTEM_GECIKME = 5000;
const int GOREV_ARASI_BEKLEME = 60000;  // 1 dk
const int YUZEYDE_BEKLEME = 4000;

BNO055Sensor bno055;  // BNO için OOP sınıfımız
uint32_t currentTime = millis();
uint32_t previousTime = millis();

void setup() {
  Serial.begin(9600);
  if (!bno055.begin(0x28)) {
    Serial.println("BNO sensörü başlatılamadı!");
  }
  // bme280.begin();

  // while (1) {
  //   bno055.printOrientation();
  //   // bme280.printPressure();
  //   delay(100);
  // }

  // Servo pinleri bağlanıyor
  servo1.attach(3);
  servo2.attach(5);
  servo3.attach(6);
  servo4.attach(9);
  servo5.attach(10);
  servoLED.attach(ledPin);

  // Başlangıç durumu
  servo1.writeMicroseconds(MOTOR_STOP_PWM);
  tumServolariNotrle();
  ledPWMSon();
  // delay(SISTEM_GECIKME);

  // GÖREV 1
  currentTime = millis();
  previousTime = millis();
  gorev1();
  delay(GOREV_ARASI_BEKLEME);

  // GÖREV 2
  gorev2();
}

void loop() {

  // Görevler sadece 1 defa setup’ta çalışır
}

// ---------- LED FONKSİYONLARI ----------
void ledPWMYak() {
  servoLED.writeMicroseconds(LED_PWM_ACIK);
}

void ledPWMSon() {
  servoLED.writeMicroseconds(LED_PWM_KAPALI);
}

// --------- SERVOLARI NÖTRLE -----------
void tumServolariNotrle() {
  servo2.write(90);
  servo3.write(90);
  servo4.write(90);
  servo5.write(90);
}

// --------- MOTOR KONTROLLERİ ----------
void motorYavasArtir(int baslangicPWM, int hedefPWM) {
  for (int pwm = baslangicPWM; pwm <= hedefPWM; pwm += 2) {
    servo1.writeMicroseconds(pwm);
    delay(1);
  }
}

void motorYavasAzalt(int baslangicPWM, int hedefPWM) {
  for (int pwm = baslangicPWM; pwm >= hedefPWM; pwm -= 2) {
    servo1.writeMicroseconds(pwm);
    delay(1);
  }
}

bool ilkDonus = 1;
bool ikinciDonus = 1;

void sagaDondur() {
  servo3.write(135);
  servo5.write(45);
}

void solaDondur() {
  servo3.write(45);
  servo5.write(135);
}

void yukariDondur() {
  servo2.write(45);
  servo4.write(135);
}

void asagiDondur() {
  servo2.write(135);
  servo4.write(45);
}

float pressure = 0;
// ------------- GÖREV 1 ------------------
bool yukariKontrolFlag = 1;
void gorev1() {
  previousTime = millis();  // Görev başlama zamanı
  Serial.println("Görev 1 başlıyor...");

  while (1) {
    // pressure = bme280.readPressure();

    currentTime = millis();
    // if (yukariKontrolFlag) {
    //   if (pressure > 940) {
    //     yukariDondur();
    //   }
    //   else if(pressure < 920) {
    //     asagiDondur();
    //   }
    // }
    // **60 saniye geçtiyse görevi bitir**
    if ((currentTime - previousTime) >= 63000) {
      tumServolariNotrle();
      motorYavasAzalt(MOTOR_MAX_PWM, MOTOR_STOP_PWM);
      Serial.println("Görev 1 tamamlandı, 60 sn geçti.");
      break;
    }

    // Görev başladı mesajı
    if (ikinciDonus) {
      Serial.println("Görev 1 başladı.");
      ikinciDonus = 0;
    }

    // === 0 - 12 saniye arası ===
    if (currentTime - previousTime < 12000) {
      tumServolariNotrle();
      if (ilkDonus) {
        motorYavasArtir(MOTOR_STOP_PWM, MOTOR_MAX_PWM);
        ilkDonus = 0;
      }
      if (bno055.getYaw() < 50 && bno055.getYaw() > 5) {
        solaDondur();
        Serial.println(currentTime);
        Serial.println("Sola dönüyor");
      } else if (bno055.getYaw() > 330 && bno055.getYaw() < 355) {
        sagaDondur();
        Serial.println(currentTime);
        Serial.println("Sağa dönüyor");
      }
      delay(50);
    }

    // === 12 - 13 sn bekleme ===
    if (currentTime - previousTime > 12000 && currentTime - previousTime < 13000) {
      tumServolariNotrle();
      motorYavasAzalt(MOTOR_MAX_PWM, MOTOR_DONUS_PWM);
    }

    // === 13 - 18 sn ===
    if (currentTime - previousTime > 13000 && currentTime - previousTime < 18000) {
      tumServolariNotrle();
      if (bno055.getYaw() < 85) {
        sagaDondur();
      } else if (bno055.getYaw() > 95) {
        solaDondur();
        Serial.println(currentTime);
        Serial.println("Sola dönüyor");
      } else {
        motorYavasArtir(MOTOR_DONUS_PWM, MOTOR_MAX_PWM);
      }
      delay(50);
    }

    // === 18 - 19 sn bekleme ===
    if (currentTime - previousTime > 18000 && currentTime - previousTime < 19000) {
      tumServolariNotrle();
      motorYavasAzalt(MOTOR_MAX_PWM, MOTOR_DONUS_PWM);
    }

    // === 19 - 31 sn ===
    if (currentTime - previousTime > 19000 && currentTime - previousTime < 31000) {
      tumServolariNotrle();
      if (bno055.getYaw() < 175) {
        sagaDondur();
      } else if (bno055.getYaw() > 185) {
        solaDondur();
        Serial.println(currentTime);
        Serial.println("Sola dönüyor");
      } else {
        motorYavasArtir(MOTOR_DONUS_PWM, MOTOR_MAX_PWM);
      }
      delay(50);
    }

    // === 31 - 32 sn bekleme ===
    if (currentTime - previousTime > 31000 && currentTime - previousTime < 32000) {
      tumServolariNotrle();
      motorYavasAzalt(MOTOR_MAX_PWM, MOTOR_DONUS_PWM);
    }

    // === 32 - 37 sn ===
    if (currentTime - previousTime > 32000 && currentTime - previousTime < 37000) {
      tumServolariNotrle();
      if (bno055.getYaw() < 265) {
        sagaDondur();
      } else if (bno055.getYaw() > 275) {
        solaDondur();
        Serial.println(currentTime);
        Serial.println("Sola dönüyor");
      } else {
        motorYavasArtir(MOTOR_DONUS_PWM, MOTOR_MAX_PWM);
      }
      delay(50);
    }

    // === 37 - 38 sn bekleme ===
    if (currentTime - previousTime > 37000 && currentTime - previousTime < 38000) {
      tumServolariNotrle();
      motorYavasAzalt(MOTOR_MAX_PWM, MOTOR_DONUS_PWM);
    }

    // === 38 - 50 sn ===
    if (currentTime - previousTime > 38000 && currentTime - previousTime < 50000) {
      tumServolariNotrle();
      if (bno055.getYaw() < 50 && bno055.getYaw() > 5) {
        sagaDondur();
      } else if (bno055.getYaw() > 330 && bno055.getYaw() < 355) {
        solaDondur();
      } else {
        motorYavasArtir(MOTOR_DONUS_PWM, MOTOR_MAX_PWM);
      }
      delay(50);
    }

    // === 38 - 65 sn pitch kontrol ===
    if (currentTime - previousTime > 38000 && currentTime - previousTime < 65000) {
      tumServolariNotrle();
      if (bno055.getPitch() > -35) {
        yukariDondur();
      } else if (bno055.getPitch() < -45) {
        asagiDondur();
      }
      delay(50);
    }

    // === 36 - 36.1 sn özel bitiş mesajı ===
    if (currentTime - previousTime > 36000 && currentTime - previousTime < 36100) {
      tumServolariNotrle();
      motorYavasAzalt(MOTOR_MAX_PWM, MOTOR_STOP_PWM);
      Serial.println("Bitti!");
      delay(100);
      break;
    }
  }
}

void gorev2() {
  tumServolariNotrle();
  motorYavasArtir(MOTOR_STOP_PWM, MOTOR_MAX_PWM);
  previousTime = millis();

  while (1) {
    currentTime = millis();

    // 0–5000 ms: Yukarı çık
    if (currentTime - previousTime < 3000) {
      if (bno055.getPitch() > -35) {
        yukariDondur();
        Serial.println("Yukarı dönüyor");
      } else if (bno055.getPitch() < -45) {
        asagiDondur();
        Serial.println("Aşağı dönüyor (gereksiz ama güvenlik)");
      } else {
        tumServolariNotrle();
        Serial.println("Düz konumda");
      }
    }

    // 5000–10000 ms: Düz git
    else if (currentTime - previousTime > 3000 && currentTime - previousTime < 6000) {
      tumServolariNotrle();
      Serial.println("Düz gidiyor");
    }

    // 10000–15000 ms: Aşağı in
    else if (currentTime - previousTime > 6000 && currentTime - previousTime < 10000) {
      if (bno055.getPitch() > 0) {
        yukariDondur();
        Serial.println("Yukarı dönüyor (gereksiz ama güvenlik)");
      } else if (bno055.getPitch() < -10) {
        asagiDondur();
        Serial.println("Aşağı dönüyor");
      } else {
        tumServolariNotrle();
        Serial.println("Düz konumda");
      }
    }

    // 15000–25000 ms: Yukarı çık (10 saniye)
    else if (currentTime - previousTime > 10000 && currentTime - previousTime < 14000) {
      if (bno055.getPitch() > -35) {
        yukariDondur();
        Serial.println("Yukarı dönüyor");
      } else if (bno055.getPitch() < -45) {
        asagiDondur();
        Serial.println("Aşağı dönüyor (gereksiz ama güvenlik)");
      } else {
        tumServolariNotrle();
        Serial.println("Düz konumda");
      }
    }


    else if (currentTime - previousTime > 14000 && currentTime - previousTime < 18000) {
      ledPWMYak();
      if (bno055.getPitch() > -35) {
        yukariDondur();
        Serial.println("Yukarı dönüyor");
      } else if (bno055.getPitch() < -45) {
        asagiDondur();
        Serial.println("Aşağı dönüyor (gereksiz ama güvenlik)");
      } else {
        tumServolariNotrle();
        Serial.println("Düz konumda");
      }
    }



    // Görev bitti
    else if (currentTime - previousTime > 18000) {
      ledPWMSon();
      tumServolariNotrle();
      motorYavasAzalt(MOTOR_MAX_PWM, MOTOR_STOP_PWM);
      Serial.println("Görev 3 bitti.");
      delay(100);
      break;
    }

    delay(50);  // Sensör okuma rahatlasın diye kısa bir bekleme
  }
}
