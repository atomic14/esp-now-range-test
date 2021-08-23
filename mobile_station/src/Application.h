#pragma once

class GPS;
class TFT_eSPI;
class Logger;
class EspNowReceiver;

class Application
{
  TFT_eSPI *m_display;
  GPS *m_gps;
  Logger *m_logger;
  EspNowReceiver *m_receiver;

public:
  Application(TFT_eSPI *display);
  void begin();
  void update_display();
  void log(unsigned int elapsed_ms);
};