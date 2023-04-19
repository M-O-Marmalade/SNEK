#pragma once

#include "ASCIIColor.h"


class ColorPalette {
public:
	Soil::ASCIIColor standard = Soil::ASCIIColor(Soil::ANSI_4BIT_FG_GREEN);
	Soil::ASCIIColor logo = Soil::ASCIIColor(Soil::ANSI_4BIT_FG_GREEN | Soil::ANSI_4BIT_FG_BLUE | Soil::ANSI_4BIT_FG_BRIGHT);
	Soil::ASCIIColor press_start = Soil::ASCIIColor(Soil::ANSI_4BIT_FG_RED | Soil::ANSI_4BIT_FG_BLUE | Soil::ANSI_4BIT_FG_BRIGHT);
	Soil::ASCIIColor player_amount = Soil::ASCIIColor(Soil::ANSI_4BIT_FG_RED | Soil::ANSI_4BIT_FG_GREEN | Soil::ANSI_4BIT_FG_BLUE | Soil::ANSI_4BIT_FG_BRIGHT);
	Soil::ASCIIColor player_1 = Soil::ASCIIColor(Soil::ANSI_4BIT_FG_GREEN | Soil::ANSI_4BIT_FG_BRIGHT);
	Soil::ASCIIColor player_2 = Soil::ASCIIColor(Soil::ANSI_4BIT_FG_RED | Soil::ANSI_4BIT_FG_BRIGHT);
	Soil::ASCIIColor fruit = Soil::ASCIIColor(Soil::ANSI_4BIT_FG_RED | Soil::ANSI_4BIT_FG_BLUE | Soil::ANSI_4BIT_FG_BRIGHT);
	Soil::ASCIIColor fruit_swallowed = Soil::ASCIIColor(Soil::ANSI_4BIT_FG_RED | Soil::ANSI_4BIT_FG_BLUE);
	Soil::ASCIIColor portal = Soil::ASCIIColor(Soil::ANSI_4BIT_FG_BLUE);
	Soil::ASCIIColor keyboard = Soil::ASCIIColor(Soil::ANSI_4BIT_FG_GREEN);
	Soil::ASCIIColor keyboard_selected = Soil::ASCIIColor(Soil::ANSI_4BIT_BG_GREEN);
	Soil::ASCIIColor hud = Soil::ASCIIColor(Soil::ANSI_4BIT_FG_GREEN, 213, 0, { 255,255,255 }, {0,0,0}, Soil::ASCIIColor::ANSI_8BIT_COLOR);


	Soil::ASCIIColor bright_cyan = Soil::ASCIIColor(Soil::ANSI_4BIT_FG_GREEN | Soil::ANSI_4BIT_FG_BLUE | Soil::ANSI_4BIT_FG_BRIGHT);
	Soil::ASCIIColor green = Soil::ASCIIColor(Soil::ANSI_4BIT_FG_GREEN);
	Soil::ASCIIColor bright_green = Soil::ASCIIColor(Soil::ANSI_4BIT_FG_GREEN | Soil::ANSI_4BIT_FG_BRIGHT);
	Soil::ASCIIColor red = Soil::ASCIIColor(Soil::ANSI_4BIT_FG_RED);
	Soil::ASCIIColor bright_red = Soil::ASCIIColor(Soil::ANSI_4BIT_FG_RED | Soil::ANSI_4BIT_FG_BRIGHT);
	Soil::ASCIIColor pink = Soil::ASCIIColor(Soil::ANSI_4BIT_FG_RED | Soil::ANSI_4BIT_FG_BLUE);
	Soil::ASCIIColor bright_pink = Soil::ASCIIColor(Soil::ANSI_4BIT_FG_RED | Soil::ANSI_4BIT_FG_BLUE | Soil::ANSI_4BIT_FG_BRIGHT);
	Soil::ASCIIColor white = Soil::ASCIIColor(Soil::ANSI_4BIT_FG_RED | Soil::ANSI_4BIT_FG_GREEN | Soil::ANSI_4BIT_FG_BLUE | Soil::ANSI_4BIT_FG_BRIGHT);
	Soil::ASCIIColor grey = Soil::ASCIIColor(Soil::ANSI_4BIT_FG_RED | Soil::ANSI_4BIT_FG_GREEN | Soil::ANSI_4BIT_FG_BLUE);
	Soil::ASCIIColor black = Soil::ASCIIColor(0, 0, 0);
};