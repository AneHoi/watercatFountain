#include "timemanager.h"
#include <RTClib.h>

TimeManager::TimeManager(const char* ssid, const char* password)
  : _ssid(ssid), _password(password), _timeClient(_ntpUDP, "pool.ntp.org", GMT_OFFSET_SECONDS) {}

void TimeManager::begin() {
  // Connect to Wi-Fi
  Serial.println("Connecting to WiFi...for time");
  WiFi.begin(_ssid, _password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi network");

  // Initialize and update NTP client
  _timeClient.begin();
  if (_timeClient.update()) {
    Serial.println("NTP time updated successfully");
  } else {
    Serial.println("Failed to update NTP time");
  }

  // Disconnect Wi-Fi
  WiFi.disconnect(true);
  Serial.println("Disconnected from WiFi");
}

std::string TimeManager::getCurrentTime() {
  // Get current time
  time_t now = _timeClient.getEpochTime();

  if (now == 0) {
    Serial.println("Failed to get the current time from NTP");
    return "1970-01-01 00:00:00";
  }

  // Convert time to struct tm
  struct tm* timeinfo;
  timeinfo = localtime(&now);
  
  // Create a buffer for the newly formatted timestamp
  char buffer[40];  // the buffer value is 40 to make sure it contains the formatted timestamp

  // Format timestamp
  strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);

  // Convert buffer to std::string and return it
  return std::string(buffer);
}
