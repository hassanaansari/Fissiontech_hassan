#include <WiFi.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>
#include <WiFiClientSecure.h>
#include "cert.h"

const char * ssid = "Fission Tech 2.4G";
const char * password = "Dev@4fission_";


const char* versionURL = "https://raw.githubusercontent.com/hassanaansari/Fissiontech_hassan/main/esp32_ota/bin_version.txt";
const char* firmwareURL = "https://raw.githubusercontent.com/hassanaansari/Fissiontech_hassan/main/esp32_ota/fw.bin";
const char* currentVersion = "2.0";

// const char* ssid = "your_SSID";
// const char* password = "your_PASSWORD";
// const char* versionURL = "https://raw.githubusercontent.com/your_username/your_repo/main/version.txt";
// const char* firmwareURL = "https://raw.githubusercontent.com/your_username/your_repo/main/firmware.bin";
// const char* currentVersion = "1.0.0"; // Change this to match your current version

void setup() {
  Serial.begin(115200);
  pinMode(2,OUTPUT);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
  checkForUpdates();
}

void loop() {
  // Your main code here
  digitalWrite(2,1);
  delay(100);
  digitalWrite(2,0);
  delay(100);
}

void checkForUpdates() {
  HTTPClient http;

  // Check current version
  http.begin(versionURL);
  int httpCode = http.GET();
  if (httpCode == HTTP_CODE_OK) {
    String newVersion = http.getString();
    newVersion.trim();
    Serial.println("Current version: " + String(currentVersion));
    Serial.println("Available version: " + newVersion);
    if (newVersion != currentVersion) {
      Serial.println("New firmware available, updating...");
      updateFirmware();
    } else {
      Serial.println("Firmware is up to date");
    }
  } else {
    Serial.printf("Failed to check for updates, HTTP code: %d\n", httpCode);
  }

  http.end();
}

void updateFirmware() {
  HTTPClient http;
  http.begin(firmwareURL);
  int httpCode = http.GET();
  if (httpCode == HTTP_CODE_OK) {
    int contentLength = http.getSize();
    bool canBegin = Update.begin(contentLength);
    if (canBegin) {
      WiFiClient *client = http.getStreamPtr();
      size_t written = Update.writeStream(*client);
      if (written == contentLength) {
        Serial.println("Written : " + String(written) + " successfully");
      } else {
        Serial.println("Written only : " + String(written) + "/" + String(contentLength) + ". Retry?");
      }

      if (Update.end()) {
        Serial.println("OTA done!");
        if (Update.isFinished()) {
          Serial.println("Update successfully completed. Rebooting.");
          ESP.restart();
        } else {
          Serial.println("Update not finished? Something went wrong!");
        }
      } else {
        Serial.println("Error Occurred. Error #: " + String(Update.getError()));
      }
    } else {
      Serial.println("Not enough space to begin OTA");
    }
  } else {
    Serial.printf("Firmware download failed, HTTP code: %d\n", httpCode);
  }

  http.end();
}
