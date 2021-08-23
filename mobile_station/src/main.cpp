#include <Arduino.h>
#include <TFT_eSPI.h>
#include "Application.h"

Application *application;

TFT_eSPI display;

void setup()
{
  Serial.begin(115200);
  display.begin();
  display.setRotation(1);
  display.fillScreen(0);
  display.setCursor(0, 0, 2);
  display.setTextColor(TFT_WHITE, TFT_BLACK);
  display.setTextSize(1);
  display.println("Hello World");

  delay(1000);
  application = new Application(&display);
  application->begin();
}

void loop()
{
  vTaskDelay(1000);
}