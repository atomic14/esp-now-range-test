#include <Arduino.h>
#include <WiFi.h>
#include "esp_wifi.h"
#include "GPS.h"
#include "Logger.h"
#include "SDCard.h"
#include "Application.h"
#include "config.h"
#include "Arduino.h"
#include "EspNowReceiver.h"
#include <TFT_eSPI.h>

#define CONFIG_ESPNOW_ENABLE_LONG_RANGE

void logging_task(void *param)
{
  Application *application = reinterpret_cast<Application *>(param);
  unsigned long last_time = millis();
  while (true)
  {
    vTaskDelay(pdMS_TO_TICKS(1000));
    unsigned long current_time = millis();
    unsigned int elapsed = current_time - last_time;
    application->log(elapsed);
    last_time = current_time;
  }
}

void display_task(void *param)
{
  Application *application = reinterpret_cast<Application *>(param);
  while (true)
  {
    vTaskDelay(pdMS_TO_TICKS(5000));
    application->update_display();
  }
}

Application::Application(TFT_eSPI *display) : m_display(display)
{
  m_gps = new GPS(UART_PORT, UART_TX, UART_RX);
  m_logger = new Logger();
  m_receiver = new EspNowReceiver();
}

void Application::begin()
{
  // bring up WiFi
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
#ifdef CONFIG_ESPNOW_ENABLE_LONG_RANGE
  ESP_ERROR_CHECK(esp_wifi_set_protocol(
      WIFI_IF_STA,
      WIFI_PROTOCOL_LR));
#endif

  new SDCard("/sdcard", PIN_NUM_MISO, PIN_NUM_MOSI, PIN_NUM_CLK, PIN_NUM_CS);

  m_gps->begin();
  m_receiver->begin();
  m_logger->begin();
  TaskHandle_t task_handle_logger;
  TaskHandle_t task_handle_display;
  xTaskCreate(logging_task, "logging", 8196, this, 1, &task_handle_logger);
  xTaskCreate(display_task, "display", 8192, this, 1, &task_handle_display);
}

void Application::log(unsigned int elapsed_ms)
{
  unsigned int packets = m_receiver->get_packets();
  m_receiver->reset_counts();
  unsigned int packets_per_second = (packets * 1000) / elapsed_ms;
  Serial.printf("Elapsed %u, Packets %u, per second %u\n", elapsed_ms, packets, packets_per_second);
  if (m_gps->get_status() == 'A')
  {
    m_logger->log(
        m_gps->get_time(),
        m_gps->get_status() == 'A',
        m_gps->get_fix().latitude,
        m_gps->get_fix().longitude,
        (packets * 1000) / elapsed_ms);
  }
}

void Application::update_display()
{
  // Serial.printf(m_gps->get_fix().toString().c_str());
  m_display->fillScreen(0);
  m_display->setCursor(0, 0, 2);
  m_display->println(m_gps->get_fix().toString().c_str());
  m_display->fillScreen(0);
  m_display->setCursor(0, 0, 2);
  m_display->printf("Tracking %d of %d visible\n", m_gps->get_tracking_satellites(), m_gps->get_visibile_satellites());
  m_display->printf("Status: %s\n", m_gps->get_status() == 'A' ? "LOCK!" : "SEARCHING...");
  m_display->printf("Time: %s\n", m_gps->get_time().c_str());
  m_display->printf("Lat: %f\n", m_gps->get_fix().latitude);
  m_display->printf("Lng: %f\n", m_gps->get_fix().longitude);
  m_display->printf("Almanac: %.1f%% complete\n", m_gps->get_fix().almanac.percentComplete());
}