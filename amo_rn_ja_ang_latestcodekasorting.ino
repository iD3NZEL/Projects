// PAPER SORTING PROCESS (Arduino 1) - Sending Tray Selection and Fold Count
#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

const int trayMotors[3][2] = {{10, 11}, {12, 13}, {A1, A0}};
const int trayEnablePins[3] = {5, 6, 3}; // ENA and ENB PWM pins for speed control
const int traySensors[3] = {A2, A3, 2};
bool traySelection[3] = {false, false, false};
int trayOrder[3];
int trayOrderCount = 0;
int foldCount = 0;
bool isTraySelectionComplete = false;
bool isFoldingCountSet = false;
bool isDispensing = false;

// Function Prototypes (fixes 'not declared in this scope' error)
void stopDispensing();
void goBack();
void dispensePaper();

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.print("Select Trays:");

  for (int i = 0; i < 3; i++) {
    pinMode(trayMotors[i][0], OUTPUT);
    pinMode(trayMotors[i][1], OUTPUT);
    pinMode(traySensors[i], INPUT_PULLUP); // Ensure proper sensor reading
    pinMode(trayEnablePins[i], OUTPUT);
    analogWrite(trayEnablePins[i], 255);
  }
}

void loop() {
  char key = keypad.getKey();
  if (key) {
    if (key == '*') {
      stopDispensing();
    } else if (key == 'A') {
      goBack();
    }
  }

  if (!isTraySelectionComplete) {
    if (key) {
      if (key >= '1' && key <= '3') {
        int trayIndex = key - '1';
        if (!traySelection[trayIndex]) {
          traySelection[trayIndex] = true;
          trayOrder[trayOrderCount++] = trayIndex;
        }
        lcd.clear();
        lcd.print("Tray ");
        lcd.print(trayIndex + 1);
        lcd.print(" Selected");
      } else if (key == '#') {
        isTraySelectionComplete = true;
        lcd.clear();
        lcd.print("Enter Count:");
      }
    }
  } else if (!isFoldingCountSet) {
    if (key) {
      if (key >= '0' && key <= '9') {
        foldCount = foldCount * 10 + (key - '0');
        lcd.setCursor(0, 1);
        lcd.print(foldCount);
      } else if (key == '#') {
        isFoldingCountSet = true;
        isDispensing = true;
        lcd.clear();
        lcd.print("Dispensing...");
        Serial.print("TRAYSELECTION");
        for (int i = 0; i < 3; i++) {
          Serial.print(traySelection[i] ? "1" : "0");
        }
        Serial.println();
        Serial.print("FOLDCOUNT");
        Serial.println(foldCount);
      }
    }
  } else if (isDispensing) {
    dispensePaper();
  }
}

// Fixed dispensePaper function
void dispensePaper() {
  for (int i = 0; i < trayOrderCount; i++) {
    int currentTray = trayOrder[i];

    Serial.print("Tray ");
    Serial.print(currentTray + 1);
    Serial.print(" Sensor State: ");
    Serial.println(digitalRead(traySensors[currentTray])); // Debugging output

    if (digitalRead(traySensors[currentTray]) == LOW) {
      for (int j = 0; j < foldCount; j++) {
        digitalWrite(trayMotors[currentTray][0], LOW);
        digitalWrite(trayMotors[currentTray][1], HIGH);
        analogWrite(trayEnablePins[currentTray], 255);

        unsigned long startTime = millis();
        while (millis() - startTime < 1000) {
          char key = keypad.getKey();
          if (key == '*') {
            stopDispensing();
            return;
          }
        }

        digitalWrite(trayMotors[currentTray][0], LOW);
        digitalWrite(trayMotors[currentTray][1], LOW);
        analogWrite(trayEnablePins[currentTray], 0);

        startTime = millis();
        while (millis() - startTime < 500) {
          char key = keypad.getKey();
          if (key == '*') {
            stopDispensing();
            return;
          }
        }
      }
    } else {
      Serial.print("No paper detected in Tray ");
      Serial.println(currentTray + 1);
    }
  }

  isDispensing = false;
  lcd.clear();
  lcd.print("Done Dispensing");
}

void stopDispensing() {
  isDispensing = false;
  lcd.clear();
  lcd.print("Dispensing Stopped");
  for (int i = 0; i < 3; i++) {
    digitalWrite(trayMotors[i][0], LOW);
    digitalWrite(trayMotors[i][1], LOW);
    analogWrite(trayEnablePins[i], 0);
  }
}

void goBack() {
  isTraySelectionComplete = false;
  isFoldingCountSet = false;
  trayOrderCount = 0;
  foldCount = 0;
  for (int i = 0; i < 3; i++) {
    traySelection[i] = false;
  }
  lcd.clear();
  lcd.print("Select Trays:");
}
