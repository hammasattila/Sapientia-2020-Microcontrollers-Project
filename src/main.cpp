#include "EnviromentVariables.h"

#include <Arduino.h>
#include <MFRC522.h>
#include <WiFi.h>

#define PIN_CHIP_SELECT_SPI GPIO_NUM_15
#define PIN_RFID_RESET GPIO_NUM_27

/// GLOBAL VARIABLES
MFRC522 mfrc522(PIN_CHIP_SELECT_SPI, PIN_RFID_RESET);
MFRC522::MIFARE_Key key;

void setup() {

    delay(1000);

    /// INIT SERIAL COMUNICATION.
    Serial.begin(115200);
    while (!Serial) {
        delay(100);
    }

    /// CONNECTING TIO THE NETWORK.
    WiFi.begin(WIFI_SSID, WIFI_PSWD);
    Serial.print(F("Connecting to WiFi"));
    while (WiFi.status() != WL_CONNECTED) {
        delay(250);
        Serial.print(F("."));
    }
    Serial.println();
    Serial.println(F("Connected to the WiFi succesfully."));
    Serial.print(F("Device's IP addres is: "));
    Serial.println(WiFi.localIP());

    /// INITIALIZING THE RFID READER
    Serial.println(F("Initializing SPI bus."));
    SPI.begin(GPIO_NUM_14, GPIO_NUM_12, GPIO_NUM_13, GPIO_NUM_15);
    delay(1000);
    Serial.println(F("Initializing MFRC522 reader."));
    mfrc522.PCD_Init();
    Serial.print(F("MFRC522 Digital self test: "));
    Serial.println(mfrc522.PCD_PerformSelfTest() ? F("OK") : F("DEFECT or UNKNOWN"));
    mfrc522.PCD_DumpVersionToSerial();
    mfrc522.PCD_Init();

    Serial.println();
    Serial.println(F("#### SYSTEM READY TO GO ####"));
    Serial.println();

}

void loop() {
    if (!mfrc522.PICC_IsNewCardPresent()) {
        // sleep(500);
        return;
    }

    if (!mfrc522.PICC_ReadCardSerial()) {
        return;
    }

    mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
}