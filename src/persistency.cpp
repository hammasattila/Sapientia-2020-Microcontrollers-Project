#include "persistency.h"

#define PERSISTENCY_LOG_KEY_COUNT 40
#define PERSISTENCY_LOG_ADDRESS_COUNT 16
#define PERSISTENCY_LOG_ADDRESS_START 8 * PERSISTENCY_LOG_KEY_COUNT

String byteToHex(const uint8_t b) {
    String res;
    char c;
    c = (b >> 4) + '0';
    if ('9' < c) {
        c += 'A' - '9';
    }
    res += c;
    c = (b & 0x0F) + '0';
    if ('9' < c) {
        c += 'A' - '9';
    }
    res += c;

    return res;
}

String getKeysJson() {
    const uint8_t keyCount = EEPROM.read(510);
    String res = "{\"keys\": [";

    for (uint8_t i = 0; i < keyCount; ++i) {
        uint16_t address = i * 8;
        uint8_t keyLength = EEPROM.read(address++);
        res += F("{\"key\":\"");

        for (uint8_t j = 0; j < keyLength; ++j, ++address) {
            const uint8_t b = EEPROM.read(address);
            res += byteToHex(b);
            if (j < keyLength - 1) {
                res += ":";
            }
        }
        res += (keyCount <= i + 1 ? F("\"}") : F("\"},"));
    }
    res += "]}";

    return res;
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

    Serial.println(keyCount);
    Serial.println(keyCount + 1);
    EEPROM.write(510, keyCount + 1);
    EEPROM.commit();
    Serial.println(EEPROM.read(510));

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

String getLogsJson() {
    const uint8_t logState = EEPROM.read(511);
    uint8_t logCount = logState & 0x0F;
    uint8_t logIndex = ((logCount == PERSISTENCY_LOG_ADDRESS_COUNT - 1) ? (logState >> 4) & 0x0F : 0);
    if (logIndex) {
        ++logCount;
    }
    String res = "{\"logs\": [";

    for (uint8_t i = 0; i < logCount; ++i, ++logIndex) {
        if (PERSISTENCY_LOG_ADDRESS_COUNT <= logIndex) {
            logIndex = logIndex % PERSISTENCY_LOG_ADDRESS_COUNT;
        }
        uint16_t address = logIndex * 8 + PERSISTENCY_LOG_ADDRESS_START;
        uint8_t keyLength = EEPROM.read(address++);
        res += (keyLength & 0x80 ? F("{\"accepted\": true,") : F("{\"accepted\": false,"));
        keyLength &= 0x7F;
        res += "\"key\":\"";
        for (uint8_t j = 0; j < keyLength; ++j, ++address) {
            const uint8_t b = EEPROM.read(address);

            res += byteToHex(b);
            if (j < keyLength - 1) {
                res += ":";
            }
        }
        res += (logCount <= i + 1 ? F("\"}") : F("\"},"));
    }
    res += F("]}");

    return res;
}

void saveLog(MFRC522::Uid *uid) {
    const uint8_t logState = EEPROM.read(511);
    uint8_t logCount = logState & 0x0F;
    uint8_t logIndex = (logState >> 4) & 0x0F;

    uint16_t address = logIndex * 8 + PERSISTENCY_LOG_ADDRESS_START;
    EEPROM.write(address, uid->size | (findKey(uid) != UINT8_MAX) << 7);
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