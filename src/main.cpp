#include "main.h"

// HTTP Server
WebServer server(80);
WiFiServer socketServer(1221, 1);

Button2 onOffButton;

void buttonClick(Button2& b){
    if(getPower()) { setLedOff(); }
    else           { setLedOn();  }
}

void buttonLongClick(Button2& b){
    setup(true);
}

void httpServerSetup(){
    // GET Requests Callbacks
    server.on("/state",      HTTP_GET, getState);
    server.on("/color",      HTTP_GET, getColor);
    server.on("/power",      HTTP_GET, getPowerState);
    server.on("/brightness", HTTP_GET, getBrightnessState);

    // GET request Tcp Socket connection
    server.on("/socket", HTTP_GET, getSocketConnection);

    // POST Requests Callbacks (Power control)
    server.on("/on",         HTTP_POST, postOn);
    server.on("/off",        HTTP_POST, postOff);
    server.on("/brightness", HTTP_POST, postBrightness);

    // POST Requests Callbacks (LED Control)
    server.on("/color/byname",   HTTP_POST, postNamedColor);
    server.on("/color/static",   HTTP_POST, postStaticColor);
    server.on("/color/dynamic",  HTTP_POST, postDynamicColor);
    server.on("/color/interupt", HTTP_POST, postInteruptColor);
    server.on("/color/default",  HTTP_POST, postDefaultColor);

    // HTTP Server Startup
    server.begin();
}

void setup(){ setup(false); }

void setup(bool reset) {
    // PWM Variables Setup
    analogWriteFrequency(ANALFRQ);
    analogWriteResolution(ANALRES);

    // LED setup
    setBrightness(100);
    setLedOn();

    // Initial "No Connection" Color
    setLeds({10, 256, 0, 0});
    
    if(!reset){
        // Connection to Wifi
        WiFi.begin(WIFI_SSID, WIFI_PASS);
        while (WiFi.status() != WL_CONNECTED) { delay(500); }

        httpServerSetup();
        socketServer.begin();
    }

    // Button setup
    onOffButton.setTapHandler(buttonClick);
    onOffButton.setLongClickDetectedHandler(buttonLongClick);
    onOffButton.setLongClickTime(LONG_CLICK); // setting the long click time
    
    onOffButton.begin(BUTTON_PIN);

    // Seting colors & socket states
    setDefaultColorState();
    setDefaultSocketState();
}


void loop() {
    if(!inSocketMode()){
        // HTTP Updates
        server.handleClient();

        // Color Change
        updateState();
        applyState();
    } else {
        // Socket mode
        handleSocket();
    }
 
    // Button check
    onOffButton.loop();
}
