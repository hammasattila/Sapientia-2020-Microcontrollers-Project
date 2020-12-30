#include <EEPROM.h>
#include <MFRC522.h>
#include <WiFi.h>

void readKeysForClient(WiFiClient &);
uint8_t findKey(MFRC522::Uid *);
bool addKey(MFRC522::Uid *);
bool removeKey(uint8_t);

void readLogsForClient(WiFiClient &);
void saveLog(MFRC522::Uid *);