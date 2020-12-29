#ifndef WEB_H
#define WEB_H

#include <EnviromentVariables.h>
#include <WiFi.h>

extern WiFiServer server;

void handleHttpRequest();
void serveWebPage(WiFiClient &);

#endif