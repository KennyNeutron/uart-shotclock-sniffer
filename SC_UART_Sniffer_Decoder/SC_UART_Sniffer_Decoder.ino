#include <SoftwareSerial.h>

#define buzzerPin 8
#define buttonPin 7
#define SOFT_RX 2
#define SOFT_TX 3

SoftwareSerial softSerial(SOFT_RX, SOFT_TX);


// State machine
enum {
  WAIT_HEADER,
  SC_ONES,
  SC_TENS,
  GT_SEC_ONES,
  GT_SEC_TENS,
  UNKNOWN1,
  GT_MIN_ONES,
  GT_MIN_TENS,
  GUEST_ONES,
  GUEST_TENS,
  GUEST_HUNDREDS,
  HOME_ONES,
  HOME_TENS,
  HOME_HUNDREDS,
  GUEST_FOUL,
  QTR,
  BALLPOSS,
  HOME_FOUL
};

uint8_t state = WAIT_HEADER;

// Raw bytes
uint8_t sc_o, sc_t;
uint8_t sec_o, sec_t;
uint8_t min_o, min_t;
uint8_t guest_o, guest_t, guest_h;
uint8_t home_o, home_t, home_h;
uint8_t guest_foul, home_foul;
uint8_t qtr, ballposs;
uint8_t unknown1;

// Helpers
uint8_t scoreDigit(uint8_t b) {
  return (b == 0x0F) ? 0 : b;
}

const char* possText(uint8_t b) {
  if (b == 0x82) return "LEFT";
  if (b == 0x83) return "RIGHT";
  return "UNK";
}

bool buzzerFlag = false;
uint8_t buzzerScore = 0;
bool buzzerHasBuzzed = false;

void setup() {
  pinMode(buzzerPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  digitalWrite(buzzerPin, 0);

  Serial.begin(115200);
  softSerial.begin(19200);

  Serial.println(F("UART ShotClock Decoder Ready"));
}

void loop() {
  if (!digitalRead(buttonPin)) {
    digitalWrite(buzzerPin, 1);
  } else {
    digitalWrite(buzzerPin, 0);
  }

  if (!softSerial.available()) return;
  uint8_t b = softSerial.read();

  switch (state) {

    case WAIT_HEADER:
      if (b == 0x80) state = SC_ONES;
      break;

    case SC_ONES:
      sc_o = b;
      state = SC_TENS;
      break;
    case SC_TENS:
      sc_t = b;
      state = GT_SEC_ONES;
      break;

    case GT_SEC_ONES:
      sec_o = b;
      state = GT_SEC_TENS;
      break;
    case GT_SEC_TENS:
      sec_t = b;
      state = UNKNOWN1;
      break;

    case UNKNOWN1:
      unknown1 = b;
      state = GT_MIN_ONES;
      break;

    case GT_MIN_ONES:
      min_o = b;
      state = GT_MIN_TENS;
      break;
    case GT_MIN_TENS:
      min_t = b;
      state = GUEST_ONES;
      break;

    case GUEST_ONES:
      guest_o = b;
      state = GUEST_TENS;
      break;
    case GUEST_TENS:
      guest_t = b;
      state = GUEST_HUNDREDS;
      break;
    case GUEST_HUNDREDS:
      guest_h = b;
      state = HOME_ONES;
      break;

    case HOME_ONES:
      home_o = b;
      state = HOME_TENS;
      break;
    case HOME_TENS:
      home_t = b;
      state = HOME_HUNDREDS;
      break;
    case HOME_HUNDREDS:
      home_h = b;
      state = GUEST_FOUL;
      break;

    case GUEST_FOUL:
      guest_foul = b;
      state = QTR;
      break;
    case QTR:
      qtr = b;
      state = BALLPOSS;
      break;
    case BALLPOSS:
      ballposs = b;
      state = HOME_FOUL;
      break;

    case HOME_FOUL:
      home_foul = b;

      // Decode
      uint8_t shotclock = (sc_t * 10) + sc_o;
      uint8_t seconds = (sec_t * 10) + sec_o;
      uint8_t minutes = (min_t * 10) + min_o;

      if (sc_t == 0 && sc_o == 0 && minutes != 0 && !buzzerFlag) {
        buzzerScore++;
      } else if (sc_t == 0 && sc_o == 0 && seconds == 0 && minutes == 0 && !buzzerFlag) {
        buzzerScore += 10;
        buzzerFlag = true;
      }

      if (sc_t != 0 && sc_o != 0) {
        buzzerFlag = false;
        buzzerScore = 0;
      }

      if (buzzerScore >= 6 && buzzerScore <= 10 && !buzzerFlag && !buzzerHasBuzzed) {
        digitalWrite(buzzerPin, 1);
        delay(2000);
        digitalWrite(buzzerPin, 0);
        buzzerFlag = true;
        buzzerHasBuzzed = true;
      }

      if (buzzerScore >= 60 && buzzerScore <= 100 && !buzzerFlag && !buzzerHasBuzzed) {
        digitalWrite(buzzerPin, 1);
        delay(5000);
        digitalWrite(buzzerPin, 0);
        buzzerFlag = true;
        buzzerHasBuzzed = true;
      }

      if (sc_t == 1 && sc_o == 4) {
        buzzerHasBuzzed = false;
      } else if (sc_t == 2 && sc_o == 4) {
        buzzerHasBuzzed = false;
      }

      uint16_t guest_score =
        scoreDigit(guest_h) * 100 + scoreDigit(guest_t) * 10 + guest_o;

      uint16_t home_score =
        scoreDigit(home_h) * 100 + scoreDigit(home_t) * 10 + home_o;

      // ONE clean line (auto-scroll friendly)
      Serial.print(F("SC "));
      Serial.print(shotclock);

      Serial.print(F(" | GT "));
      Serial.print(minutes);
      Serial.print(':');
      if (seconds < 10) Serial.print('0');
      Serial.print(seconds);

      Serial.print(F(" | G "));
      Serial.print(guest_score);
      Serial.print(F(" (F "));
      Serial.print(guest_foul);
      Serial.print(')');

      Serial.print(F(" | H "));
      Serial.print(home_score);
      Serial.print(F(" (F "));
      Serial.print(home_foul);
      Serial.print(')');

      Serial.print(F(" | Q"));
      Serial.print(qtr);

      Serial.print(F(" | POSS "));
      Serial.print(possText(ballposs));

      Serial.println();

      state = WAIT_HEADER;
      break;
  }
}
