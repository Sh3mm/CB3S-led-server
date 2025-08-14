#pragma once

#include <Arduino.h>
#include <WebServer.h>

#include "LedControl.h"

// The server is defined in main.cpp
extern WebServer  server;
extern WiFiServer socketServ;

void postOn();
void postOff();
void postBrightness();

void postWhite();
void postStaticColor();
void postDynamicColor();
void postInteruptColor();

void getState();
void getPowerState();
void getBrightnessState();
