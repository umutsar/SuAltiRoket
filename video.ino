#include <Servo.h>
#include "BNO055.h"


// ----------- SERVO NESNELERİ -----------
Servo servo1;    // ESC (itici motor)
Servo servo2;    // Pitch yukarı
Servo servo3;    // Yaw sağ
Servo servo4;    // Pitch aşağı
Servo servo5;    // Yaw sol
Servo servoLED;  // PWM LED çıkışı

// ----------- PWM SABİTLERİ ------------
const int MOTOR_STOP_PWM = 1490;
const int MOTOR_MAX_PWM = 1100;
const int MOTOR_DONUS_PWM = 1250;

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

  while (1) {
    bno055.printOrientation();
    delay(100);
  }

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
  // gorev2();
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
  for (int pwm = baslangicPWM; pwm >= hedefPWM; pwm -= 2) {
    servo1.writeMicroseconds(pwm);
    delay(1);
  }
}

void motorYavasAzalt(int baslangicPWM, int hedefPWM) {
  for (int pwm = baslangicPWM; pwm <= hedefPWM; pwm += 2) {
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


// ------------- GÖREV 1 ------------------
void gorev1() {
  while (1) {
    if (ikinciDonus) {
      Serial.println("Gorev 1 basladi.");
      ikinciDonus = 0;
    }

    currentTime = millis();
    if (currentTime - previousTime < 12000) {
      tumServolariNotrle();
      if (ilkDonus) {
        motorYavasArtir(MOTOR_STOP_PWM, MOTOR_MAX_PWM);
        ilkDonus = 0;
      }
      if (bno055.getYaw() < 50 && bno055.getYaw() > 5) {
        solaDondur();
        Serial.print(currentTime);
        Serial.println("  Sola dönüyor");
      } else if (bno055.getYaw() > 330 && bno055.getYaw() < 355) {
        sagaDondur();
        Serial.print(currentTime);
        Serial.println("  Sağa dönüyor");
      }
      delay(50);
    }



    // bekleme
    if (currentTime - previousTime > 12000 && currentTime - previousTime < 13000) {
      tumServolariNotrle();
      motorYavasAzalt(MOTOR_MAX_PWM, MOTOR_DONUS_PWM);
    }



    if (currentTime - previousTime > 13000 && currentTime - previousTime < 18000) {
      tumServolariNotrle();
      if (bno055.getYaw() < 85) {
        sagaDondur();
        Serial.print(currentTime);
        Serial.println("  Saga dönüyor");

      } else if (bno055.getYaw() > 95) {
        solaDondur();
        Serial.print(currentTime);
        Serial.println("  Sola dönüyor");
      } else {
        motorYavasArtir(MOTOR_DONUS_PWM, MOTOR_MAX_PWM);
      }
      delay(50);
    }


    // bekleme
    if (currentTime - previousTime > 18000 && currentTime - previousTime < 19000) {
      tumServolariNotrle();
      motorYavasAzalt(MOTOR_MAX_PWM, MOTOR_DONUS_PWM);
    }

    if (currentTime - previousTime > 19000 && currentTime - previousTime < 31000) {
      tumServolariNotrle();
      if (bno055.getYaw() < 175) {
        sagaDondur();
        Serial.print(currentTime);
        Serial.println("  Saga dönüyor");

      } else if (bno055.getYaw() > 185) {
        solaDondur();
        Serial.print(currentTime);
        Serial.println("  Sola dönüyor");
      } else {
        motorYavasArtir(MOTOR_DONUS_PWM, MOTOR_MAX_PWM);
      }
      delay(50);
    }

    // bekleme
    if (currentTime - previousTime > 31000 && currentTime - previousTime < 32000) {
      tumServolariNotrle();
      motorYavasAzalt(MOTOR_MAX_PWM, MOTOR_DONUS_PWM);
    }



    if (currentTime - previousTime > 32000 && currentTime - previousTime < 37000) {
      tumServolariNotrle();
      if (bno055.getYaw() < 265) {
        sagaDondur();
        Serial.print(currentTime);
        Serial.println("  Saga dönüyor");

      } else if (bno055.getYaw() > 275) {
        solaDondur();
        Serial.print(currentTime);
        Serial.println("  Sola dönüyor");
      } else {
        motorYavasArtir(MOTOR_DONUS_PWM, MOTOR_MAX_PWM);
      }
      delay(50);
    }

    // bekleme
    if (currentTime - previousTime > 37000 && currentTime - previousTime < 38000) {
      tumServolariNotrle();
      motorYavasAzalt(MOTOR_MAX_PWM, MOTOR_DONUS_PWM);
    }



    if (currentTime - previousTime > 38000 && currentTime - previousTime < 50000) {
      tumServolariNotrle();
      if (bno055.getYaw() < 50 && bno055.getYaw() > 5) {
        sagaDondur();
        Serial.print(currentTime);
        Serial.println("  Saga dönüyor");
      } else if (bno055.getYaw() > 330 && bno055.getYaw() < 355) {
        solaDondur();
        Serial.print(currentTime);
        Serial.println("  Sola dönüyor");
      } else {
        motorYavasArtir(MOTOR_DONUS_PWM, MOTOR_MAX_PWM);
      }
      delay(50);
    }













    if (currentTime - previousTime > 38000 && currentTime - previousTime < 65000) {
      tumServolariNotrle();
      if (bno055.getPitch() > -35) {
        yukariDondur();
        Serial.print(currentTime);
        Serial.println("  Yukarı dönüyor");

      } else if (bno055.getPitch() < -45) {
        asagiDondur();
        Serial.print(currentTime);
        Serial.println("  Asağı dönüyor");
      }
      delay(50);
    }

    if (currentTime - previousTime > 36000 && currentTime - previousTime < 36100) {
      tumServolariNotrle();
      motorYavasAzalt(MOTOR_MAX_PWM, MOTOR_STOP_PWM);
      Serial.print(currentTime);
      Serial.println("  Bitti!");
      delay(100);
    }
  }
}

// ------------- GÖREV 2 ------------------
void gorev2() {
  tumServolariNotrle();
  motorYavasArtir(MOTOR_STOP_PWM, MOTOR_MAX_PWM);
  delay(7000);  // düz git 7sn

  // 1. sola dön
  motorYavasAzalt(MOTOR_MAX_PWM, MOTOR_DONUS_PWM);
  servo3.write(45);
  servo5.write(135);
  delay(1500);
  tumServolariNotrle();
  motorYavasArtir(MOTOR_DONUS_PWM, MOTOR_MAX_PWM);
  delay(5000);  // sola gittikten sonra düz 5sn

  // 2. tekrar sağa dön
  motorYavasAzalt(MOTOR_MAX_PWM, MOTOR_DONUS_PWM);
  servo3.write(135);
  servo5.write(45);
  delay(1500);
  tumServolariNotrle();
  motorYavasArtir(MOTOR_DONUS_PWM, MOTOR_MAX_PWM);
  delay(5000);  // tekrar düz 5sn

  // Aşağı dal
  motorYavasAzalt(MOTOR_MAX_PWM, MOTOR_DONUS_PWM);
  servo2.write(135);  // aşağı pitch
  servo4.write(45);
  delay(2000);

  // Yukarı çık (yunuslama)
  servo2.write(45);
  servo4.write(135);
  motorYavasArtir(MOTOR_DONUS_PWM, MOTOR_MAX_PWM);
  delay(3000);

  // Yüzeyde kal 4 sn – hafif motor sinyaliyle
  servo1.writeMicroseconds(1350);
  ledPWMYak();
  delay(YUZEYDE_BEKLEME);
  ledPWMSon();

  // Durdur
  motorYavasAzalt(1350, MOTOR_STOP_PWM);
  tumServolariNotrle();
}