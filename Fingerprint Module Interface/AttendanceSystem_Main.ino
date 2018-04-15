#include <Key.h>
#include <Keypad.h>
#include<LiquidCrystal.h>
LiquidCrystal lcd(13, 12, 11, 10, 9, 8);
#include <SoftwareSerial.h>
SoftwareSerial fingerPrint(2, 3);
#include <Adafruit_Fingerprint.h>

//Variables for Finger Print Device:
uint8_t id;
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&fingerPrint);

//Variables for Keypad:
const byte ROWS = 4;
const byte COLS = 4;
char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

//Variables for secure login:
#define Password_Length 8
char Data[Password_Length];
char Master[Password_Length] = "123A456";
byte data_count = 0, master_count = 0;
char customKey;
char choice;

void setup()  {
  delay(1000);
  lcd.begin(16, 2);
  lcd.print("    WELCOME TO ");
  lcd.setCursor(0, 1);
  lcd.print("      CAMS     ");
  delay(2000);
  finger.begin(57600);
  Serial.begin(9600);
  lcd.clear();
  lcd.print(" Module Check... ");
  lcd.setCursor(0, 1);
  delay(2000);
  if (finger.verifyPassword())
  {
    Serial.println("Found fingerprint sensor!");
    lcd.clear();
    lcd.print(" Module Found ");
    delay(2000);
  }
  else
  {
    Serial.println("Did not find fingerprint sensor :(");
    lcd.clear();
    lcd.print("Module Not Found");
    lcd.setCursor(0, 1);
    lcd.print("Check Connections");
  }
  authenticate();
}

void authenticate() {
  while (1)
  {
    lcd.setCursor(0, 0);
    lcd.print("Enter Password:");
    readKey(8);
    if (data_count == Password_Length - 1) {
      lcd.clear();
      if (!strcmp(Data, Master)) {
        lcd.print("Correct");
        delay(5000);
        break;
      }
      else {
        lcd.print("Incorrect");
        delay(1000);
      }
      lcd.clear();
      clearData();
    }
    clearData();
  }
  return;
}

void clearData()  {
  while (data_count != 0) {
    Data[data_count--] = 0;
  }
  return;
}

void readKey(int n)  {
  int i;
  for (i = 0; i < n; i++)
  {
    customKey = customKeypad.getKey();
    if (customKey) {
      Data[data_count] = customKey;
      lcd.setCursor(data_count, 1);
      lcd.print(Data[data_count]);
      data_count++;
    }
  }
}

void loop() {
  lcd.setCursor(0, 0);
  lcd.print("A-ENROLL    B-DELETE");
  lcd.setCursor(0, 1);
  lcd.print("C-MATCH    D-HOME");
  checkKeys();
  delay(2000);
}

void checkKeys()  {
  choice = customKeypad.getKey();
  switch (choice)
  {
    case 'A':
      enroll();
      break;
    case 'B':
      delet();
      break;
    case 'C':
      match();
      break;
    default:
      return;
  }
}

void match()  {
  while (1)
  {
    lcd.clear();
    uint8_t t_count = finger.getTemplateCount();
    lcd.print("Enroll Count="); // Displays total enrolled.
    lcd.print(t_count);
    delay(1000);
    lcd.clear();
    for (uint8_t i = 0; i < t_count; i++) // Iterates for all templates during matching phase.
    {
      lcd.clear();
      lcd.print("Place Finger");
      delay(3000);
      int result = getFingerprintIDez();
      if (result <= 0) // This solves the problem of auto-incrementing 'i' when "Finger Not Found!" is encountered.
        t_count += 1; //
      if (result > 0) // ID returned.
      {
        //digitalWrite(openLight, HIGH);
        //digitalWrite(closeLight, LOW);
        lcd.clear();
        lcd.print("ID:");
        lcd.print(result);
        lcd.setCursor(0, 1);
        lcd.print("MARKED");
        delay(3000);
        //digitalWrite(closeLight, HIGH);
        //digitalWrite(openLight, LOW);
        lcd.clear();
        lcd.print("    Next...   ");
        delay(2000);
        lcd.clear();
      }
    }
    lcd.print("Press A to continue, D to go to Home screen.");
    delay(3000);
    lcd.clear();
    choice = customKeypad.getKey();
    switch (choice)
    {
      case 'A':
        continue;
      default:
        return;
    }
  }
  return;
}

void enroll() {
  lcd.clear();
  lcd.print("Enroll Finger...");
  lcd.setCursor(0, 1);
  lcd.print("Set ID:");
  delay(3000);
  readKey(4);
  id = Data;
  clearData();
  getFingerprintEnroll();
  return;
}

void delet()  {
  lcd.clear();
  lcd.print("Delete Finger Print.");
  lcd.setCursor(0, 1);
  readKey(4);
  id = Data;
  clearData();
  deleteFingerprint(id);
  return;
}

uint8_t getFingerprintEnroll()  {
  int p = -1;
  lcd.clear();
  lcd.print("Finger ID:");
  lcd.print(id);
  lcd.setCursor(0, 1);
  lcd.print("Place Finger");
  delay(2000);
  //Capture FP image taken from FPR
  while (p != FINGERPRINT_OK)
  {
    p = finger.getImage();
    switch (p)
    {
      case FINGERPRINT_OK:
        Serial.println("Image taken");
        lcd.clear();
        lcd.print("Image taken");
        break;
      case FINGERPRINT_NOFINGER:
        Serial.println("No Finger");
        lcd.clear();
        lcd.print("No Finger");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        lcd.clear();
        lcd.print("Comm Error");
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error");
        lcd.clear();
        lcd.print("Imaging Error");
        break;
      default:
        Serial.println("Unknown error");
        lcd.clear();
        lcd.print("Unknown Error");
        break;
    }
  }

  //To convert FPI taken above to a readable format.
  p = finger.image2Tz(1);
  switch (p)
  {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      lcd.clear();
      lcd.print("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      lcd.clear();
      lcd.print("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      lcd.clear();
      lcd.print("Comm Error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      lcd.clear();
      lcd.print("Feature Not Found");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      lcd.clear();
      lcd.print("Feature Not Found");
      return p;
    default:
      Serial.println("Unknown error");
      lcd.clear();
      lcd.print("Unknown Error");
      return p;
  }

  Serial.println("Remove Finger");
  lcd.clear();
  lcd.print(" Remove Finger ");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER)
  {
    p = finger.getImage();
  }
  Serial.print("ID ");
  Serial.println(id);

  p = -1;
  Serial.println("Place same finger again");
  lcd.clear();
  lcd.print("  Place Again  ");
  while (p != FINGERPRINT_OK)
  {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Image taken");
        break;
      case FINGERPRINT_NOFINGER:
        Serial.print("No Finger!");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error!");
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error!");
        break;
      default:
        Serial.println("Unknown error!");
        return;
    }
  }


  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // Model Creation for converted image.
  Serial.print("Creating model for #");
  lcd.print("Creating model for #");
  lcd.clear();
  delay(500);
  Serial.println(id);
  lcd.println(id);
  p = finger.createModel();
  if (p == FINGERPRINT_OK)
  {
    Serial.println("Model Created");
    delay(500);
  }
  else if (p == FINGERPRINT_PACKETRECIEVEERR)
  {
    Serial.println("Communication error!");
    lcd.print("COMM ERROR!");
    return p;
  }
  else if (p == FINGERPRINT_ENROLLMISMATCH)
  {
    Serial.println("Fingerprints did not match");
    lcd.print("   MATCH ERROR!    ");
    return p;
  }
  else
  {
    Serial.println("UNKNOWN ERROR!");
    return p;
  }

  // Storing Model
  Serial.print("ID ");
  Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK)
  {
    Serial.println("Stored!");
    lcd.clear();
    lcd.print("Stored!");
    delay(2000);
  }
  else if (p == FINGERPRINT_PACKETRECIEVEERR)
  {
    Serial.println("Communication error");
    return p;
  }
  else if (p == FINGERPRINT_BADLOCATION)
  {
    Serial.println("Could not store in that location");
    return p;
  }
  else if (p == FINGERPRINT_FLASHERR)
  {
    Serial.println("Error writing to flash");
    return p;
  }
  else
  {
    Serial.println("Unknown error");
    return p;
  }
}

int getFingerprintIDez()
{
  uint8_t p = finger.getImage();

  if (p != FINGERPRINT_OK)
    return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)
    return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)
  {
    lcd.clear();
    lcd.print("Finger Not Found");
    lcd.setCursor(0, 1);
    lcd.print("Try Later");
    delay(2000);
    return -1;
  }
  // found a match!
  Serial.print("Found ID #");
  Serial.print(finger.fingerID);
  return finger.fingerID;
}

uint8_t deleteFingerprint(uint8_t id) {
  uint8_t p = -1;
  lcd.clear();
  lcd.print("Please wait");
  p = finger.deleteModel(id);
  if (p == FINGERPRINT_OK)
  {
    Serial.println("Deleted!");
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("ID ");
    lcd.print(id);
    lcd.print(" Deleted!");
    delay(2000);
  }

  else
  {
    Serial.print("Something's Wrong");
    lcd.print("Try Again Later");
    delay(2000);
    return p;
  }
}
