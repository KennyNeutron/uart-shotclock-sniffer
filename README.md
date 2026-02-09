# UART Shot Clock Sniffer & Decoder

A reverse-engineered **UART sniffer and decoder** for basketball shot clock / scoreboard systems.

This project listens to a proprietary UART data stream, decodes shot clock, game time, scores, fouls, quarter, and ball possession, and provides **real-time readable output** along with **event-based buzzer alerts**.

Designed to run **standalone** (no PC required), with Serial Monitor used only for debugging/logging.

---

## Features

- UART frame decoding at **19200 baud**
- Marker-based frame synchronization (`0x80`)
- Shot clock decoding (24 / 14 / 0)
- Game time decoding (MM:SS)
- Guest & Home scores (up to 3 digits)
- Guest & Home fouls
- Quarter indicator
- Ball possession indicator
- **Shot-clock violation buzzer**
- **Game-over buzzer**
- One-shot buzzer latch (no repeated buzzing)
- Auto-scroll-friendly Serial output
- Robust non-blocking design (`millis()`)

---

## Hardware Requirements

- Arduino Uno / Nano / compatible AVR board
- UART signal source (scoreboard / shot clock controller)
- Active buzzer (or piezo)
- Optional LED for decode indicator

### Pin Assignments

| Function   | Pin                   |
| ---------- | --------------------- |
| UART RX    | D2                    |
| UART TX    | D3 (unused, optional) |
| Buzzer     | D8                    |
| Decode LED | D13                   |

---

## UART Configuration

| Parameter    | Value     |
| ------------ | --------- |
| Baud Rate    | 19200     |
| Data Bits    | 8         |
| Parity       | None      |
| Stop Bits    | 1         |
| Bit Order    | LSB first |
| Idle State   | HIGH      |
| Signal Level | TTL       |

---

## Frame Structure

```
0x80  Header
SC_Ones
SC_Tens
GT_Sec_Ones
GT_Sec_Tens
Unknown1
GT_Min_Ones
GT_Min_Tens
Guest_Score_Ones
Guest_Score_Tens
Guest_Score_Hundreds
Home_Score_Ones
Home_Score_Tens
Home_Score_Hundreds
Guest_Foul
Quarter
Ball_Possession
Home_Foul
```

Total frame length: **18 bytes**

---

## Buzzer Logic

- **Shot Clock Violation**
  - Shot clock = 0
  - Game time > 0:00
  - Buzz 2 seconds

- **Game Over**
  - Shot clock = 0
  - Game time = 0:00
  - Buzz 5 seconds

- **One-shot latch**
  - Buzzer triggers once per shot clock reset

---

## Serial Output Example

```
SC 24 | GT 19:53 | G 87 (F 4) | H 91 (F 5) | Q3 | POSS LEFT
```

---

## Repository

**uart-shotclock-sniffer**

---

## License

Open-source. Use freely.
