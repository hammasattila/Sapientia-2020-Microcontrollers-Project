#ifndef WEB_H
#define WEB_H

#include <EnviromentVariables.h>
#include <WiFi.h>

extern WiFiServer server;
extern bool addNextCard;

void handleHttpRequest();
void serveWebPage(WiFiClient &);
inline void beginJsonResponse(WiFiClient &client) {
    client.println(F("HTTP/1.1 200 OK\r\nContent-type:application/json\r\nConnection: close\r\n"));
}
void servKeys(WiFiClient &);
void servLogs(WiFiClient &);
void servAdd(WiFiClient &);
void servRemove(WiFiClient &, uint8_t );

#endif