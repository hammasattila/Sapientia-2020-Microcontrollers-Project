#include <EEPROM.h>
#include <MFRC522.h>
#include <WiFi.h>

String getKeysJson();
uint8_t findKey(MFRC522::Uid *);
bool addKey(MFRC522::Uid *);
bool removeKey(uint8_t);

String getLogsJson();
void saveLog(MFRC522::Uid *);