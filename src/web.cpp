#include "web.h"
#include "persistency.h"

bool addNextCard = false;
AsyncWebServer server(80);

const char htmlIndex[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>

<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link rel="icon"
        href="data:">
    <link rel="preconnect" href="https://fonts.gstatic.com">
    <link href="https://fonts.googleapis.com/css2?family=Roboto&display=swap" rel="stylesheet">
    <style>
        html {
            font-family: "Roboto";
        }

        .title {
            text-align: center;
        }

        .card {
            box-shadow: 0 1px 2px 0 rgba(0, 0, 0, .2), 0 2px 7px 0 rgba(0, 0, 0, .19);
            padding: 1rem;
            transition: box-shadow .5s;
        }

        .card:hover {
            box-shadow: 0 8px 17px 0 rgba(0, 0, 0, .2), 0 6px 20px 0 rgba(0, 0, 0, .19);
        }

        #log_container {
            display: flex;
            flex-wrap: wrap;
            justify-content: center;
        }
    </style>
    <script>
        logs = () => {
            fetch("api/logs")
                .then(response => response.json())
                .then(data => {
                    for (const it of document.getElementsByClassName("log")) {
                        it.remove();
                    }

                    for (const it of data.logs) {
                        const card = document.createElement("div");
                        card.className = "card log";
                        card.appendChild(document.createTextNode(it.key));
                        document.getElementById("log_container").appendChild(card);
                    }
                });
        };

        keys = () => {
            fetch("api/keys")
                .then(response => response.json())
                .then(data => {
                    for (const it of document.getElementsByClassName("key")) {
                        it.remove();
                    }

                    for (const [i, it] of data.keys.entries()) {
                        const card = document.createElement("div");
                        card.className = "card key";
                        const p = document.createElement("p");
                        p.innerText = it.key;
                        card.appendChild(p);
                        const button = document.createElement("button");
                        button.appendChild(document.createTextNode("Delete"));
                        button.addEventListener("click", event => {
                            fetch(`/api/remove/${i}`);
                        });
                        card.appendChild(button);
                        document.getElementById("key_container").appendChild(card);
                    }
                });
        };

        window.onload = () => {
            console.debug("Window loaded.");
            logs();
            keys();
        };

        function addNextCard() {
            fetch("api/add")
                .then(response => response.json())
                .then(data => {
                    console.debug(data);
                });
        }

        setInterval(() => {
            keys();
            logs();
        }, 10000);
    </script>
</head>

<body>
    <h1 class="title">Acces Control</h1>
    <div id="key_container" class="container card">
        <h4 class="title">Keys</h4>
    </div>
    <button onClick="addNextCard();">Add the next card</button>
    <div class="container card">
        <h4 class="title">Logs</h4>
        <div id="log_container"></div>
    </div>
</body>

</html>
)rawliteral";

void setupAsyncWebServer() {
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        Serial.println("Got request.");
        request->send_P(200, "text/html", htmlIndex, nullptr);
    });

    server.begin();
}

void serveWebPage(WiFiClient &client) {
    client.println(F("HTTP/1.1 200 OK\r\nContent-type:text/html\r\nContent-type:text/html\r\nConnection: close\r\n"));

    // Display the HTML web page
    client.println(F("<html><head> <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"> <link rel=\"preconnect\" href=\"https://fonts.gstatic.com\"> <link href=\"https://fonts.googleapis.com/css2?family=Roboto&display=swap\" rel=\"stylesheet\"> <style>html{font-family: \"Roboto\";}.title{text-align: center;}.card{box-shadow: 0 1px 2px 0 rgba(0, 0, 0, .2), 0 2px 7px 0 rgba(0, 0, 0, .19); padding: 1rem; transition: box-shadow .5s;}.card:hover{box-shadow: 0 8px 17px 0 rgba(0, 0, 0, .2), 0 6px 20px 0 rgba(0, 0, 0, .19);}</style> <script>logs=()=>{fetch(\"api/logs\") .then(response=> response.json()) .then(data=>{for (const it of document.getElementsByClassName(\"log\")){it.remove();}for (const it of data.logs){const card=document.createElement(\"div\"); card.className=\"card log\"; card.appendChild(document.createTextNode(it.key)); document.getElementById(\"log_container\").appendChild(card);}});}; keys=()=>{fetch(\"api/keys\") .then(response=> response.json()) .then(data=>{for (const it of document.getElementsByClassName(\"key\")){it.remove();}for (const [i, it] of data.keys.entries()){const card=document.createElement(\"div\"); card.className=\"card key\"; const p=document.createElement(\"p\"); p.innerText=it.key; card.appendChild(p); const button=document.createElement(\"button\"); button.appendChild(document.createTextNode(\"Delete\")); button.addEventListener(\"click\", event=>{fetch(`/api/remove/${i}`);}); card.appendChild(button); document.getElementById(\"key_container\").appendChild(card);}});}; window.onload=()=>{console.debug(\"Window loaded.\"); logs(); keys();}; function addNextCard(){fetch(\"api/add\") .then(response=> response.json()) .then(data=>{console.debug(data);});}setInterval(()=>{keys(); logs();}, 3000); </script></head><body> <h1 class=\"title\">Acces Control</h1> <div id=\"key_container\" class=\"container card\"> <h4 class=\"title\">Keys</h4> </div><button onClick=\"addNextCard();\">Add the next card</button> <div id=\"log_container\" class=\"container card\"> <h4 class=\"title\">Logs</h4> </div></body></html>"));
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

void servAdd(WiFiClient &client) {
    addNextCard = true;
    beginJsonResponse(client);
    client.print("{\"response\":");
    client.print(addNextCard);
    client.println("}");
}

void servRemove(WiFiClient &client, uint8_t pos) {
    beginJsonResponse(client);
    client.print("{\"response\":");
    client.print(removeKey(pos));
    client.println("}");
}