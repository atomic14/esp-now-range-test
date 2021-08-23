#include "Arduino.h"
#include "GPS.h"
#include <esp_err.h>

void nmea_parser(void *param)
{
  GPS *gps = reinterpret_cast<GPS *>(param);
  // work space
  uint8_t buffer[100];
  while (true)
  {
    // Serial.println("Waiting for bytes");
    int bytes_read = Serial1.readBytes(buffer, 99);
    buffer[bytes_read] = '\0';
    if (bytes_read > 0)
    {
      Serial.printf(reinterpret_cast<char *>(buffer));
      gps->m_parser->readBuffer(buffer, bytes_read);
    }
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

GPS::GPS(uart_port_t uart_num, gpio_num_t tx, gpio_num_t rx) : m_uart_num(uart_num), m_tx(tx), m_rx(rx)
{
  m_parser = new nmea::NMEAParser();
  m_gps_service = new nmea::GPSService(*m_parser);
}

void GPS::begin()
{
  Serial1.begin(115200, SERIAL_8N1, m_rx, m_tx);
  Serial.println("Booting GPS up\n");
  pinMode(27, OUTPUT);
  digitalWrite(27, 0);
  delay(500);
  digitalWrite(27, 1);
  delay(500);
  // switch to NMEA mode at 4800 baud
  uint8_t cmd[] = {0xA0, 0xA2, 0x00, 0x18, 0x81, 0x02, 0x01, 0x01, 0x00, 0x01, 0x01, 0x01, 0x05, 0x01, 0x01, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x12, 0xC0, 0x01, 0x65, 0xB0, 0xB3};
  Serial1.write(cmd, 32);
  Serial1.flush(true);
  Serial1.updateBaudRate(4800);
  Serial.println("Switched to NMEA mode 4800 baud");
  TaskHandle_t task_handle;
  xTaskCreate(nmea_parser, "nmea_parser", 8192, this, 0, &task_handle);
}

void GPS::end()
{
}

std::string GPS::get_time()
{
  time_t time = get_fix().timestamp.getTime();
  char buf[sizeof "2011-10-08T07:07:09Z"];
  strftime(buf, sizeof buf, "%FT%TZ", gmtime(&time));
  return buf;
}