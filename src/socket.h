#pragma once

#include <Arduino.h>
#include <WiFiServer.h>
#include <WebServer.h>
#include "LedControl.h"

struct SocketState {
    WiFiClient socketClient;
    unsigned long timeout;
    bool colorSet;
    bool sockMode;
};

extern WebServer server;
extern WiFiServer socketServer;

void handleSocket();
bool inSocketMode();

void getSocketConnection();
void setDefaultSocketState();
