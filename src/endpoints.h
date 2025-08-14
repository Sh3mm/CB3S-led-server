#pragma once

#include <Arduino.h>
#include <WebServer.h>

#include "LedControl.h"

// The server is defined in main.cpp
extern WebServer server;

void postOn();
void postOff();
void postWhite();
void postStaticColor();
void postDynamicColor();
void postInteruptColor();

void getState();
void getPowerState();
