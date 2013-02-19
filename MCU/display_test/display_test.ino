#include <Wire.h>
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"

// TODO: one of these ends up being mounted upside-down; correct for that automatically

Adafruit_BicolorMatrix matrixR = Adafruit_BicolorMatrix();
Adafruit_BicolorMatrix matrixL = Adafruit_BicolorMatrix();

void show_bmp(Adafruit_BicolorMatrix *matrix, uint8_t *img, uint8_t color) {
  matrix->clear();
  matrix->drawBitmap(0, 0, img, 8, 8, color);
  matrix->writeDisplay();
}

void setup() {
  matrixR.begin(0x74);  // pass in the address
  matrixL.begin(0x70);  // pass in the address
}

static uint8_t __attribute__ ((progmem)) smile_bmp[]={0x3C, 0x42, 0x95, 0xA1, 0xA1, 0x95, 0x42, 0x3C};
static uint8_t __attribute__ ((progmem)) frown_bmp[]={0x3C, 0x42, 0xA5, 0x91, 0x91, 0xA5, 0x42, 0x3C};
static uint8_t __attribute__ ((progmem)) neutral_bmp[]={0x3C, 0x42, 0x95, 0x91, 0x91, 0x95, 0x42, 0x3C};



void loop() {
  show_bmp(&matrixR, smile_bmp, LED_GREEN);
  show_bmp(&matrixL, smile_bmp, LED_GREEN);
  delay(500);

  show_bmp(&matrixR, neutral_bmp, LED_YELLOW);
  show_bmp(&matrixL, neutral_bmp, LED_YELLOW);
  delay(500);

  show_bmp(&matrixR, frown_bmp, LED_RED);
  show_bmp(&matrixL, frown_bmp, LED_RED);
  delay(500);


  matrixR.setTextWrap(false);
  matrixR.setTextSize(1);
  matrixR.setTextColor(LED_GREEN);
  
  matrixL.setTextWrap(false);
  matrixL.setTextSize(1);
  matrixL.setTextColor(LED_RED);
  
  for (int8_t x=7; x>=-36; x--) {
    matrixR.clear();
    matrixL.clear();
    
    matrixR.setCursor(x,0);
    matrixL.setCursor(x,0);

    matrixR.print("Right");
    matrixL.print("Left");

    matrixR.writeDisplay();
    matrixL.writeDisplay();

    delay(100);
  }
}
