#include "comond.h"
void setup()
{
  Serial.begin(9600);
  lcd.backlight();
  lcd.begin(20, 4);
  pinMode(nextButtonPin, INPUT),
      pinMode(okButtonPin, INPUT),
      pinMode(kompresor1Pin, OUTPUT),
      pinMode(kompresor2Pin, OUTPUT),
      pinMode(kompresor3Pin, OUTPUT),
      pinMode(fanPin, OUTPUT),
      pinMode(waterpumpPin, OUTPUT);
  getMemo(c[0], celsius),
      getMemo(c[1], Farenhet),
      getMemo(c[2], kelvin),
      getMemo(c[3], kompresorAktif);
      getMemo(c[4], setsuhu);

  for (int i = 0; i < numSafetySwitches; ++i)
  {
    pinMode(safetySwitchPins[i], INPUT);
  }
  displayMenu();
}
void loop()
{
  waktu = millis();

  if (checkSafetySwitch() > 0)
  {
    safetyShutdown();
    return;
  }
  else if (waktu - gettempsuhu > 5000)
  {

    int analogValue = analogRead(thermis);
    celsius = 1 / (log(1 / (1023. / analogValue - 1)) / BETA + 1.0 / 298.15) - 273.15;
    Farenhet = (celsius * 9 / 5) + 32;
    kelvin = celsius + 273.15;
    putMemo(c[0], celsius),
        putMemo(c[2], kelvin),
        putMemo(c[1], Farenhet);
    Serial.print("value my temp is = "),
        Serial.print(celsius),
        Serial.println("C"),
        Serial.print("value my temp is = "),
        Serial.print(kelvin),
        Serial.println("K"),
        Serial.print("value my temp is = "),
        Serial.print(Farenhet),
        Serial.println("K");
    gettempsuhu = waktu;
    if (waktu - waktuakhir > (5000 * 2))
    {
      showsuhu(celsius, Farenhet, kelvin);
    }
    else
    {
      if (!suhu < setsuhu)
      {
        suhuConfirming = false;
      }
      else
      {
        suhuConfirming = true;
      }
    }
  }
  delay(100);
  int nextButtonState = digitalRead(nextButtonPin);
  int okButtonState = digitalRead(okButtonPin);

  if (nextButtonState)
  {
    a = -1;
    waktuakhir = waktu;
    lcd.backlight();
    menuIndex = (menuIndex + 1) % menuDisplayLimit;
    displayMenu();
    return;
  }

  else if (okButtonState)
  {
    a = -1;
    waktuakhir = waktu;
    lcd.backlight();
    if (isConfirming)
    {
      handleConfirmation();
      return;
    }
    if (menuIndex == 0 && subMenu == 0)
    {
      subMenu = 1;
      menuIndex = 0;
      displayMenu();
      return;
    }
    if (subMenu == 0)
    {
      displayConfirmation();
      return;
    }
    if (subMenu == 1)
    {
      if (menuIndex == 3)
      {
        subMenu = 0;
        menuIndex = 0;
        displayMenu();
        return;
      }
      kompresorAktif ^= kompFlags[menuIndex];
      displayMenu();
      return;
    }
  }
}

void displayMenu()
{
  lcd.clear();
  switch (subMenu)
  {
  case 0:
    for (int i = 0; i < menuDisplayLimit; i++)
    {
      lcd.setCursor(0, i);
      String menuSelect = (menuIndex == i) ? "> " : "  ";
      menuSelect.concat(menu1[i]);
      lcd.print(menuSelect);
    }
    break;

  case 1:
    for (int i = 0; i < 3; i++)
    {
      lcd.setCursor(0, i);
      String kompSelect = (menuIndex == i) ? "> " : "  ";
      kompSelect.concat("Kompresor "),
          kompSelect.concat(i + 1),
          kompSelect.concat(": "),
          kompSelect.concat((kompresorAktif & kompFlags[i]) ? "ON" : "OFF");
      lcd.print(kompSelect),
          lcd.setCursor(0, 3);
    }
    if (menuIndex == 3)
    {
      lcd.print("> Kembali");
      putMemo(c[3], kompresorAktif);
    }
    else
    {
      lcd.print("  Kembali");
    }
    break;
  }
}

void displayConfirmation()
{
  isConfirming = true;
  msg("Konfirmasi", "OK untuk konfirmasi");
}

void handleConfirmation()
{
  switch (menuIndex)
  {
  case 1:
    activateCoolMode();
    break;
  case 2:
    activatedModeTurbo();
    break;
  case 3:
    activatedModeFan();
    break;
  case 4:
    timerMode();
    break;
  }
  isConfirming = false;
  displayMenu();
}

void activateCoolMode()
{
  lcd.clear();

  // lcd.setCursor(0, 1);
  // lcd.print("Proses...");
  if (suhuConfirming)
  {
    int kompresorPins[3] = {kompresor1Pin, kompresor2Pin, kompresor3Pin};
    for (int i = 0; i < 3; i++)
    {
      digitalWrite(kompresorPins[i], (kompresorAktif & kompFlags[i]) ? HIGH : LOW);
      for (int check = 0; check < 20; check++)
      {
        delay(100);
        if (checkSafetySwitch() > 0)
        {
          safetyShutdown();
          return;
        }
      }
    }
    digitalWrite(fanPin, HIGH);
    digitalWrite(waterpumpPin, HIGH);
  }
}

void safetyShutdown()
{
  int kompresorPins[3] = {kompresor1Pin, kompresor2Pin, kompresor3Pin};
  for (int i = 0; i < 3; i++)
  {
    digitalWrite(kompresorPins[i], LOW);
  }
  digitalWrite(waterpumpPin, LOW);
  digitalWrite(fanPin, LOW);
  delay(1000);
}

int checkSafetySwitch()
{
  for (int i = 0; i < numSafetySwitches; ++i)
  {
    if (digitalRead(safetySwitchPins[i]) == HIGH)
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Safety Switch Error C ");
      lcd.print(i + 1);
      return i + 1;
    }
  }
  return 0;
}

void activatedModeTurbo()
{
  lcd.clear();
  if (suhuConfirming)
  {
    int kompresorPins[3] = {kompresor1Pin, kompresor2Pin, kompresor3Pin};
    for (int i = 0; i < 3; i++)
    {
      digitalWrite(kompresorPins[i], (kompresorAktif & kompFlags[i]) ? HIGH : LOW);
      for (int check = 0; check < (i == 1 ? 50 : 30); check++)
      {
        delay(100);
        if (checkSafetySwitch() > 0)
        {
          safetyShutdown();
          return;
        }
      }
    }
    digitalWrite(fanPin, HIGH);
    digitalWrite(waterpumpPin, HIGH);
  }
}

void activatedModeFan()
{
  lcd.clear();
  if (suhuConfirming)
  {
    int kompresorPins[3] = {kompresor1Pin, kompresor2Pin, kompresor3Pin};
    for (int i = 0; i < 3; i++)
    {
      digitalWrite(kompresorPins[i], LOW);
    }
    digitalWrite(fanPin, HIGH);
    digitalWrite(waterpumpPin, LOW);
  }
}

void timerMode()
{
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("push next to handle");
  lcd.setCursor(0, 1);
  lcd.print(" TIMER ON");
  lcd.setCursor(0, 2);
  lcd.print(" TIMER OFF");
  int xy = 0, x[] = {1, 2, 3}, y[] = {1, 2, 3}, i;
  do
  {
    for (i = 0; i < xy; i++)
    {
      lcd.setCursor(0, xy);
      lcd.print(">");
    }
    if (digitalRead(nextButtonPin) == HIGH)
    {
      xy++;
      if (xy > 2)
      {
        xy = 1;
      }
      lcd.setCursor(0, i);
      lcd.print(" ");
      delay(100);
    }
    else if (digitalRead(okButtonPin) == HIGH)
    {
      if (i == 2)
      {
        isConfirming = false;
        break;
      }
    }
    else if (digitalRead(timerPin) == HIGH)
    {
      if (suhuConfirming)
      {
        int check = 0;

        if ((kompresorAktif & kompFlags[0]) == kompFlags[0])
        {
          digitalWrite(kompresor1Pin, HIGH);
        }
        else
        {
          digitalWrite(kompresor1Pin, LOW);
        }
        while (check < 20)
        {
          delay(100);
          if (checkSafetySwitch() > 0)
          {
            safetyShutdown();
            return;
          }
          check++;
        }
        check = 0;

        if ((kompresorAktif & kompFlags[1]) == kompFlags[1])
        {
          digitalWrite(kompresor2Pin, HIGH);
        }
        else
        {
          digitalWrite(kompresor2Pin, LOW);
        }
        while (check < 20)
        {
          delay(100);
          if (checkSafetySwitch() > 0)
          {
            safetyShutdown();
            return;
          }
          check++;
        }
        check = 0;

        if ((kompresorAktif & kompFlags[2]) == kompFlags[2])
        {
          digitalWrite(kompresor3Pin, HIGH);
        }
        else
        {
          digitalWrite(kompresor3Pin, LOW);
        }
        while (check < 20)
        {
          delay(100);
          if (checkSafetySwitch() > 0)
          {
            safetyShutdown();
            return;
          }
          check++;
        }

        digitalWrite(fanPin, HIGH);
        digitalWrite(waterpumpPin, HIGH);
      }
    }
  } while (isConfirming);
}

float showsuhu(float S1, float S2, float S3)
{

  if (digitalRead(nextButtonPin) == HIGH)
  {
    if (digitalRead(okButtonPin) == HIGH)
    {
      a += 1;
    }
  }
  if (a == -1)
  {
    lcd.clear();
    a = 0;
  }
  switch (a)
  {
  case 0:
    lcd.setCursor(5, 1);
    lcd.print(" SUHU ");
    lcd.print(S1);
    lcd.print(" C ");
    break;
  case 1:
    lcd.setCursor(5, 1);
    lcd.print(" SUHU ");
    lcd.print(S2);
    lcd.print(" F ");
    break;
  case 2:
    lcd.setCursor(5, 1);
    lcd.print(" SUHU ");
    lcd.print(S3);
    lcd.print(" K ");
    break;
  }
}

void getsuhu()
{
}