#pragma once

#include <driver/uart.h>

#include "NemaTode/include/nmeaparse/nmea.h"

class GPS
{
private:
  uart_port_t m_uart_num;
  gpio_num_t m_tx;
  gpio_num_t m_rx;

  nmea::NMEAParser *m_parser;
  nmea::GPSService *m_gps_service;

public:
  GPS(uart_port_t uart_num, gpio_num_t tx, gpio_num_t rx);
  nmea::GPSFix &get_fix() { return m_gps_service->fix; }
  int get_tracking_satellites() { return m_gps_service->fix.trackingSatellites; }
  int get_visibile_satellites() { return m_gps_service->fix.visibleSatellites; }
  char get_status() { return m_gps_service->fix.status; }
  std::string get_time();
  void begin();
  void end();
  friend void nmea_parser(void *param);
};