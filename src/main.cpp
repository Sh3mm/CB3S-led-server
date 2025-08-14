#include "main.h"

// HTTP Server
WebServer server(80);

Button2 onOffButton;

void buttonClick(Button2& b){
    if(getPower()) { setLedOff(); }
    else           { setLedOn();  }
}

void buttonLongClick(Button2& b){
    setup();
}

void setup() {
    // PWM Variables Setup
    analogWriteFrequency(ANALFRQ);
    analogWriteResolution(ANALRES);

    // LED setup
    setBrightness(100);
    setLedOn();

    // Initial "No Connection" Color
    setLeds({10, 256, 0, 0});
    
    // Connection to Wifi
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED) { delay(500); }

    // GET Requests Callbacks
    server.on("/state",      HTTP_GET, getState);
    server.on("/power",      HTTP_GET, getPowerState);
    server.on("/brightness", HTTP_GET, getBrightnessState);

    // POST Requests Callbacks (Power control)
    server.on("/on",         HTTP_POST, postOn);
    server.on("/off",        HTTP_POST, postOff);
    server.on("/brightness", HTTP_POST, postBrightness);

    // POST Requests Callbacks (Specific Colors)
    server.on("/white",          HTTP_POST, postWhite);
    
    // POST Requests Callbacks (LED Control)
    server.on("/static_color",   HTTP_POST, postStaticColor);
    server.on("/dynamic_color",  HTTP_POST, postDynamicColor);
    server.on("/interupt_color", HTTP_POST, postInteruptColor);

    // HTTP Server Startup
    server.begin();

    // Button setup
    onOffButton.setTapHandler(buttonClick);
    onOffButton.setLongClickDetectedHandler(buttonLongClick);
    onOffButton.setLongClickTime(LONG_CLICK); // setting the long click time
    
    onOffButton.begin(BUTTON_PIN);

    // Seting default colors
    setDefaultState();
}


void loop() {
    // HTTP Updates
    server.handleClient();
    
    // Color Change
    updateState();
    applyState();

    // Button check
    onOffButton.loop();
}
