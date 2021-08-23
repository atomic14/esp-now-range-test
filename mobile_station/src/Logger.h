#pragma once

class Logger
{
private:
  FILE *m_fp;

public:
  Logger();
  bool begin();
  void log(std::string time, bool gps_lock, double lat, double lng, unsigned int packets);
};