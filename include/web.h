#ifndef WEB_H
#define WEB_H

#include <WiFi.h>
#include "ESPAsyncWebServer.h"

#include <EnviromentVariables.h>

extern bool addNextCard;
extern AsyncWebServer server;
extern const char htmlIndex[];

void setupAsyncWebServer();
void serveWebPage(WiFiClient &);
inline void beginJsonResponse(WiFiClient &client) {
    client.println(F("HTTP/1.1 200 OK\r\nContent-type:application/json\r\nConnection: close\r\n"));
}
void servAdd(WiFiClient &);
void servRemove(WiFiClient &, uint8_t );

#endif