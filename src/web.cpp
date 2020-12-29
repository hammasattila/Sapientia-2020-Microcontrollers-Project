#include "web.h"

WiFiServer server(80);

void handleHttpRequest() {
    static unsigned long currentTime, previousTime;
    static bool isEndOfRequest = false;
    static String header;

    WiFiClient client = server.available();

    if (client) {
        previousTime = currentTime = millis();
        while (client.connected() && currentTime - previousTime <= WEB_SERVER_TIMEOUT) {
            currentTime = millis();
            if (client.available()) {
                char c = client.read();
                header += c;
                if (c == '\n') {
                    if (isEndOfRequest) {
                        if (0 <= header.indexOf("GET / HTTP/1.1")) {
                            serveWebPage(client);
                        } else if (0 <= header.indexOf("GET /api?add=")) {
                            serveWebPage(client);
                        }

                        break;
                    } else {
                        isEndOfRequest = true;
                    }
                } else if (c != '\r') {
                    isEndOfRequest = false;
                }
            }
        }
        client.stop();

        Serial.println(header);
        header = "";
    }
}

void serveWebPage(WiFiClient &client) {
    client.println(F("HTTP/1.1 200 OK\r\nContent-type:text/html\r\nContent-type:text/html\r\nConnection: close\r\n"));

    // Display the HTML web page
    client.println(F("<!DOCTYPE html>"
                     "<html>"
                     "<head>"
                     "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
                     "<link rel=\"icon\" href=\"data:,\">"
                     "</head>"
                     "<body>"
                     "<h1>ESP32 Web Server</h1>"
                     "</body>"
                     "</html>"));
    client.println();
}

