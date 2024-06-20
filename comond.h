#include <Arduino.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "getssram.h"
LiquidCrystal_I2C lcd(0x27, 20, 4);
const float BETA = 3950;
bool isConfirming = false, suhuConfirming = false;

int ON = 0, menuIndex = 0, subMenu = 0;
float celsius, Farenhet, kelvin;
float
    suhu,
    setsuhu,
    tempC,
    tempF,
    tempK;
const int numSafetySwitches = 10, menuDisplayLimit = 5;
const int nextButtonPin = A1,
          okButtonPin = A2,
          thermis = A3,
          kompresor1Pin = 2,
          kompresor2Pin = 3,
          kompresor3Pin = 4,
          fanPin = 5,
          timerPin = 7, test,
          waterpumpPin = 6,
          safetySwitchPins[] = {22, 23, 24, 25, 26, 27, 28, 29, 30, 31};
int a = -1, b = 0, c[6] = {1, 2, 3, 4, 5, 6};
char kompFlags[3] = {0x01, 0x02, 0x04}, kompresorAktif = 0x00;
char menu1[menuDisplayLimit][20] = {"Set Kompresor", "Mode Cool",
                                    "Mode Turbo", "Mode Fan", "TIMER"};

long int waktu, timerSet, waktuakhir = 0, gettempsuhu = 0;
void msg(const char *s1, const char *s2)
{
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.println(s1);
  lcd.setCursor(0, 2);
  lcd.println(s2);
}

/*

  if(ON==HIGH){
    waktu = millis();
    EEPROM.write(1,waktu);
    int y = 0, x = 0;
    lcd.setCursor(0, 0);
    lcd.println("<<===TIMER ON===>>");
    lcd.setCursor(1, 5);
    lcd.println(waktu);
    //
     int check = 0; if ((kompresorAktif & kompFlags[0]) == kompFlags[0]) { digitalWrite(kompresor1Pin, HIGH); } else { digitalWrite(kompresor1Pin, LOW); } while (check < 20) { delay(100); if (checkSafetySwitch() > 0) { safetyShutdown(); return; } check++; } check = 0; if ((kompresorAktif & kompFlags[1]) == kompFlags[1]) { digitalWrite(kompresor2Pin, HIGH); } else { digitalWrite(kompresor2Pin, LOW); } while (check < 20) { delay(100); if (checkSafetySwitch() > 0) { safetyShutdown(); return; } check++; } check = 0; if ((kompresorAktif & kompFlags[2]) == kompFlags[2]) { digitalWrite(kompresor3Pin, HIGH); } else { digitalWrite(kompresor3Pin, LOW); } do { delay(100); if (checkSafetySwitch() > 0) { safetyShutdown(); return; } check++; } while (check < 20); digitalWrite(fanPin, HIGH); digitalWrite(waterpumpPin, HIGH);
    //
    }
    else if(digitalRead(okButtonPin)==HIGH){if (digitalRead(nextButtonPin)==HIGH)
    {

    }
    }
    else{
  lcd.setCursor(0, 0);
  lcd.print("<<=BREAK=>>");

    digitalWrite(fanPin,LOW);
    digitalWrite(waterpumpPin,LOW);
    digitalWrite(kompresor1Pin,LOW);
    digitalWrite(kompresor2Pin,LOW);
    digitalWrite(kompresor3Pin,LOW);
  }

*/