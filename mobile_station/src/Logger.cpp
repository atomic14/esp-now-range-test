#include "Arduino.h"
#include "Logger.h"

Logger::Logger()
{
}

bool Logger::begin()
{
  char fname[100];
  for (int i = 1; i < 100; i++)
  {
    sprintf(fname, "/sdcard/log_%03d.csv", i);
    Serial.printf("Checking to see if file %s exists\n", fname);
    FILE *fp = fopen(fname, "r");
    // file does not exist yet so we can use it
    if (!fp)
    {
      break;
    }
    // try the next slot
    fclose(fp);
  }
  Serial.printf("Opening file %s\n", fname);
  m_fp = fopen(fname, "wt");
  if (m_fp)
  {
    Serial.println("Success!");
    fprintf(m_fp, "time,lat,lng,gps_lock,packets\n");
  }
  else
  {
    Serial.println("Failed to open file");
  }
  return m_fp != NULL;
}

void Logger::log(std::string time, bool gps_lock, double lat, double lng, unsigned int packets_per_second)
{
  if (m_fp)
  {
    Serial.println("Logging data");
    fprintf(m_fp, "%s,%f,%f,%s,%u\n", time.c_str(), lat, lng, gps_lock ? "GPS" : "-", packets_per_second);
    fflush(m_fp);
    fsync(fileno(m_fp));
  }
}