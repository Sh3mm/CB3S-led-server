//
// Created by sh3mm on 8/4/25.
//
#include "main.h"

WebServer server(80);

void setup() {
    Serial.begin(9600);
    analogWriteFrequency(ANALFRQ);
    analogWriteResolution(ANALRES);

    setLeds({8, 255, 0, 0});
    
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    while (WiFi.status() != WL_CONNECTED) { delay(500); }

    server.on("/static_color", HTTP_POST, postStaticColor);
    server.on("/dynamic_color", HTTP_POST, postDynamicColor);
    server.on("/state", HTTP_GET, getState);

    server.begin();

    setDefaultState();
}


void loop() {
    server.handleClient();
    updateState();
    applyState();
}
