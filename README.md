# Local Wifi Led strip

Project made to adapt a Genni "Smart" led strip that required an app and a cloud connection into a local only Wifi led strip that communicates through an HTTP server hosted on the chip.

The chip is a [CB3S from tuya](https://docs.libretiny.eu/boards/cb3s/). 

The code depends on [libretiny](https://docs.libretiny.eu/).


## Set-up

1. To setup, enter your wifi's SSID and password in the [main.h file](src/main.h). 

2. Upload the project to the chip using [platformIO](https://platformio.org/)