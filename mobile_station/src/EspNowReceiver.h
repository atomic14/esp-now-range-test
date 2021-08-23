#pragma once

class EspNowReceiver
{
private:
  unsigned int m_out_of_order = 0;
  unsigned int m_packets_received = 0;
  unsigned int m_current_packet_number = 0;

public:
  EspNowReceiver();
  bool begin();
  void add_packet(const uint8_t *data);
  unsigned int get_packets()
  {
    return m_packets_received;
  }
  unsigned int  get_out_of_order()
  {
    return m_out_of_order;
  }
  void reset_counts()
  {
    m_packets_received = 0;
    m_out_of_order = 0;
  }
};