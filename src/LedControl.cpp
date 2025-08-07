#include "LedControl.h"


PatternState state;
PatternState stateBuffer;


template<typename T> T clamp(T val, T mn, T mx){ return std::max(std::min(val, mx), mn);}

int mapValRes(int val, uint8_t in_res) {
    return map(
        val, 
        0, 2 << (in_res  - 1), 
        0, 2 << (ANALRES - 1)
    );
}

void setLeds(Color color) {
    analogWrite(R_PIN, mapValRes(color.r, color.res));
    analogWrite(G_PIN, mapValRes(color.g, color.res));
    analogWrite(B_PIN, mapValRes(color.b, color.res));
}

void setStaticColor(Color color) {
    PatternState& normState = state.interupt ? stateBuffer : state;

    // releasing old memory
    delete normState.patern.colors;
    delete normState.patern.pauseTimes;
    delete normState.patern.transTimes;

    normState.patern.length = 1;
    normState.patern.colors = new Color[1] {color,};
    normState.patern.pauseTimes = new unsigned long[1] {0,};
    normState.patern.transTimes = new unsigned long[1] {0,};
    
    normState.curColor = 0;
    normState.curColorPlay = 0;
    normState.isStatic = true;
    normState.staticSet = false;
}

void setDynamicColor(ColorPattern pattern) {
    PatternState& normState = state.interupt ? stateBuffer : state;

    // releasing old memory
    delete normState.patern.colors;
    delete normState.patern.pauseTimes;
    delete normState.patern.transTimes;

    // setting up the pattern
    normState.patern = pattern;
    
    // resetting the state
    normState.curColor = 0;
    normState.curColorPlay = millis() + normState.patern.pauseTimes[0];
    normState.isStatic = false;
    normState.staticSet = false;

}

void setInterruptColor(ColorPattern color) {

}

double getTimeDist(long now){
    uint8_t curColorIndex  = state.curColor;
    uint8_t nextColorIndex = ((state.curColor + 1) == state.patern.length) ? 0 : state.curColor;

    Color curColor  = state.patern.colors[curColorIndex];
    Color nextColor = state.patern.colors[nextColorIndex];

    double timeDist = ((double)now - (double)state.curColorPlay) / (double)state.patern.transTimes[curColorIndex];
    return timeDist;
}

Color getMidColor(long now){
    uint8_t curColorIndex  = state.curColor;
    uint8_t nextColorIndex = ((state.curColor + 1) == state.patern.length) ? 0 : state.curColor + 1;

    Color curColor  = state.patern.colors[curColorIndex];
    Color nextColor = state.patern.colors[nextColorIndex];

    double timeDist = ((double)now - (double)state.curColorPlay) / (double)state.patern.transTimes[curColorIndex];
    Color midColor = {
        curColor.res,
        curColor.r + (long)(timeDist * ((double)nextColor.r - (double)curColor.r)),
        curColor.g + (long)(timeDist * ((double)nextColor.g - (double)curColor.g)),
        curColor.b + (long)(timeDist * ((double)nextColor.b - (double)curColor.b)),
    };

    return midColor;
}

void setDefaultState() {
    setStaticColor({8, 0, 255, 0});
    updateState();
    applyState();
}

void updateState() {
    // The state cannot be updated is it's a static pattern
    if(state.isStatic) { return; }

    unsigned long changeTime = state.curColorPlay + state.patern.transTimes[state.curColor];

    // The state is has not reach the next color
    if(changeTime > millis()) { return; }

    // updating the curent color
    state.curColor = ((state.curColor + 1) == state.patern.length) ? 0 : state.curColor + 1;

    // seting the next play time
    state.curColorPlay = changeTime + state.patern.pauseTimes[state.curColor];
}

void applyState() {
    if(state.staticSet) { return; }
    if(state.isStatic ) { 
        setLeds(state.patern.colors[0]); 
        state.staticSet = true;
        return;
    }

    long now = millis();

    // The state is in a pause
    if(state.curColorPlay > now) { 
        setLeds(state.patern.colors[state.curColor]);
        return;
    }

    Color midColor = getMidColor(now);

    setLeds(midColor);
}

String getStateString(){
    ColorPattern pattern = state.patern;

    String message("Pattern:\n");
    for (uint8_t i = 0; i < pattern.length; i++) {
        Color color = pattern.colors[i];
        String line = (
            String("Color set to: (") + color.r + ',' + color.g + ',' + color.b + ',' + color.res + "b)" +
            "\nPause for: " + pattern.pauseTimes[i] + "ms" +
            "\nTransitioning for: " + pattern.transTimes[i] + "ms"
        );
        message += line + '\n';
    }

    message += "\n==============\n";
    message = message + "Static: " + state.isStatic + "\n";
    message = message + "StaticSet: " + state.staticSet + "\n\n";

    message = message + "interupt: " + state.interupt + "\n";
    message = message + "curColor: " + state.curColor + "\n";
    message = message + "curColorPlay: " + state.curColorPlay + "\n\n";

    message = message + "now: " + millis() + "\n";

    return message;
}