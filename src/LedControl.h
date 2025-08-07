#pragma once

#include <Arduino.h>

#define R_PIN  8
#define G_PIN 24
#define B_PIN 26

#define ANALRES 10
#define ANALFRQ 100

struct Color {
    uint8_t res; // Resolution of the color in bits (8 -> 255, 10 -> 1024)
    long r;      // Red   componant
    long g;      // Green componant
    long b;      // Blue  componant
};

struct ColorPattern {
    uint8_t length;            // The number of color in each array
    Color* colors;             // The list of peak colors
    unsigned long* pauseTimes; // The amount of time to spent on each peak color
    unsigned long* transTimes; // The amount of time to spent transition between each peak color
};

struct PatternState {
    bool power;
    bool interupt;               // If interupt is true, once the pattern is finished, the old one will restart
    bool isStatic;               // If isStatic is true, the pattern is a static color and does not need to progress
    bool staticSet;              // If staticSet is true, the right color is already set. no need to aplly it each loop

    ColorPattern patern;         // The partern information about the currently running pattern

    uint8_t curColor;            // The cenrent color step
    unsigned long curColorPlay;  // The time at which the next transition should start / started
};

void setLedOn();
void setLedOff();

// Pattern Change
void setStaticColor(Color color);
void setDynamicColor(ColorPattern color);
void setInteruptColor(ColorPattern color);
void setDefaultState();

// External Utils
String getStateJson();
String getPowerJson();

bool getPower();
void setLeds(Color color);

// Apply State
void updateState();
void applyState();