#include <SoftwareSerial.h>

#define SOFT_RX 10
#define SOFT_TX 11

SoftwareSerial softSerial(SOFT_RX, SOFT_TX); // RX, TX

// State machine
enum {
  WAIT_MARKER,
  WAIT_SC_ONES,
  WAIT_SC_TENS,
  WAIT_GT_ONES,
  WAIT_GT_TENS
};

uint8_t state = WAIT_MARKER;

// Shot clock
uint8_t sc_ones = 0;
uint8_t sc_tens = 0;

// Game time seconds
uint8_t gt_ones = 0;
uint8_t gt_tens = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  softSerial.begin(19200);

  Serial.println(F("UART Decoder Ready"));
  Serial.println(F("Waiting for 0x80 marker..."));
}

void loop() {
  if (softSerial.available()) {
    uint8_t b = softSerial.read();

    switch (state) {

      case WAIT_MARKER:
        if (b == 0x80) {
          state = WAIT_SC_ONES;
        }
        break;

      case WAIT_SC_ONES:
        sc_ones = b;
        state = WAIT_SC_TENS;
        break;

      case WAIT_SC_TENS:
        sc_tens = b;
        state = WAIT_GT_ONES;
        break;

      case WAIT_GT_ONES:
        gt_ones = b;
        state = WAIT_GT_TENS;
        break;

      case WAIT_GT_TENS:
        gt_tens = b;

        // Combine values
        uint8_t shotclock = (sc_tens * 10) + sc_ones;
        uint8_t gametime_seconds = (gt_tens * 10) + gt_ones;

        // Output
        Serial.print(F("ShotClock = "));
        Serial.print(shotclock);
        Serial.print(F(" | GameTime Seconds = "));
        Serial.println(gametime_seconds);

        // Reset for next frame
        state = WAIT_MARKER;
        break;
    }
  }
}
