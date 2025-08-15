# include "endpoints.h"

long getLongParam(String name, long defaultVal = 0) {
    String val = server.arg(name);
    // If the value is absent return the default
    if(val == ""  ) { return defaultVal; }
    // Return 0 for the special case '0' since toInt will also give 0 in case of an error
    if(val == "0" ) { return 0         ; }
    
    long intVal = val.toInt();
    // If the value doesn't have numbers, return the defaut value
    if(intVal == 0) { return defaultVal; }
    
    // return the value
    return intVal;
}

void postOn() {
    setLedOn();
    server.send(200, "text/plain", getPowerJson());
}

void postOff() {
    setLedOff();
    server.send(200, "text/plain", getPowerJson());
}

void postBrightness(){
    if(!server.hasArg("brightness")){
        server.send(400, "text/plain", "Missing the 'brightness' parameter");
        return;
    }

    uint8_t strength = server.arg("brightness").toInt();
    if(0 > strength || strength > 100) {
        server.send(400, "text/plain", "The 'brightness' parameter must be between 0 and 100");
        return;
    }

    setBrightness(strength);
    server.send(200, "application/json", getBrightnessJson());
}

void postNamedColor(){
    if (!server.hasArg("color")) {
        server.send(400, "text/plain", "missing 'color' parameter");
        return;
    }

    String name = server.arg("color");
    // Color by name
    if     (name == "white"  ) { setStaticColor({10, 1024,  461,  154}); } 
    else if(name == "cyan"   ) { setStaticColor({10,    0, 1024,  342}); } 
    else if(name == "magenta") { setStaticColor({10, 1024,    0,  154}); } 
    else if(name == "yellow" ) { setStaticColor({10, 1024,  461,    0}); } 
    else if(name == "red"    ) { setStaticColor({10, 1024,    0,    0}); } 
    else if(name == "green"  ) { setStaticColor({10,    0, 1024,    0}); } 
    else if(name == "blue"   ) { setStaticColor({10,    0,    0, 1024}); } 
    else { // If the color is not part of the choices return an error
        server.send(400, "text/plain", "Unknown color");
        return;
    }
    server.send(200, "application/json", getStateJson());
}

void postStaticColor() {

    Color color;

    color.r   = getLongParam("R");
    color.g   = getLongParam("G");
    color.b   = getLongParam("B");
    color.res = getLongParam("res", 8);

    setStaticColor(color);
    server.send(200, "application/json", getStateJson());
}

ColorPattern extractPattern() {
    uint8_t colorNum   = server.arg("colorNum").toInt();
    uint8_t resolution = server.hasArg("res") ? server.arg("res").toInt() : 8;

    ColorPattern pattern;
    pattern.length = colorNum;
    pattern.colors = new Color[colorNum];
    pattern.pauseTimes = new unsigned long[colorNum];
    pattern.transTimes = new unsigned long[colorNum];

    for (uint8_t i = 0; i < pattern.length; i++) {
        pattern.colors[i].r   = getLongParam(String("R") + i, 0);
        pattern.colors[i].g   = getLongParam(String("G") + i, 0);
        pattern.colors[i].b   = getLongParam(String("B") + i, 0);
        pattern.colors[i].res = resolution;

        pattern.pauseTimes[i] = getLongParam(String("pause") + i, 0);
        pattern.transTimes[i] = getLongParam(String("trans") + i, 0);
    }
    
    return pattern;
}

void postDynamicColor() {
    if (!server.hasArg("colorNum")){
        server.send(400, "text/plain", "missing 'colorNum' parameter");
        return;
    }
    
    uint8_t colorNum = server.arg("colorNum").toInt();
    if (colorNum == 0){
        server.send(400, "text/plain", "the value of the 'colorNum' parameter is not valid");
        return;
    }

    setDynamicColor(extractPattern());
    server.send(200, "application/json", getStateJson());
}

void postInteruptColor() {
    if (!server.hasArg("colorNum")){
        server.send(400, "text/plain", "missing 'colorNum' parameter");
        return;
    }
    
    uint8_t colorNum = server.arg("colorNum").toInt();
    if (colorNum == 0){
        server.send(400, "text/plain", "the value of the 'colorNum' parameter is not valid");
        return;
    }

    setInteruptColor(extractPattern());
    server.send(200, "application/json", getStateJson());
}

void postDefaultColor(){
    setDefaultState();
    server.send(200, "application/json", getStateJson());
}


void getState() {
    server.send(200, "application/json", getStateJson());
}

void getColor() {
    server.send(200, "application/json", getColorJson());
}

void getPowerState() {
    server.send(200, "application/json", getPowerJson());
}

void getBrightnessState() {
    server.send(200, "application/json", getBrightnessJson());
}