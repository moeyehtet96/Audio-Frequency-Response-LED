#include <LiquidCrystal.h>

// MSGEQ7 Pins
#define MSGEQ7_OUTPUT A0
#define MSGEQ7_STROBE 2
#define MSGEQ7_RESET  4

// LED Pins
#define LED_RED   3
#define LED_GREEN 6
#define LED_BLUE  11

// LCD Pins
#define LCD_RS    5
#define LCD_EN    7
#define LCD_D4    8
#define LCD_D5    9
#define LCD_D6    10
#define LCD_D7    12

// MSGEQ7 variables and initializations
int freqVal[7];
int band;
int filterVal = 80;

// LCD Setup
const int ROWS = 2;
const int COLS = 16;
LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

// Equalizer Level Characters
byte level0[8] = {0B00000, 0B00000, 0B00000, 0B00000, 0B00000, 0B00000, 0B00000, 0B11111};
byte level1[8] = {0B00000, 0B00000, 0B00000, 0B00000, 0B00000, 0B00000, 0B11111, 0B11111};
byte level2[8] = {0B00000, 0B00000, 0B00000, 0B00000, 0B00000, 0B11111, 0B11111, 0B11111};
byte level3[8] = {0B00000, 0B00000, 0B00000, 0B00000, 0B11111, 0B11111, 0B11111, 0B11111};
byte level4[8] = {0B00000, 0B00000, 0B00000, 0B11111, 0B11111, 0B11111, 0B11111, 0B11111};
byte level5[8] = {0B00000, 0B00000, 0B11111, 0B11111, 0B11111, 0B11111, 0B11111, 0B11111};
byte level6[8] = {0B00000, 0B11111, 0B11111, 0B11111, 0B11111, 0B11111, 0B11111, 0B11111};
byte level7[8] = {0B11111, 0B11111, 0B11111, 0B11111, 0B11111, 0B11111, 0B11111, 0B11111};

void setup() {
  Serial.begin(9600);
  // Set up I/O Modes for MSGEQ7
  pinMode(MSGEQ7_OUTPUT, INPUT);
  pinMode(MSGEQ7_STROBE, OUTPUT);
  pinMode(MSGEQ7_RESET, OUTPUT);
  analogReference(DEFAULT);

  // Set MSGEQ7 Reset to LOW and Strobe to HIGH
  digitalWrite(MSGEQ7_RESET, LOW);
  digitalWrite(MSGEQ7_STROBE, HIGH);

  // Set up I/O Modes for LEDs
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);

  // Start the LCD
  lcd.begin(COLS, ROWS);
  lcd.clear();

  // Create Custom Characters for LCD
  lcd.createChar(0,level0);
  lcd.createChar(1,level1);
  lcd.createChar(2,level2);
  lcd.createChar(3,level3);
  lcd.createChar(4,level4);
  lcd.createChar(5,level5);
  lcd.createChar(6,level6);
  lcd.createChar(7,level7);

  lcd.setCursor(3,1);
  lcd.print("EqualizerZ");
}

void sendToLED(int frequency[]) {
  analogWrite(LED_RED, frequency[1]);
  analogWrite(LED_GREEN, frequency[3]);
  analogWrite(LED_BLUE, frequency[5]);
}

void readMSGEQ7() {
  digitalWrite(MSGEQ7_RESET, HIGH);
  digitalWrite(MSGEQ7_RESET, LOW);

  for (band = 0; band < 7; band++) {
    digitalWrite(MSGEQ7_STROBE, LOW);
    delayMicroseconds(30);
    freqVal[band] = analogRead(MSGEQ7_OUTPUT);

    freqVal[band] = constrain(freqVal[band], filterVal, 1023);
    freqVal[band] = map(freqVal[band], filterVal, 1023, 0, 255);

    digitalWrite(MSGEQ7_STROBE, HIGH);
    Serial.print(freqVal[band]);
    Serial.print(" ");
  }
  Serial.print("\n");
}

void equalizerSetup(int freqBandVal[]) {
  for (int bandNum =0; bandNum < 7; bandNum++) {
    lcd.setCursor(2*bandNum+1,0);

    if (freqBandVal[bandNum] >= 204) {
      lcd.write(7);
      lcd.setCursor(2*bandNum+2,0);
      lcd.write(7);
    }
    else if (freqBandVal[bandNum] >= 140) {
      lcd.write(6);
      lcd.setCursor(2*bandNum+2,0);
      lcd.write(6);
    }
    else if (freqBandVal[bandNum] >= 91) {
      lcd.write(5);
      lcd.setCursor(2*bandNum+2,0);
      lcd.write(5);
    }
    else if (freqBandVal[bandNum] >= 55) {
      lcd.write(4);
      lcd.setCursor(2*bandNum+2,0);
      lcd.write(4);
    }
    else if (freqBandVal[bandNum] >= 30) {
      lcd.write(3);
      lcd.setCursor(2*bandNum+2,0);
      lcd.write(3);
    }
    else if (freqBandVal[bandNum] >= 14) {
      lcd.write(2);
      lcd.setCursor(2*bandNum+2,0);
      lcd.write(2);
    }
    else if (freqBandVal[bandNum] >= 5) {
      lcd.write(1);
      lcd.setCursor(2*bandNum+2,0);
      lcd.write(1);
    }
    else if (freqBandVal[bandNum] >= 0) {
      lcd.write(byte(0));
      lcd.setCursor(2*bandNum+2,0);
      lcd.write(byte(0));
    }
  }
}

void loop() {
  readMSGEQ7();
  sendToLED(freqVal);
  equalizerSetup(freqVal);
}
