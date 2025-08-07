# Local Wifi Led strip

Project made to adapt a Genni "Smart" led strip that required an app and a cloud connection into a local only Wifi led strip that communicates through an HTTP server hosted on the chip.

The chip is a [CB3S from tuya](https://docs.libretiny.eu/boards/cb3s/). 

The code depends on [libretiny](https://docs.libretiny.eu/).


## Set-up

1. To setup, Fisrt create a new `varriables.h` file in the `src` folder and fill it with the following information:
```c
// WiFi configs
#define WIFI_SSID "XXXX"
#define WIFI_PASS "XXXX"

// Pin configs (the default for my project)
#define BUTTON_PIN  9
#define R_PIN       8
#define G_PIN      24
#define B_PIN      26

// PWM configs
#define ANALRES 10
#define ANALFRQ 100
```  

2. Upload the project to the chip using [platformIO](https://platformio.org/)