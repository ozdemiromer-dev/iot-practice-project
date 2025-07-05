#include "BluetoothSerial.h"

const int ledPin = 25;
String device_name = "CORTEX";

BluetoothSerial SerialBT;

char currentCommand = '\0';
unsigned long stateStartTime = 0;
unsigned long lastBlinkTime = 0;
bool ledState = false;

// B modu için durum takibi
enum ModeBState { B_WAIT, B_ON };
ModeBState bState = B_WAIT;

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  SerialBT.begin(device_name);
  Serial.println("Bluetooth cihazı hazır. A/B/C/D/E komutlarını gönderin.");
}

void loop() {
  if (SerialBT.available()) {
    char received = SerialBT.read();
    if (received == 'A' || received == 'B' || received == 'C' || received == 'D' || received == 'E') {
      currentCommand = received;
      stateStartTime = millis();
      lastBlinkTime = millis();
      ledState = false;
      digitalWrite(ledPin, LOW);
      bState = B_WAIT;
      Serial.printf("Yeni komut alındı: %c\n", currentCommand);
    }
  }

  unsigned long now = millis();

  switch (currentCommand) {
    case 'A': // 15 dakika açık
      if ((now - stateStartTime) < 15UL * 60 * 1000) {
        digitalWrite(ledPin, HIGH);
      } else {
        digitalWrite(ledPin, LOW);
      }
      break;

    case 'B': // 90dk kapalı -> 5dk açık döngüsü
      switch (bState) {
        case B_WAIT:
          digitalWrite(ledPin, LOW);
          if ((now - stateStartTime) >= 90UL * 60 * 1000) {
            bState = B_ON;
            stateStartTime = now;
          }
          break;
        case B_ON:
          digitalWrite(ledPin, HIGH);
          if ((now - stateStartTime) >= 5UL * 60 * 1000) {
            bState = B_WAIT;
            stateStartTime = now;
          }
          break;
      }
      break;

    case 'C': // Sürekli açık
      digitalWrite(ledPin, HIGH);
      break;

    case 'D': // 10Hz yanıp sönme (her 50ms'de bir değişir)
      if ((now - lastBlinkTime) >= 50) {
        ledState = !ledState;
        digitalWrite(ledPin, ledState);
        lastBlinkTime = now;
      }
      break;

    case 'E': // 40Hz yanıp sönme (her 12.5ms'de bir değişir)
      if ((now - lastBlinkTime) >= 13) { // Yaklaşık 12.5ms
        ledState = !ledState;
        digitalWrite(ledPin, ledState);
        lastBlinkTime = now;
      }
      break;
  }

  delay(1); // minimum gecikme
}
