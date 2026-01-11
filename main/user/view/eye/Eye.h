/***************************************************
Copyright (c) 2020 Luis Llamas
(www.luisllamas.es)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program.  If not, see <http://www.gnu.org/licenses
****************************************************/

#ifndef _EYE_h
#define _EYE_h

#include "Common.h"
#include "Animations.h"
#include "EyeConfig.h"

#include "EyeTransition.h"
#include "EyeTransformation.h"
#include "EyeVariation.h"
#include "EyeBlink.h"
#include "EyeVariation.h"

#include <functional>

class Face;

enum CornerType
{
  T_R,
  T_L,
  B_L,
  B_R
};

class Eye
{
protected:
  Face &_face;

  void Update();
  void ChainOperators();
  std::function<void(void *ctx, int32_t x0, int32_t y0,
                     int32_t w, int32_t h,
                     uint16_t color)>
      on_draw_fill_rectangle_;
  std::function<void(void *ctx, int32_t x0, int32_t y0,
                     int32_t w, int32_t h, int32_t radius,
                     uint16_t color)>
      on_draw_fill_rectangle_round_;
  std::function<void(void *ctx, int32_t x0, int32_t y0,
                     int32_t x1, int32_t y1,
                     int32_t x2, int32_t y2,
                     uint16_t color)>
      on_draw_fill_triangle_;
  std::function<void(void *ctx, int32_t x0, int32_t y0,
                     int32_t rx, int32_t ry,
                     int32_t start, int32_t end,
                     uint16_t color)>
      on_draw_arc_;

public:
  Eye(Face &face);
  uint16_t CenterX;
  uint16_t CenterY;
  bool IsMirrored = false;

  EyeConfig Config;
  EyeConfig *FinalConfig;

  EyeTransition Transition;
  EyeTransformation Transformation;
  EyeVariation Variation1;
  EyeVariation Variation2;
  EyeBlink BlinkTransformation;

  void ApplyPreset(const EyeConfig preset);
  void TransitionTo(const EyeConfig preset);
  void Draw(void *ctx);

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

private:
  void drawFillRectangle(void *ctx, int32_t x0, int32_t y0,
                         int32_t w, int32_t h,
                         uint16_t color);
  void drawFillRectangleRound(void *ctx, int32_t x0, int32_t y0,
                              int32_t w, int32_t h, int32_t radius,
                              uint16_t color);
  void drawFillTriangle(void *ctx, int32_t x0, int32_t y0,
                        int32_t x1, int32_t y1,
                        int32_t x2, int32_t y2,
                        uint16_t color);
  void drawFillRectangularTriangle(void *ctx, int32_t x0, int32_t y0,
                                   int32_t x1, int32_t y1,
                                   uint16_t color);
  void drawFillRectangle2Point(void *ctx, int32_t x0, int32_t y0,
                               int32_t x1, int32_t y1,
                               uint16_t color);
  void drawArc(void *ctx, int32_t x0, int32_t y0,
               int32_t rx, int32_t ry,
               int32_t start, int32_t end,
               uint16_t color);
  void drawFillCorner(void *ctx, CornerType corner, int16_t x0, int16_t y0, int32_t rx, int32_t ry, uint16_t color);
};

#endif