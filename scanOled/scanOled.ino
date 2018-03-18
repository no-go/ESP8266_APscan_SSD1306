#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include "SSD1306Spi.h"
#include "font.h"

static const uint8_t D0   = 16;
static const uint8_t D1   = 5;  // i2c SCL
static const uint8_t D2   = 4;  // i2c SDA
static const uint8_t D3   = 0;  // this with reset to GND: flash
static const uint8_t D4   = 2;  // wifi led
static const uint8_t D5   = 14; // pwm
static const uint8_t D6   = 12; // pwm
static const uint8_t D7   = 13;
static const uint8_t D8   = 15; // pwm
static const uint8_t D9   = 3;  // RX
static const uint8_t D10  = 1;  // TX
static const uint8_t D11  = 9;  // does only work without elf2image  --flash_mode dio
static const uint8_t D12  = 10; // does only work without elf2image  --flash_mode dio

// Initialize the OLED display using SPI
// GPIO 14 D5 -> CLK
// GPIO 13 D7 -> MOSI (DOUT)
// GPIO 16 D0 -> RES
// GPIO  4 D2 -> DC
// GPIO 15 D8 -> CS
SSD1306Spi display(D0, D2, D8);

int wifiLed = D4;

struct ip_addr *IPaddress;
IPAddress address;

void setup() {
  pinMode(D3, INPUT_PULLUP);
  pinMode(wifiLed, OUTPUT);
  digitalWrite(wifiLed, HIGH);

  display.init();
  display.flipScreenVertically();
  display.setFont(DejaVu_Sans_Condensed_8);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
}

void loop() {
  display.clear();

  int n = WiFi.scanNetworks();
  int line = 0;
  for (int i=0; i<n; i++,line++) {
    String ssid_scan;
    int32_t rssi_scan;
    uint8_t sec_scan;
    uint8_t* BSSID_scan;
    int32_t chan_scan;
    bool hidden_scan;
    
    WiFi.getNetworkInfo(i, ssid_scan, sec_scan, rssi_scan, BSSID_scan, chan_scan, hidden_scan);
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    
    display.drawStringMaxWidth(0, 9*line, 70, ssid_scan);
    if (display.getStringWidth(ssid_scan) > 69) line++;
    display.drawString(70,  9*line, String(rssi_scan) + String("dB") );
    display.drawString(101, 9*line, String("C") + String(chan_scan) );
    display.setTextAlignment(TEXT_ALIGN_RIGHT);
    switch (sec_scan) {
    case ENC_TYPE_WEP:
      display.drawString(128, 9*line, "E");
      break;
    case ENC_TYPE_TKIP:
      display.drawString(128, 9*line, "W1");
      break;
    case ENC_TYPE_CCMP:
      display.drawString(128, 9*line, "W2");
      break;
    case ENC_TYPE_NONE:
      display.drawString(128, 9*line, "--");
      digitalWrite(wifiLed, LOW);
      break;
    case ENC_TYPE_AUTO:
      display.drawString(128, 9*line, "a");
      break;
    }
    if ((9*line) > 54 && (i+1)<n) {
      display.display();
      delay(800);
      line=-1;
      display.clear();
    }
  }
  display.display();  
  delay(800);
  digitalWrite(wifiLed, HIGH);
}

