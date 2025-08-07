#pragma once

#include <Arduino.h>

#define R_PIN  8
#define G_PIN 24
#define B_PIN 26

#define ANALRES 10
#define ANALFRQ 100

struct Color {
    uint8_t res;
    long r;
    long g;
    long b;
};

struct ColorPattern {
    uint8_t length;
    Color* colors;
    unsigned long* pauseTimes;
    unsigned long* transTimes;
};

struct PatternState {
    bool interupt;         // If interupt is true, once the pattern is finished, the old one will restart
    bool isStatic;         // If isStatic is true, the pattern is a static color and does not need to progress
    bool staticSet;        // If staticSet is true, the right color is already set. no need to aplly it each loop
    
    ColorPattern patern;   // The partern information about the currently running pattern

    uint8_t curColor;      // The cenrent color step
    unsigned long curColorPlay;     // The time at which the next transition should start / started
};

void setLeds(Color color);

void setStaticColor(Color color);
void setDynamicColor(ColorPattern color);
void setInterruptColor(ColorPattern color);
void setDefaultState();

double getTimeDist(long now);
String getStateString();

void updateState();
void applyState();