# Sapientia-Microcontrollers2020-Project

Durin Microcontrollers Cuorse my project was an RFID Autorization System.

## Configration

Create an *EnviromentVariables.h* header file in your include directory. This should contain the folowing enviroment definitions:

```c++
#define WIFI_SSID "<wifi ssid name here>"
#define WIFI_PSWD "<wifi pasword here>"

const byte P_CHIP_SELECT_SPI = GPIO_NUM_15;
const byte P_RFID_RESET = GPIO_NUM_27;
```
