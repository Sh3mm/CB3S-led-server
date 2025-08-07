#pragma once

#include <Arduino.h>
#include <WebServer.h>

#include "LedControl.h"

extern WebServer server;

void postStaticColor();
void postDynamicColor();
void getState();