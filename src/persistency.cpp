#include "persistency.h"

#define PERSISTENCY_LOG_KEY_COUNT 48
#define PERSISTENCY_LOG_ADDRESS_COUNT 16
#define PERSISTENCY_LOG_ADDRESS_START 8 * PERSISTENCY_LOG_KEY_COUNT

void readKeysForClient(WiFiClient &client) {
    const uint8_t keyCount = EEPROM.read(510);

    client.print(F("{\"keys\": ["));
    for (uint8_t i = 0; i < keyCount; ++i) {
        client.print(F("{\"key\":\""));
        uint16_t address = i * 8;
        uint8_t keyLength = EEPROM.read(address++);
        for (uint8_t j = 0; j < keyLength; ++j, ++address) {
            const uint8_t b = EEPROM.read(address);

            client.print(b, HEX);
            if (j < keyLength - 1) {
                client.print(":");
            }
        }
        client.print(keyCount <= i + 1 ? F("\"}") : F("\"},"));
    }
    client.print(F("]}"));
}

bool addKey(MFRC522::Uid *uid) {
    const uint8_t keyCount = EEPROM.read(510);

    if (findKey(uid) != UINT8_MAX) {
        return false;
    }

    if (keyCount == PERSISTENCY_LOG_KEY_COUNT) {
        return false;
    }

    uint16_t address = keyCount * 8;
    EEPROM.write(address, uid->size);
    for (uint8_t i = 0; i < uid->size; ++i) {
        EEPROM.write(address + i + 1, uid->uidByte[i]);
    }

    EEPROM.write(510, keyCount + 1);
    EEPROM.commit();

    return true;
}

bool removeKey(uint8_t pos) {
    const uint8_t keyCount = EEPROM.read(510);

    if (keyCount <= pos) {
        return false;
    }

    uint16_t addressTo = pos * 8;
    uint16_t addressFrom = (keyCount - 1) * 8;

    if (addressTo != addressFrom) {
        auto keyLength = EEPROM.read(addressFrom);
        EEPROM.put(addressTo, keyLength);
        for (uint8_t i = 0; i < keyLength; ++i) {
            EEPROM.put(addressTo + i + 1, EEPROM.read(addressFrom + i + 1));
        }
    }

    EEPROM.write(510, keyCount - 1);
    EEPROM.commit();

    return true;
}

uint8_t findKey(MFRC522::Uid *uid) {
    const uint8_t keyCount = EEPROM.read(510);

    for (uint8_t i = 0; i < keyCount; ++i) {
        uint16_t address = i * 8;
        if (uid->size != EEPROM.read(address++)) {
            continue;
        }

        uint8_t j;
        for (j = 0; j < uid->size; ++j, ++address) {
            if (uid->uidByte[j] != EEPROM.read(address)) {
                break;
            }
        }

        if (j == uid->size) {
            return i;
        }
    }

    return UINT8_MAX;
}

void readLogsForClient(WiFiClient &client) {
    const uint8_t logState = EEPROM.read(511);
    const uint8_t logCount = logState & 0x0F;
    uint8_t logIndex = ((logCount == PERSISTENCY_LOG_ADDRESS_COUNT - 1) ? (logState >> 4) & 0x0F : 0);

    client.print(F("{\"logs\": ["));
    for (uint8_t i = 0; i < logCount; ++i, ++logIndex) {
        client.print(F("{\"key\":\""));
        if (PERSISTENCY_LOG_ADDRESS_COUNT <= logIndex) {
            logIndex = logIndex % PERSISTENCY_LOG_ADDRESS_COUNT;
        }
        uint16_t address = logIndex * 8 + PERSISTENCY_LOG_ADDRESS_START;
        uint8_t keyLength = EEPROM.read(address++);
        for (uint8_t j = 0; j < keyLength; ++j, ++address) {
            const uint8_t b = EEPROM.read(address);

            client.print(b, HEX);
            if (j < keyLength - 1) {
                client.print(":");
            }
        }
        client.print(logCount <= i + 1 ? F("\"}") : F("\"},"));
    }
    client.print(F("]}"));
}

void saveLog(MFRC522::Uid *uid) {
    const uint8_t logState = EEPROM.read(511);
    uint8_t logCount = logState & 0x0F;
    uint8_t logIndex = (logState >> 4) & 0x0F;

    uint16_t address = logIndex * 8 + PERSISTENCY_LOG_ADDRESS_START;
    EEPROM.write(address, uid->size);
    for (uint8_t i = 0; i < uid->size; ++i) {
        EEPROM.write(address + i + 1, uid->uidByte[i]);
    }

    logIndex += 1;
    if (PERSISTENCY_LOG_ADDRESS_COUNT <= logIndex) {
        logIndex = logIndex % PERSISTENCY_LOG_ADDRESS_COUNT;
    }
    logCount = min(PERSISTENCY_LOG_ADDRESS_COUNT - 1, logCount + 1);
    EEPROM.write(511, (logIndex << 4) | logCount);
    EEPROM.commit();
}