#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Button2.h>
#include <WiFiServer.h>

#include "LedControl.h"
#include "endpoints.h"
#include "socket.h"

#include "variables.h"

void buttonClick(Button2& b);
void buttonLongClick(Button2& b);
void setup(bool reset);
void setup();
void loop();