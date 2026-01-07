/*
  arduino_uno_hcsr04_16x2_i2c_404wait.ino
  HC-SR04 -> Arduino Uno, display on 16x2 I2C (PCF8574 backpack)
  When no echo (pulse=0) the LCD shows "404 wait".

  - TRIG -> D9
  - ECHO -> D8
  - LCD SDA -> A4, SCL -> A5
  - Change LCD_I2C_ADDR to 0x3F if your backpack uses that address
*/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define LCD_I2C_ADDR 0x27  // change if needed
LiquidCrystal_I2C lcd(LCD_I2C_ADDR, 16, 2);

const uint8_t TRIG_PIN = 9;
const uint8_t ECHO_PIN = 8;
const unsigned long PULSE_TIMEOUT_US = 30000UL; // 30 ms

float measureDistanceCm(unsigned long &duration_out) {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  unsigned long duration = pulseIn(ECHO_PIN, HIGH, PULSE_TIMEOUT_US);
  duration_out = duration;
  if (duration == 0) return -1.0f;
  return (float)duration / 58.0f;
}

void setup() {
  Serial.begin(115200);
  Wire.begin(); // UNO: A4 SDA, A5 SCL
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  digitalWrite(TRIG_PIN, LOW);

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Digital Measure ");
  lcd.setCursor(0,1);
  lcd.print("By QuantumThread");
  delay(2000);
}

void loop() {
  unsigned long dur;
  float dist = measureDistanceCm(dur);

  lcd.clear();
  if (dist < 0) {
    // No echo: show "404 wait" instead of pulse_us: 0
    lcd.setCursor(0,0);
    lcd.print("Distance: ?");
    lcd.setCursor(0,1);
    lcd.print("Unknown");
    // pad remainder to clear previous characters
    lcd.print("        ");
    Serial.println("No echo -> 404 wait");
  } else {
    lcd.setCursor(0,0);
    lcd.print("Distance:");
    lcd.setCursor(0,1);
    lcd.print(dist, 1);
    lcd.print(" cm ");
    float inches = dist / 2.54;
    lcd.setCursor(10,1);
    lcd.print(inches, 1);
    lcd.print("in");
    Serial.print("pulse_us=");
    Serial.print(dur);
    Serial.print("  dist_cm=");
    Serial.println(dist, 2);
  }

  delay(250);
}