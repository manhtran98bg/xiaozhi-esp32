
#ifndef _FLUXGARAGE_ROBOEYES_H
#define _FLUXGARAGE_ROBOEYES_H
#include "stdint.h"

#include "ICanvas.h"
#include "ICanvasManager.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_system.h"

#include "lvgl.h"

#define esp_random() (rand() % UINT32_MAX)

#define RANDOM_RANGE(min, max) \
    ((int)(esp_random() % ((max) - (min)) + (min)))

#define RANDOM(c) \
    ((int)((c) > 0 ? (esp_random() % (c)) : 0))

typedef uint8_t byte;

// Display colors

// For mood type switch
#define DEFAULT 0
#define TIRED 1
#define ANGRY 2
#define HAPPY 3

// For turning things on or off
#define ON 1
#define OFF 0

// For switch "predefined positions"
#define N 1  // north, top center
#define NE 2 // north-east, top right
#define E 3  // east, middle right
#define SE 4 // south-east, bottom right
#define S 5  // south, bottom center
#define SW 6 // south-west, bottom left
#define W 7  // west, middle left
#define NW 8 // north-west, top left
// for middle center set "DEFAULT"

class RoboEyes
{
public:
    RoboEyes();
    // For general setup - screen size and max. frame rate
    int screenWidth = 128;      // OLED display width, in pixels
    int screenHeight = 64;      // OLED display height, in pixels
    int frameInterval = 20;     // default value for 50 frames per second (1000/50 = 20 milliseconds)
    unsigned long fpsTimer = 0; // for timing the frames per second

    // For controlling mood types and expressions
    bool tired = 0;
    bool angry = 0;
    bool happy = 0;
    bool curious = 0;   // if true, draw the outer eye larger when looking left or right
    bool cyclops = 0;   // if true, draw only one eye
    bool eyeL_open = 0; // left eye opened or closed?
    bool eyeR_open = 0; // right eye opened or closed?

    //*********************************************************************************************
    //  Eyes Geometry
    //*********************************************************************************************

    // EYE LEFT - size and border radius
    int eyeLwidthDefault = 36;
    int eyeLheightDefault = 36;
    int eyeLwidthCurrent = eyeLwidthDefault;
    int eyeLheightCurrent = 1; // start with closed eye, otherwise set to eyeLheightDefault
    int eyeLwidthNext = eyeLwidthDefault;
    int eyeLheightNext = eyeLheightDefault;
    int eyeLheightOffset = 0;
    // Border Radius
    byte eyeLborderRadiusDefault = 8;
    byte eyeLborderRadiusCurrent = eyeLborderRadiusDefault;
    byte eyeLborderRadiusNext = eyeLborderRadiusDefault;

    // EYE RIGHT - size and border radius
    int eyeRwidthDefault = eyeLwidthDefault;
    int eyeRheightDefault = eyeLheightDefault;
    int eyeRwidthCurrent = eyeRwidthDefault;
    int eyeRheightCurrent = 1; // start with closed eye, otherwise set to eyeRheightDefault
    int eyeRwidthNext = eyeRwidthDefault;
    int eyeRheightNext = eyeRheightDefault;
    int eyeRheightOffset = 0;
    // Border Radius
    byte eyeRborderRadiusDefault = 8;
    byte eyeRborderRadiusCurrent = eyeRborderRadiusDefault;
    byte eyeRborderRadiusNext = eyeRborderRadiusDefault;

    int spaceBetweenDefault = 10;
    uint8_t BGCOLOR = 0;   // background and overlays
    uint8_t MAINCOLOR = 1; // drawings
    // EYE LEFT - Coordinates
    int eyeLxDefault = ((screenWidth) - (eyeLwidthDefault + spaceBetweenDefault + eyeRwidthDefault)) / 2;
    int eyeLyDefault = ((screenHeight - eyeLheightDefault) / 2);
    int eyeLx = eyeLxDefault;
    int eyeLy = eyeLyDefault;
    int eyeLxNext = eyeLx;
    int eyeLyNext = eyeLy;

    // EYE RIGHT - Coordinates
    int eyeRxDefault = eyeLx + eyeLwidthCurrent + spaceBetweenDefault;
    int eyeRyDefault = eyeLy;
    int eyeRx = eyeRxDefault;
    int eyeRy = eyeRyDefault;
    int eyeRxNext = eyeRx;
    int eyeRyNext = eyeRy;

    // BOTH EYES
    // Eyelid top size
    byte eyelidsHeightMax = eyeLheightDefault / 2; // top eyelids max height
    byte eyelidsTiredHeight = 0;
    byte eyelidsTiredHeightNext = eyelidsTiredHeight;
    byte eyelidsAngryHeight = 0;
    byte eyelidsAngryHeightNext = eyelidsAngryHeight;
    // Bottom happy eyelids offset
    byte eyelidsHappyBottomOffsetMax = (eyeLheightDefault / 2) + 3;
    byte eyelidsHappyBottomOffset = 0;
    byte eyelidsHappyBottomOffsetNext = 0;
    // Space between eyes

    int spaceBetweenCurrent = spaceBetweenDefault;
    int spaceBetweenNext = 10;

    //*********************************************************************************************
    //  Macro Animations
    //*********************************************************************************************

    // Animation - horizontal flicker/shiver
    bool hFlicker = 0;
    bool hFlickerAlternate = 0;
    byte hFlickerAmplitude = 2;

    // Animation - vertical flicker/shiver
    bool vFlicker = 0;
    bool vFlickerAlternate = 0;
    byte vFlickerAmplitude = 10;

    // Animation - auto blinking
    bool autoblinker = 0;           // activate auto blink animation
    int blinkInterval = 1;          // basic interval between each blink in full seconds
    int blinkIntervalVariation = 4; // interval variaton range in full seconds, random number inside of given range will be add to the basic blinkInterval, set to 0 for no variation
    unsigned long blinktimer = 0;   // for organising eyeblink timing

    // Animation - idle mode: eyes looking in random directions
    bool idle = 0;
    int idleInterval = 1;                 // basic interval between each eye repositioning in full seconds
    int idleIntervalVariation = 3;        // interval variaton range in full seconds, random number inside of given range will be add to the basic idleInterval, set to 0 for no variation
    unsigned long idleAnimationTimer = 0; // for organising eyeblink timing

    // Animation - eyes confused: eyes shaking left and right
    bool confused = 0;
    unsigned long confusedAnimationTimer = 0;
    int confusedAnimationDuration = 500;
    bool confusedToggle = 1;

    // Animation - eyes laughing: eyes shaking up and down
    bool laugh = 0;
    unsigned long laughAnimationTimer = 0;
    int laughAnimationDuration = 500;
    bool laughToggle = 1;

    // Animation - sweat on the forehead
    bool sweat = 0;
    byte sweatBorderradius = 3;

    // Sweat drop 1
    int sweat1XPosInitial = 2;
    int sweat1XPos;
    float sweat1YPos = 2;
    int sweat1YPosMax;
    float sweat1Height = 2;
    float sweat1Width = 1;

    // Sweat drop 2
    int sweat2XPosInitial = 2;
    int sweat2XPos;
    float sweat2YPos = 2;
    int sweat2YPosMax;
    float sweat2Height = 2;
    float sweat2Width = 1;

    // Sweat drop 3
    int sweat3XPosInitial = 2;
    int sweat3XPos;
    float sweat3YPos = 2;
    int sweat3YPosMax;
    float sweat3Height = 2;
    float sweat3Width = 1;

    void update(lv_layer_t *layer);
    void drawEyes(lv_obj_t *obj, lv_layer_t *layer);
    void begin(int width, int height, byte frameRate);

    void setFramerate(byte fps);
    void setDisplayColors(uint8_t background, uint8_t main);
    void setWidth(byte leftEye, byte rightEye);
    void setHeight(byte leftEye, byte rightEye);
    void setBorderradius(byte leftEye, byte rightEye);
    void setSpacebetween(int space);
    void setMood(unsigned char mood);
    void setPosition(unsigned char position);
    void setAutoblinker(bool active, int interval, int variation);
    void setAutoblinker(bool active);
    void setIdleMode(bool active, int interval, int variation);
    void setIdleMode(bool active);
    void setCuriosity(bool curiousBit);
    void setCyclops(bool cyclopsBit);
    void setHFlicker(bool flickerBit, byte Amplitude);
    void setHFlicker(bool flickerBit);
    void setVFlicker(bool flickerBit, byte Amplitude);
    void setVFlicker(bool flickerBit);
    void setSweat(bool sweatBit);
    int getScreenConstraint_X();
    int getScreenConstraint_Y();
    void close();
    void open();
    void blink();
    void close(bool left, bool right);
    void open(bool left, bool right);

    void blink(bool left, bool right);
    void anim_confused();
    void anim_laugh();
private:
    void drawFillRectangle(lv_obj_t* obj, lv_layer_t *layer, int32_t x0, int32_t y0,
                           int32_t w, int32_t h,
                           uint16_t color);
    void drawFillRectangleRound(lv_obj_t* obj, lv_layer_t *layer, int32_t x0, int32_t y0,
                                int32_t w, int32_t h, int32_t radius,
                                uint16_t color);
    void drawFillTriangle(lv_obj_t* obj,lv_layer_t *layer, int32_t x0, int32_t y0,
                          int32_t x1, int32_t y1,
                          int32_t x2, int32_t y2,
                          uint16_t color);
}; // end of class roboEyes

#endif