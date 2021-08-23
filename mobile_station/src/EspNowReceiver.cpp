#include "Arduino.h"
#include <WiFi.h>
#include <esp_now.h>
#include "EspNowReceiver.h"

const int MAX_ESP_NOW_PACKET_SIZE = 250;
const uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

static EspNowReceiver *instance = NULL;

void receiveCallback(const uint8_t *macAddr, const uint8_t *data, int dataLen)
{
  instance->add_packet(data);
}

EspNowReceiver::EspNowReceiver()
{
  instance = this;
}

bool EspNowReceiver::begin()
{
  esp_err_t result = esp_now_init();
  if (result == ESP_OK)
  {
    Serial.println("ESPNow Init Success");
    esp_now_register_recv_cb(receiveCallback);
  }
  else
  {
    Serial.printf("ESPNow Init failed: %s\n", esp_err_to_name(result));
    return false;
  }
  // this will broadcast a message to everyone in range
  esp_now_peer_info_t peerInfo = {};
  memcpy(&peerInfo.peer_addr, broadcastAddress, 6);
  if (!esp_now_is_peer_exist(broadcastAddress))
  {
    result = esp_now_add_peer(&peerInfo);
    if (result != ESP_OK)
    {
      Serial.printf("Failed to add broadcast peer: %s\n", esp_err_to_name(result));
      return false;
    }
  }
  return true;
}

void EspNowReceiver::add_packet(const uint8_t *data)
{
  uint32_t packet_number = reinterpret_cast<const uint32_t *>(data)[0];
  if (packet_number < m_current_packet_number)
  {
    m_out_of_order++;
  }
  else
  {
    m_current_packet_number = packet_number;
  }
  m_packets_received++;
}
