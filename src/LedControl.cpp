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

int mapValRes(int val, uint8_t in_res) {
    return map(
        val,
        0, 2 << (in_res  - 1),
        0, 2 << (ANALRES - 1)
    );
}

void copyState(PatternState& from, PatternState& to) {
        to.interupt     = false;
        to.staticSet    = false;
        to.isStatic     = from.isStatic;
        to.patern       = from.patern;
        to.curColor     = from.curColor;
        to.curColorPlay = millis() + to.patern.pauseTimes[state.curColor];
}

void clearStateMem(PatternState& normState) {
    delete normState.patern.colors;
    delete normState.patern.pauseTimes;
    delete normState.patern.transTimes;
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
    setLeds({8, 0, 0, 0});
}

/******************
*  Pattern Change *
*******************/

void setStaticColor(Color color) {
    PatternState& normState = state.interupt ? stateBuffer : state;

    // releasing old memory
    clearStateMem(normState);

    // setting up the pattern
    normState.patern.length = 1;
    normState.patern.colors = new Color[1] {color,};
    normState.patern.pauseTimes = new unsigned long[1] {0,};
    normState.patern.transTimes = new unsigned long[1] {0,};
    
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
    clearStateMem(normState);

    // setting up the pattern
    normState.patern = pattern;
    
    // resetting the state
    normState.curColor = 0;
    normState.curColorPlay = millis() + normState.patern.pauseTimes[0];
    normState.isStatic = false;
    normState.interupt = false;
    normState.staticSet = false;
}

void setInteruptColor(ColorPattern pattern) {
    if(!state.interupt){ copyState(state, stateBuffer); }

    // setting up the pattern
    state.patern = pattern;
    
    // resetting the state
    state.curColor = 0;
    state.curColorPlay = millis() + state.patern.pauseTimes[0];
    state.isStatic = false;
    state.interupt = true;
    state.staticSet = false;
}

void setDefaultState() {
    setDynamicColor({
        3, 
        new Color[3] {{10, 512, 0, 0}, {10, 0, 512, 0}, {10, 0, 0, 512}},
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

    unsigned long changeTime = state.curColorPlay + state.patern.transTimes[state.curColor];

    // The state is has not reach the next color
    if(changeTime > millis()) { return; }

    // Change back from interupt 
    if((state.curColor + 1) == state.patern.length && state.interupt) {
        clearStateMem(state);
        copyState(stateBuffer, state);
        return;
    }

    // updating the curent color
    state.curColor = ((state.curColor + 1) == state.patern.length) ? 0 : state.curColor + 1;

    // seting the next play time
    state.curColorPlay = changeTime + state.patern.pauseTimes[state.curColor];
}

Color getTransColor(long now){
    uint8_t curColorIndex  = state.curColor;
    // If the current color is the last one, the next one should be the first
    uint8_t nextColorIndex = ((state.curColor + 1) == state.patern.length) ? 0 : state.curColor + 1;

    Color curColor  = state.patern.colors[curColorIndex];
    Color nextColor = state.patern.colors[nextColorIndex];

    // The color ratio between the curent and next color: (now - transition change) / transition time
    double timeDist = ((double)now - (double)state.curColorPlay) / (double)state.patern.transTimes[curColorIndex];
    
    // change the color in a linear maner: cur + (ratio * (next - cur))
    Color midColor = {
        curColor.res,
        curColor.r + (long)(timeDist * ((double)nextColor.r - (double)curColor.r)),
        curColor.g + (long)(timeDist * ((double)nextColor.g - (double)curColor.g)),
        curColor.b + (long)(timeDist * ((double)nextColor.b - (double)curColor.b)),
    };

    return midColor;
}

void applyState() {
    // If the state is static and already set or the power is off, don't do anything
    if(state.staticSet || !state.power) { return; }
    
    // If the state is static and not set yet, set the color and return
    if(state.isStatic ) { 
        setLeds(state.patern.colors[0]); 
        state.staticSet = true;
        return;
    }

    long now = millis();

    // The state is in a pause, set to the corect color and return
    if(state.curColorPlay > now) { 
        setLeds(state.patern.colors[state.curColor]);
        return;
    }

    // Set the led color to the correct transition color
    setLeds(getTransColor(now));
}

/******************
* External Utils  *
*******************/

String getStateJson(){
    ColorPattern pattern = state.patern;

    String message("{\"pattern\": [");
    for (uint8_t i = 0; i < pattern.length; i++) {
        Color color = pattern.colors[i];
        
        message = (message +
            "{\"color\": {\"r\":" + color.r + ", \"g\":" + color.g + ", \"b\":" + color.b + ", \"resolution\":" + color.res + "}," +
            "\"pause\": " + pattern.pauseTimes[i] + "," +
            "\"transition\": " + pattern.transTimes[i] + "}"
        );

        if(i < pattern.length - 1){ message += ",";}
    }
    message += "],";

    message = message + "\"static\": " + state.isStatic + ",";
    message = message + "\"interupt\": " + state.interupt + ",";
    message = message + "\"curColor\": " + state.curColor + "}";

    return message;
}

String getPowerJson() { return String() + "{\"power\":" + state.power + "}"; }

bool getPower() { return state.power; }

void setLeds(Color color) {
    analogWrite(R_PIN, mapValRes(color.r, color.res));
    analogWrite(G_PIN, mapValRes(color.g, color.res));
    analogWrite(B_PIN, mapValRes(color.b, color.res));
}
