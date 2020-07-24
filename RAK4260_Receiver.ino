#include <SPI.h>
#include <LoRa.h>

uint16_t counter = 0;

void hexDump(uint8_t *buf, uint16_t len) {
  String s = "|", t = "| |";
  Serial.println(F("+------------------------------------------------+ +----------------+"));
  for (uint16_t i = 0; i < len; i += 16) {
    for (uint8_t j = 0; j < 16; j++) {
      if (i + j >= len) {
        s = s + "   "; t = t + " ";
      } else {
        uint8_t c = buf[i + j];
        if (c < 16) s = s + "0";
        s = s + String(c, HEX) + " ";
        if (c < 32 || c > 127) t = t + ".";
        else t = t + (char)c;
      }
    }
    Serial.println(s + t + "|");
    s = "|"; t = "| |";
  }
  Serial.println(F("+------------------------------------------------+ +----------------+"));
}

char mybuffer[256];

void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("\n\n\nLoRa Receiver");
  LoRa.setPins(SS, RFM_RST, RFM_DIO0);
  if (!LoRa.begin(868E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  Serial.print("Setting up LoRa ");
  pinMode(RFM_SWITCH, OUTPUT);
  digitalWrite(RFM_SWITCH, 1);
  LoRa.setTxPower(20, PA_OUTPUT_PA_BOOST_PIN);
  LoRa.setSpreadingFactor(12);
  LoRa.setSignalBandwidth(500E3);
  LoRa.setCodingRate4(5);
  LoRa.setPreambleLength(8);
  Serial.println("[o]");
  delay(1000);
  pinMode(PIN_PA28, OUTPUT);
  digitalWrite(PIN_PA28, HIGH);
}

void loop() {
  // try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // received a packet
    Serial.print("Received packet with RSSI: ");
    Serial.println(LoRa.packetRssi());
    // read packet
    uint8_t ix = 0;
    while (LoRa.available()) {
      mybuffer[ix++] = (char)LoRa.read();
    }
    hexDump((uint8_t *)mybuffer, ix);
    digitalWrite(PIN_PA28, LOW);
    Serial.print("Sending packet: ");
    Serial.println(counter);
    digitalWrite(LED_BUILTIN, 1);
    // send packet
    digitalWrite(RFM_SWITCH, 0);
    LoRa.beginPacket();
    LoRa.print("Well received #");
    LoRa.print(counter);
    LoRa.endPacket();
    digitalWrite(RFM_SWITCH, 1);
    counter++;
    delay(500);
    digitalWrite(LED_BUILTIN, 0);
    delay(500);
    digitalWrite(PIN_PA28, HIGH);
  }
  delay(1000);
}
