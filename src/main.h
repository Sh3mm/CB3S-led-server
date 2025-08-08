#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Button2.h>

#include "LedControl.h"
#include "endpoints.h"

#include "variables.h"

void buttonClick(Button2& b);
void buttonLongClick(Button2& b);
void setup();
void loop();