#include <Arduino.h>
#include <MFRC522.h>
#include <WiFi.h>

#include "ESPAsyncWebServer.h"
// #include "AsyncTCP.h"

#include "EnviromentVariables.h"
#include "persistency.h"
#include "web.h"

MFRC522 mfrc522(PIN_CHIP_SELECT_SPI, PIN_RFID_RESET);
MFRC522::MIFARE_Key key;

void setup() {

    delay(1000);

    /// INIT SERIAL COMUNICATION.
    Serial.begin(115200);
    while (!Serial) {
        delay(100);
    }

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

    /// INIT EEPROM
    EEPROM.begin(512);
    // for(int i = 0 ; i < 512;++i) {
    //     EEPROM.write(i, 0);
    // }
    // EEPROM.commit();

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

    /// STARTING WEB SERVER
    Serial.println(F("Starting web server."));
    setupAsyncWebServer();

    Serial.println();
    Serial.println(F("#### SYSTEM READY TO GO ####"));
    Serial.println();
}

void loop() {
    if (!mfrc522.PICC_IsNewCardPresent()) {
        return;
    }

    if (!mfrc522.PICC_ReadCardSerial()) {
        return;
    }

    // mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
    mfrc522.PICC_DumpDetailsToSerial(&(mfrc522.uid));
    saveLog(&(mfrc522.uid));
    if(addNextCard) {
        addKey(&(mfrc522.uid));
        addNextCard = false;
    }
    Serial.println(findKey(&(mfrc522.uid)) != UINT8_MAX ? "ACCEPTED" : "DENNIED");
    mfrc522.PICC_HaltA();
}
