#include <SoftwareSerial.h>

#define SOFT_RX 10
#define SOFT_TX 11

SoftwareSerial softSerial(SOFT_RX, SOFT_TX); // RX, TX

void setup() {
  Serial.begin(115200);     // USB Serial Monitor
  while (!Serial);

  softSerial.begin(19200);   // test baud (change if needed)

  Serial.println(F("HEX UART Sniffer Ready"));
  Serial.println(F("-----------------------"));
}

void loop() {
  if (softSerial.available()) {
    uint8_t b = softSerial.read();

    // Print as 2-digit HEX
    if (b < 0x10) Serial.print('0');
    Serial.println(b, HEX);
  }
}
