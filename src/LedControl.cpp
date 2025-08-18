#include "LedControl.h"

/******************
*  Sate Machines  *
*******************/

PatternState state;
PatternState stateBuffer;


/******************
* Internal Utils  *
*******************/

template<typename T> T clamp(T val, T mn, T mx){ return std::max(std::min(val, mx), mn);}

int _mapValRes(int val, uint8_t in_res) {
    return map(
        val,
        0, 2 << (in_res  - 1),
        0, 2 << (ANALRES - 1)
    );
}

int _mapValBrightness(int val, uint8_t brightness) {
    return map(brightness, 0, 100, 0, val);
}

void _copyState(PatternState& from, PatternState& to) {
        to.interupt     = false;
        to.staticSet    = false;
        to.isStatic     = from.isStatic;
        to.pattern       = from.pattern;
        to.curColor     = from.curColor;
        to.curColorPlay = millis() + to.pattern.pauseTimes[state.curColor];
}

void _clearStateMem(PatternState& normState) {
    delete normState.pattern.colors;
    delete normState.pattern.pauseTimes;
    delete normState.pattern.transTimes;
}

void _setLeds(Color color, bool brightness=true) {
    analogWrite(R_PIN, _mapValBrightness(_mapValRes(color.r, color.res), brightness ? state.brightness: 100));
    analogWrite(G_PIN, _mapValBrightness(_mapValRes(color.g, color.res), brightness ? state.brightness: 100));
    analogWrite(B_PIN, _mapValBrightness(_mapValRes(color.b, color.res), brightness ? state.brightness: 100));
}

/******************
*  Power Option   *
*******************/

void setLedOn(){
    state.power = true;
    // change the 
    state.curColorPlay = millis();
}

void setLedOff(){
    state.power = false;
    state.staticSet = false;
    _setLeds({8, 0, 0, 0});
}

void setBrightness(uint8_t brightness){
    PatternState& normState = state.interupt ? stateBuffer : state;
    normState.brightness = brightness;
    normState.staticSet = false;
}

/******************
*  Pattern Change *
*******************/

void setStaticColor(Color color) {
    PatternState& normState = state.interupt ? stateBuffer : state;

    // releasing old memory
    _clearStateMem(normState);

    // setting up the pattern
    normState.pattern.length = 1;
    normState.pattern.colors = new Color[1] {color,};
    normState.pattern.pauseTimes = new unsigned long[1] {0,};
    normState.pattern.transTimes = new unsigned long[1] {0,};
    
    // resetting the state
    normState.curColor = 0;
    normState.curColorPlay = 0;
    normState.isStatic = true;
    normState.interupt = false;
    normState.staticSet = false;
}

void setDynamicColor(ColorPattern pattern) {
    PatternState& normState = state.interupt ? stateBuffer : state;

    // releasing old memory
    _clearStateMem(normState);

    // setting up the pattern
    normState.pattern = pattern;
    
    // resetting the state
    normState.curColor = 0;
    normState.curColorPlay = millis() + normState.pattern.pauseTimes[0];
    normState.isStatic = false;
    normState.interupt = false;
    normState.staticSet = false;
}

void setInteruptColor(ColorPattern pattern) {
    if(!state.interupt){ _copyState(state, stateBuffer); }

    // setting up the pattern
    state.pattern = pattern;
    
    // resetting the state
    state.curColor = 0;
    state.curColorPlay = millis() + state.pattern.pauseTimes[0];
    state.isStatic = false;
    state.interupt = true;
    state.staticSet = false;
}

void setDefaultColorState() {
    setDynamicColor({
        3, 
        new Color[3] {{10, 256, 0, 0}, {10, 0, 256, 0}, {10, 0, 0, 256}},
        new unsigned long[3] {1000, 1000, 1000},
        new unsigned long[3] {2000, 2000, 2000}
    });
}

/******************
*    Apply State  *
*******************/

void updateState() {
    // The state cannot be updated is it's a static pattern
    if(state.isStatic) { return; }

    unsigned long changeTime = state.curColorPlay + state.pattern.transTimes[state.curColor];

    // The state is has not reach the next color
    if(changeTime > millis()) { return; }

    // Change back from interupt 
    if((state.curColor + 1) == state.pattern.length && state.interupt) {
        _clearStateMem(state);
        _copyState(stateBuffer, state);
        return;
    }

    // updating the curent color
    state.curColor = ((state.curColor + 1) == state.pattern.length) ? 0 : state.curColor + 1;

    // seting the next play time
    state.curColorPlay = changeTime + state.pattern.pauseTimes[state.curColor];
}

Color getTransColor(long now){
    uint8_t curColorIndex  = state.curColor;
    // If the current color is the last one, the next one should be the first
    uint8_t nextColorIndex = ((state.curColor + 1) == state.pattern.length) ? 0 : state.curColor + 1;

    Color curColor  = state.pattern.colors[curColorIndex];
    Color nextColor = state.pattern.colors[nextColorIndex];

    unsigned long beg = state.curColorPlay;
    unsigned long end = state.curColorPlay + state.pattern.transTimes[curColorIndex];

    Color midColor = {
        curColor.res,
        map(now, beg, end, curColor.r, nextColor.r),
        map(now, beg, end, curColor.g, nextColor.g),
        map(now, beg, end, curColor.b, nextColor.b),
    };

    return midColor;
}

void applyState() {
    // If the state is static and already set or the power is off, don't do anything
    if(state.staticSet || !state.power) { return; }
    
    // If the state is static and not set yet, set the color and return
    if(state.isStatic ) { 
        _setLeds(state.pattern.colors[0]); 
        state.staticSet = true;
        return;
    }

    long now = millis();

    // The state is in a pause, set to the corect color and return
    if(state.curColorPlay > now) { 
        _setLeds(state.pattern.colors[state.curColor]);
        return;
    }

    // Set the led color to the correct transition color
    _setLeds(getTransColor(now));
}

/******************
* External Utils  *
*******************/

String getStateJson(){
    ColorPattern pattern = state.pattern;

    String message("{\"pattern\": [");
    for (uint8_t i = 0; i < pattern.length; i++) {
        Color color = pattern.colors[i];
        
        message = (message +
            "{\"color\":" + color.json() + ',' +
            "\"pause\": " + pattern.pauseTimes[i] + "," +
            "\"transition\": " + pattern.transTimes[i] + "}"
        );

        if(i < pattern.length - 1){ message += ",";}
    }
    message += "],";

    message = message + "\"brightness\":  \"" + state.brightness + "%\",";
    message = message + "\"static\": " + state.isStatic + ",";
    message = message + "\"interupt\": " + state.interupt + ",";
    message = message + "\"curColor\": " + state.curColor + "}";

    return message;
}

String getColorJson(){
    Color color = state.pattern.colors[state.curColor];
    int r = _mapValBrightness(_mapValRes(color.r, color.res), state.brightness);
    int g = _mapValBrightness(_mapValRes(color.g, color.res), state.brightness);
    int b = _mapValBrightness(_mapValRes(color.b, color.res), state.brightness);
    return color.json();
}

String getPowerJson() { return String() + "{\"power\":" + state.power + "}"; }

String getBrightnessJson() { return String() + "{\"brightness\":" + state.brightness + "}"; }


bool getPower() { return state.power; }

void setLeds(Color color) {
    if(state.power) _setLeds(color);
}

void setLeds(Color color, bool brightness) {
    if(state.power) _setLeds(color, brightness);
}

void resetStatic(){
    state.staticSet = false;
}