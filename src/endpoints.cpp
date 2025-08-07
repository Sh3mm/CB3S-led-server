# include "endpoints.h"

long getLongParam(String name, long defaultVal = 0) {

    String val = server.arg(name);
    if(val == ""  ) { return defaultVal; }
    if(val == "0" ) { return 0         ; }
    
    long intVal = val.toInt();
    if(intVal == 0) { return defaultVal; }
    
    return intVal;
}

void postStaticColor() {

    Color color;

    color.r   = getLongParam("R");
    color.g   = getLongParam("G");
    color.b   = getLongParam("B");
    color.res = getLongParam("res", 8);

    setStaticColor(color);
    server.send(200, "text/plain", getStateString());
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
    
    setDynamicColor(pattern);
    server.send(200, "text/plain", getStateString());
}


void getState() {
    server.send(200, "text/plain", getStateString());
}