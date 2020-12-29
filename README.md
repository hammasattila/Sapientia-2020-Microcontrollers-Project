# Sapientia-Microcontrollers2020-Project

Durin Microcontrollers Cuorse my project was an RFID Autorization System.

## Configration

Create an *EnviromentVariables.h* header file in your include directory. This should contain the folowing enviroment definitions:

```C++
#define WIFI_SSID "<wifi ssid name here>"
#define WIFI_PSWD "<wifi pasword here>"
```


```C++
void handleHttpRequest() {
    static unsigned long currentTime, previousTime;
    static String header;

    WiFiClient client = server.available(); // Listen for incoming clients

    if (client) {
        if (client.available()) {
            header = client.readString();
            // Serial.println(header);

            client.println(F("HTTP/1.1 200 OK"));
            client.println(F("Content-type:text/html\r\nConnection: close\r\n"));
            client.println(F("<!DOCTYPE html>"
                             "<html>"
                             "<head>"
                             "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
                             "<link rel=\"icon\" href=\"data:,\">"
                             "</head>"
                             "<body></body>"));

            //     char c = client.read();

            //     Serial.write(c);
            //     header += c;
            //     if (c == '\n') {
            //         if (currentLine.length() == 0) {
            //             client.println("HTTP/1.1 200 OK\r\nContent-type:text/html\r\nConnection: close\r\n");

            //             // Display the HTML web page
            //             client.println("<!DOCTYPE html><html>"
            //             "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
            //             "<link rel=\"icon\" href=\"data:,\">");

            //             // CSS to style the on/off buttons
            //             // Feel free to change the background-color and font-size attributes to fit your preferences
            //             client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            //             client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            //             client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            //             client.println(".button2 {background-color: #555555;}</style></head>");

            //             // Web Page Heading
            //             client.println("<body><h1>ESP32 Web Server</h1>");

            //             client.println("</body></html>");

            //             // The HTTP response ends with another blank line
            //             client.println();
            //             // Break out of the while loop
            //             break;
            //         } else { // if you got a newline, then clear currentLine
            //             currentLine = "";
            //         }
            //     } else if (c != '\r') { // if you got anything else but a carriage return character,
            //         currentLine += c;   // add it to the end of the currentLine
            //     }
        }

        client.stop();
        Serial.println(F("Client disconnected.\r\n"));
    }
}
```