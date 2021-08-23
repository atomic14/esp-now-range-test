
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "nvs_flash.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_now.h"
#include "esp_sleep.h"
#include "sdkconfig.h"
#include <esp_task_wdt.h>

#define CONFIG_ESPNOW_ENABLE_LONG_RANGE 1

extern "C"
{
  void app_main(void);
}

static const char *TAG = "BS";

const int MAX_ESP_NOW_PACKET_SIZE = 250;
const uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

uint32_t packet_number = 0;
uint32_t packets_sent = 0;
uint32_t packets_failed = 0;
uint32_t packets_received = 0;
uint8_t buffer[250];

/* WiFi should start before using ESPNOW */
static void wifi_init(void)
{
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));
  ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  ESP_ERROR_CHECK(esp_wifi_start());

#if CONFIG_ESPNOW_ENABLE_LONG_RANGE
  ESP_ERROR_CHECK(esp_wifi_set_protocol(ESP_IF_WIFI_STA, WIFI_PROTOCOL_LR));
#endif
}

static void receive_callback(const uint8_t *mac_addr, const uint8_t *data, int data_len)
{
  packets_received++;
}

bool sent = false;
static void sent_callback(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  sent = true;
}

static void send_message()
{
  uint32_t *data = reinterpret_cast<uint32_t *>(buffer);

  data[0] = packet_number;
  packet_number++;

  sent = false;
  esp_err_t result = esp_now_send(broadcastAddress, buffer, 250);
  if (result == ESP_OK)
  {
    while (!sent)
    {
    }
    packets_sent++;
  }
  else
  {
    ESP_LOGE(TAG, "Error: %s", esp_err_to_name(result));
    packets_failed++;
  }
}

void send_messages(void *param)
{
  esp_task_wdt_init(10, false);

  while (true)
  {
    for (int i = 0; i < 500; i++)
    {
      send_message();
    }
    vTaskDelay(1);
  }
}

void app_main()
{
  esp_task_wdt_init(10, false);

  // Initialize NVS
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
  {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);
  // start up esp-now
  wifi_init();
  ESP_ERROR_CHECK(esp_now_init());
  esp_now_peer_info_t peerInfo = {};
  memcpy(&peerInfo.peer_addr, broadcastAddress, 6);
  esp_now_add_peer(&peerInfo);
  esp_now_register_send_cb(sent_callback);
  esp_now_register_recv_cb(receive_callback);

  // run one device with this uncomment
  // xTaskCreate(send_messages, "send messages", 8192, NULL, 1, NULL);

  // just wait forever
  while (true)
  {
    vTaskDelay(pdMS_TO_TICKS(1000));
    // run one device with this uncomment
    // ESP_LOGI(TAG, "Sent %u packets, failed to send %u packets - %0.1fMbps", packets_sent, packets_failed, (packets_sent * 8.0f * 250.0f) / 1000000.0f);
    ESP_LOGI(TAG, "Received %u - %0.1fMbps", packets_received, (packets_received * 8.0f * 250.0f) / 1000000.0f);
    packets_sent = 0;
    packets_failed = 0;
    packets_received = 0;
  }
}
