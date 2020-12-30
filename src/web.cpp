#include "web.h"
#include "persistency.h"

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
                        uint pos;
                        if (0 <= header.indexOf("GET / HTTP/1.1")) {
                            serveWebPage(client);
                        } else if (0 <= header.indexOf("GET /api/logs")) {
                            servLogs(client);
                        } else if (0 <= header.indexOf("GET /api/keys")) {
                            servKeys(client);
                        } else if (0 <= (pos = header.indexOf("GET /api/remove"))) {
                            String posStr = header.substring(pos, header.indexOf(" HTTP/1.1"));
                            servRemove(client, posStr.toInt());
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
    client.println(F("<html><head> <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"> <link rel=\"preconnect\" href=\"https://fonts.gstatic.com\"> <link href=\"https://fonts.googleapis.com/css2?family=Roboto&display=swap\" rel=\"stylesheet\"> <style>html{font-family: \"Roboto\";}.title{text-align: center;}.card{box-shadow: 0 1px 2px 0 rgba(0, 0, 0, .2), 0 2px 7px 0 rgba(0, 0, 0, .19); padding: 1rem; transition: box-shadow .5s;}.card:hover{box-shadow: 0 8px 17px 0 rgba(0, 0, 0, .2), 0 6px 20px 0 rgba(0, 0, 0, .19);}</style> <script>window.onload=()=>{console.debug(\"Window loaded.\"); fetch(\"api/logs\") .then(response=> response.json()) .then(data=>{for (const it of data.logs){const card=document.createElement(\"div\"); card.className=\"card\"; card.appendChild(document.createTextNode(it.key)); document.getElementById(\"log_container\").appendChild(card);}}); fetch(\"api/keys\") .then(response=> response.json()) .then(data=>{for (const [i, it] of data.keys.entries()){const card=document.createElement(\"div\"); card.className=\"card\"; card.appendChild(document.createTextNode(it.key)); const button=document.createElement(\"button\"); button.addEventListener(\"click\", event=>{fetch(`/api/remove/${i}`);}); card.appendChild(button); document.getElementById(\"key_container\").appendChild(card);}});}</script></head><body> <h1 class=\"title\">Acces Control</h1> <div id=\"key_container\" class=\"container card\"> <h4 class=\"title\">Keys</h4> </div><div id=\"log_container\" class=\"container card\"> <h4 class=\"title\">Logs</h4> </div></body></html>"));
    client.println();
}

void servKeys(WiFiClient &client) {
    beginJsonResponse(client);
    readKeysForClient(client);
}

void servLogs(WiFiClient &client) {
    beginJsonResponse(client);
    readLogsForClient(client);
}

void servRemove(WiFiClient &client, uint8_t pos) {
    beginJsonResponse(client);
    client.print("{\"response\":");
    client.print(removeKey(pos));
    client.println("}");
}