/***************************************************
Copyright (c) 2020 Luis Llamas
(www.luisllamas.es)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program.  If not, see <http://www.gnu.org/licenses
****************************************************/

#ifndef _FACE_h
#define _FACE_h

#include "Common.h"
#include "Animations.h"
#include "EyePresets.h"
#include "Eye.h"
#include "FaceExpression.h"
#include "FaceBehavior.h"
#include "LookAssistant.h"
#include "BlinkAssistant.h"

class Face
{

public:
    Face(uint16_t eyeSize, uint16_t screenWidth = 128, uint16_t screenHeight = 64, uint16_t backgroundColor = 0, uint16_t eyeColor = 0xffff);

    uint16_t Width;
    uint16_t Height;
    uint16_t CenterX;
    uint16_t CenterY;
    uint16_t EyeSize;
    uint16_t EyeInterDistance = 4;
    Eye LeftEye;
    Eye RightEye;
    BlinkAssistant Blink;
    LookAssistant Look;
    FaceBehavior Behavior;
    FaceExpression Expression;

    void Update(void *ctx);
    void DoBlink();

    bool RandomBehavior = true;
    bool RandomLook = true;
    bool RandomBlink = true;

    void LookLeft();
    void LookRight();
    void LookFront();
    void LookTop();
    void LookBottom();

    void onDrawFillRectangle(std::function<void(void *ctx, int32_t x0, int32_t y0,
                                                int32_t w, int32_t h,
                                                uint16_t color)>
                                 callback);
    void onDrawFillRectangleRound(std::function<void(void *ctx, int32_t x0, int32_t y0,
                                                     int32_t w, int32_t h, int32_t radius,
                                                     uint16_t color)>
                                      callback);
    void onDrawFillTriangle(std::function<void(void *ctx, int32_t x0, int32_t y0,
                                               int32_t x1, int32_t y1,
                                               int32_t x2, int32_t y2,
                                               uint16_t color)>
                                callback);
    void onDrawArc(std::function<void(void *ctx, int32_t x0, int32_t y0,
                                      int32_t rx, int32_t ry,
                                      int32_t start, int32_t end,
                                      uint16_t color)>
                       callback);

protected:
    void Draw(void *ctx);
};

#endif