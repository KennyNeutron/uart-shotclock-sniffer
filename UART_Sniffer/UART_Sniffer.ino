#include <SoftwareSerial.h>

#define SOFT_RX 2
#define SOFT_TX 3

SoftwareSerial softSerial(SOFT_RX, SOFT_TX); // RX, TX

void setup() {
  Serial.begin(115200);      // USB Serial Monitor
  while (!Serial);

  softSerial.begin(19200);   // UART baud rate

  Serial.println(F("HEX + BIN UART Sniffer Ready"));
  Serial.println(F("HEX  | BINARY"));
  Serial.println(F("--------------"));
}

void loop() {
  if (softSerial.available()) {
    uint8_t b = softSerial.read();

    // HEX (2 digits)
    if (b < 0x10) Serial.print('0');
    Serial.print(b, HEX);
    Serial.print(F("  | "));

    // BINARY (MSB -> LSB)
    for (int i = 7; i >= 0; i--) {
      Serial.print((b >> i) & 0x01);
    }

    Serial.println();
  }
}
