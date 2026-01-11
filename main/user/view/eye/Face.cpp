/***************************************************
Copyright (c) 2020 Luis Llamas
(www.luisllamas.es)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program.  If not, see <http://www.gnu.org/licenses
****************************************************/

#include "Face.h"
#include "Common.h"

Face::Face(uint16_t eyeSize, uint16_t screenWidth, uint16_t screenHeight, uint16_t backgroundColor, uint16_t eyeColor)
	: LeftEye(*this), RightEye(*this), Blink(*this), Look(*this), Behavior(*this), Expression(*this)
{
	Width = screenWidth;
	Height = screenHeight;
	EyeSize = eyeSize;

	CenterX = Width / 2;
	CenterY = Height / 2;

	LeftEye.IsMirrored = true;
	Behavior.Timer.Start();
	Behavior.GoToEmotion(eEmotions::Normal);
}

void Face::LookFront()
{
	Look.LookAt(0.0, 0.0);
}

void Face::LookRight()
{
	Look.LookAt(-1.0, 0.0);
}

void Face::LookLeft()
{
	Look.LookAt(1.0, 0.0);
}

void Face::LookTop()
{
	Look.LookAt(0.0, 1.0);
}

void Face::LookBottom()
{
	Look.LookAt(0.0, -1.0);
}

void Face::DoBlink()
{
	Blink.Blink();
}

void Face::Update(void *ctx)
{

	if (RandomBehavior)
		Behavior.Update();
	if (RandomLook)
		Look.Update();
	if (RandomBlink)
		Blink.Update();
	Draw(ctx);
}

void Face::Draw(void *ctx)
{
	// Draw left eye
	LeftEye.CenterX = CenterX - EyeSize / 2 - EyeInterDistance;
	LeftEye.CenterY = CenterY;
	LeftEye.Draw(ctx);
	// Draw right eye
	RightEye.CenterX = CenterX + EyeSize / 2 + EyeInterDistance;
	RightEye.CenterY = CenterY;
	RightEye.Draw(ctx);
}
void Face::onDrawFillRectangle(std::function<void(void *ctx, int32_t x0, int32_t y0,
												  int32_t w, int32_t h,
												  uint16_t color)>
								   callback)
{
	LeftEye.onDrawFillRectangle(callback);
	RightEye.onDrawFillRectangle(callback);
}

void Face::onDrawFillRectangleRound(std::function<void(void *ctx, int32_t x0, int32_t y0,
													   int32_t w, int32_t h, int32_t radius,
													   uint16_t color)>
										callback)
{
	LeftEye.onDrawFillRectangleRound(callback);
	RightEye.onDrawFillRectangleRound(callback);
}

void Face::onDrawFillTriangle(std::function<void(void *ctx, int32_t x0, int32_t y0,
												 int32_t x1, int32_t y1,
												 int32_t x2, int32_t y2,
												 uint16_t color)>
								  callback)
{
	LeftEye.onDrawFillTriangle(callback);
	RightEye.onDrawFillTriangle(callback);
}

void Face::onDrawArc(std::function<void(void *ctx, int32_t x0, int32_t y0,
										int32_t rx, int32_t ry, 
										int32_t start, int32_t end,
										uint16_t color)>
						 callback)
{
	LeftEye.onDrawArc(callback);
	RightEye.onDrawArc(callback);
}
