/***************************************************
Copyright (c) 2020 Luis Llamas
(www.luisllamas.es)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program.  If not, see <http://www.gnu.org/licenses
****************************************************/

#include "Eye.h"
#include "Face.h"
#include "lvgl.h"
static int min(int a, int b)
{
	return (a < b) ? a : b;
}
static int max(int a, int b)
{
	return (a > b) ? a : b;
}
Eye::Eye(Face &face) : _face(face)
{

	this->IsMirrored = false;
	ChainOperators();
	Variation1.Animation._t0 = 200;
	Variation1.Animation._t1 = 200;
	Variation1.Animation._t2 = 200;
	Variation1.Animation._t3 = 200;
	Variation1.Animation._t4 = 0;
	Variation1.Animation.Interval = 800;

	Variation2.Animation._t0 = 0;
	Variation2.Animation._t1 = 200;
	Variation2.Animation._t2 = 200;
	Variation2.Animation._t3 = 200;
	Variation2.Animation._t4 = 200;
	Variation2.Animation.Interval = 800;
}

void Eye::ChainOperators()
{
	Transition.Origin = &Config;
	Transformation.Input = &Config;
	Variation1.Input = &(Transformation.Output);
	Variation2.Input = &(Variation1.Output);
	BlinkTransformation.Input = &(Variation2.Output);
	FinalConfig = &(BlinkTransformation.Output);
}

void Eye::Update()
{
	Transition.Update();
	Transformation.Update();
	Variation1.Update();
	Variation2.Update();
	BlinkTransformation.Update();
}

void Eye::ApplyPreset(const EyeConfig config)
{
	Config.OffsetX = this->IsMirrored ? -config.OffsetX : config.OffsetX;
	Config.OffsetY = -config.OffsetY;
	Config.Height = config.Height;
	Config.Width = config.Width;
	Config.Slope_Top = this->IsMirrored ? config.Slope_Top : -config.Slope_Top;
	Config.Slope_Bottom = this->IsMirrored ? config.Slope_Bottom : -config.Slope_Bottom;
	Config.Radius_Top = config.Radius_Top;
	Config.Radius_Bottom = config.Radius_Bottom;
	Config.Inverse_Radius_Top = config.Inverse_Radius_Top;
	Config.Inverse_Radius_Bottom = config.Inverse_Radius_Bottom;

	Transition.Animation.Restart();
}

void Eye::TransitionTo(const EyeConfig config)
{
	Transition.Destin.OffsetX = this->IsMirrored ? -config.OffsetX : config.OffsetX;
	Transition.Destin.OffsetY = -config.OffsetY;
	Transition.Destin.Height = config.Height;
	Transition.Destin.Width = config.Width;
	Transition.Destin.Slope_Top = this->IsMirrored ? config.Slope_Top : -config.Slope_Top;
	Transition.Destin.Slope_Bottom = this->IsMirrored ? config.Slope_Bottom : -config.Slope_Bottom;
	Transition.Destin.Radius_Top = config.Radius_Top;
	Transition.Destin.Radius_Bottom = config.Radius_Bottom;
	Transition.Destin.Inverse_Radius_Top = config.Inverse_Radius_Top;
	Transition.Destin.Inverse_Radius_Bottom = config.Inverse_Radius_Bottom;

	Transition.Animation.Restart();
}

// Draw functions

void Eye::onDrawFillRectangle(std::function<void(void *ctx, int32_t x0, int32_t y0,
												 int32_t w, int32_t h,
												 uint16_t color)>
								  callback)
{
	on_draw_fill_rectangle_ = callback;
}

void Eye::onDrawFillRectangleRound(std::function<void(void *ctx, int32_t x0, int32_t y0,
													  int32_t w, int32_t h, int32_t radius,
													  uint16_t color)>
									   callback)
{
	on_draw_fill_rectangle_round_ = callback;
}

void Eye::onDrawFillTriangle(std::function<void(void *ctx, int32_t x0, int32_t y0,
												int32_t x1, int32_t y1,
												int32_t x2, int32_t y2,
												uint16_t color)>
								 callback)
{
	on_draw_fill_triangle_ = callback;
}

void Eye::onDrawArc(std::function<void(void *ctx, int32_t x0, int32_t y0,
										int32_t rx, int32_t ry, 
										int32_t start, int32_t end,
										uint16_t color)>
						  callback)
{
	on_draw_arc_ = callback;
}
void Eye::drawFillRectangle(void *ctx, int32_t x0, int32_t y0,
							int32_t w, int32_t h,
							uint16_t color)
{
	drawFillRectangleRound(ctx, x0, y0, w, h, 0, 1);
}
void Eye::drawFillRectangle2Point(void *ctx, int32_t x0, int32_t y0,
								  int32_t x1, int32_t y1,
								  uint16_t color)
{
	int32_t l = min(x0, x1);
	int32_t r = max(x0, x1);
	int32_t t = min(y0, y1);
	int32_t b = max(y0, y1);
	int32_t w = r - l;
	int32_t h = b - t;
	drawFillRectangle(ctx, x0, y0, w, h, color);
}
void Eye::drawFillRectangleRound(void *ctx, int32_t x0, int32_t y0,
								 int32_t w, int32_t h, int32_t radius,
								 uint16_t color)
{
	if (on_draw_fill_rectangle_round_)
		on_draw_fill_rectangle_round_(ctx, x0, y0, w, h, radius, color);
}
void Eye::drawFillRectangularTriangle(void *ctx, int32_t x0, int32_t y0,
									  int32_t x1, int32_t y1,
									  uint16_t color)
{
	drawFillTriangle(ctx, x0, y0, x1, y1, x1, y0, color);
}
void Eye::drawFillTriangle(void *ctx, int32_t x0, int32_t y0,
						   int32_t x1, int32_t y1,
						   int32_t x2, int32_t y2,
						   uint16_t color)
{
	if (on_draw_fill_triangle_)
		on_draw_fill_triangle_(ctx, x0, y0, x1, y1, x2, y2, color);
}

void Eye::drawArc(void *ctx, int32_t x0, int32_t y0,
										int32_t rx, int32_t ry, 
										int32_t start, int32_t end,
										uint16_t color)
{
	if (on_draw_arc_)
		on_draw_arc_(ctx, x0, y0, rx, ry, start, end, color);
}

void Eye::Draw(void *ctx)
{
	Update();
	int16_t centerX = CenterX;
	int16_t centerY = CenterY;
	EyeConfig *config = FinalConfig;
	// Amount by which corners will be shifted up/down based on requested "slope"
	int32_t delta_y_top = config->Height * config->Slope_Top / 2.0;
	int32_t delta_y_bottom = config->Height * config->Slope_Bottom / 2.0;
	// Full extent of the eye, after accounting for slope added at top and bottom
	auto totalHeight = config->Height + delta_y_top - delta_y_bottom;
	// If the requested top/bottom radius would exceed the height of the eye, adjust them downwards
	if (config->Radius_Bottom > 0 && config->Radius_Top > 0 && totalHeight - 1 < config->Radius_Bottom + config->Radius_Top)
	{
		int32_t corrected_radius_top = (float)config->Radius_Top * (totalHeight - 1) / (config->Radius_Bottom + config->Radius_Top);
		int32_t corrected_radius_bottom = (float)config->Radius_Bottom * (totalHeight - 1) / (config->Radius_Bottom + config->Radius_Top);
		config->Radius_Top = corrected_radius_top;
		config->Radius_Bottom = corrected_radius_bottom;
	}

	// Calculate _inside_ corners of eye (TL, TR, BL, and BR) before any slope or rounded corners are applied
	int32_t TLc_y = centerY + config->OffsetY - config->Height / 2 + config->Radius_Top - delta_y_top;
	int32_t TLc_x = centerX + config->OffsetX - config->Width / 2 + config->Radius_Top;
	int32_t TRc_y = centerY + config->OffsetY - config->Height / 2 + config->Radius_Top + delta_y_top;
	int32_t TRc_x = centerX + config->OffsetX + config->Width / 2 - config->Radius_Top;
	int32_t BLc_y = centerY + config->OffsetY + config->Height / 2 - config->Radius_Bottom - delta_y_bottom;
	int32_t BLc_x = centerX + config->OffsetX - config->Width / 2 + config->Radius_Bottom;
	int32_t BRc_y = centerY + config->OffsetY + config->Height / 2 - config->Radius_Bottom + delta_y_bottom;
	int32_t BRc_x = centerX + config->OffsetX + config->Width / 2 - config->Radius_Bottom;

	// Calculate interior extents
	int32_t min_c_x = min(TLc_x, BLc_x);
	int32_t max_c_x = max(TRc_x, BRc_x);
	int32_t min_c_y = min(TLc_y, TRc_y);
	int32_t max_c_y = max(BLc_y, BRc_y);

	// Fill eye centre
	drawFillRectangle2Point(ctx, min_c_x, min_c_y, max_c_x, max_c_y, 1);

	drawFillRectangle2Point(ctx, TRc_x, TRc_y, BRc_x + config->Radius_Bottom, BRc_y, 1); // Right
	drawFillRectangle2Point(ctx, TLc_x - config->Radius_Top, TLc_y, BLc_x, BLc_y, 1);	 // Left
	drawFillRectangle2Point(ctx, TLc_x, TLc_y - config->Radius_Top, TRc_x, TRc_y, 1);	 // Top
	drawFillRectangle2Point(ctx, BLc_x, BLc_y, BRc_x, BRc_y + config->Radius_Bottom, 1); // Bottom

	// Draw slanted edges at top of bottom of eyes
	// +ve Slope_Top means eyes slope downwards towards middle of face
	if (config->Slope_Top > 0)
	{
		drawFillRectangularTriangle(ctx, TLc_x, TLc_y - config->Radius_Top, TRc_x, TRc_y - config->Radius_Top, 0);
		drawFillRectangularTriangle(ctx, TRc_x, TRc_y - config->Radius_Top, TLc_x, TLc_y - config->Radius_Top, 1);
	}
	else if (config->Slope_Top < 0)
	{
		drawFillRectangularTriangle(ctx, TRc_x, TRc_y - config->Radius_Top, TLc_x, TLc_y - config->Radius_Top, 0);
		drawFillRectangularTriangle(ctx, TLc_x, TLc_y - config->Radius_Top, TRc_x, TRc_y - config->Radius_Top, 1);
	}
	// Draw slanted edges at bottom of eyes
	if (config->Slope_Bottom > 0)
	{
		drawFillRectangularTriangle(ctx, BRc_x + config->Radius_Bottom, BRc_y + config->Radius_Bottom, BLc_x - config->Radius_Bottom, BLc_y + config->Radius_Bottom, 0);
		drawFillRectangularTriangle(ctx, BLc_x - config->Radius_Bottom, BLc_y + config->Radius_Bottom, BRc_x + config->Radius_Bottom, BRc_y + config->Radius_Bottom, 1);
	}
	else if (config->Slope_Bottom < 0)
	{
		drawFillRectangularTriangle(ctx, BLc_x - config->Radius_Bottom, BLc_y + config->Radius_Bottom, BRc_x + config->Radius_Bottom, BRc_y + config->Radius_Bottom, 0);
		drawFillRectangularTriangle(ctx, BRc_x + config->Radius_Bottom, BRc_y + config->Radius_Bottom, BLc_x - config->Radius_Bottom, BLc_y + config->Radius_Bottom, 1);
	}

	// // Draw corners (which extend "outwards" towards corner of screen from supplied coordinate values)
	if (config->Radius_Top > 0)
	{
		drawFillCorner(ctx, T_L, TLc_x, TLc_y, config->Radius_Top, config->Radius_Top, 1);
		drawFillCorner(ctx, T_R, TRc_x, TRc_y, config->Radius_Top, config->Radius_Top, 1);
	}
	if (config->Radius_Bottom > 0)
	{
		drawFillCorner(ctx, B_L, BLc_x, BLc_y, config->Radius_Bottom, config->Radius_Bottom, 1);
		drawFillCorner(ctx, B_R, BRc_x, BRc_y, config->Radius_Bottom, config->Radius_Bottom, 1);
	}
}

void Eye::drawFillCorner(void *ctx, CornerType corner, int16_t x0, int16_t y0, int32_t rx, int32_t ry,
						 uint16_t color)
{

	switch (corner)
	{
	case T_R: 
		drawArc(ctx, x0, y0, rx, ry, 270, 360, color);
		break;
	case B_R: 
		drawArc(ctx, x0, y0, rx, ry, 0, 90, color);
		break;
	case B_L: 
		drawArc(ctx, x0, y0, rx, ry, 90, 180, color);
		break;
	case T_L: 
		drawArc(ctx, x0, y0, rx, ry, 180, 270, color);
		break;
	}
}