#pragma once

#include "ASCIIColor.h"

using namespace Soil; // ASCIIColor, ANSI_#BIT_...

class SNEKColorPalette {
public:

	ASCIIColor blank = ASCIIColor(ANSI_4BIT_DEFAULT);

	// start menu
	ASCIIColor logo = ASCIIColor(ANSI_4BIT_BRIGHT_CYAN, ANSI_4BIT_DEFAULT, 214, 0, { 0xff, 0x87, 0x00 }, {0x00, 0x00, 0xd7}, ANSI_8BIT_COLOR_DEPTH);
	ASCIIColor hud = ASCIIColor(ANSI_4BIT_GREEN, ANSI_4BIT_DEFAULT, 27, 0, { 255,255,255 }, {0,0,0}, ANSI_8BIT_COLOR_DEPTH);
	ASCIIColor select_player_amount = ASCIIColor(ANSI_4BIT_WHITE);
	ASCIIColor player_amount = ASCIIColor(ANSI_4BIT_BRIGHT_WHITE);
	ASCIIColor press_start = ASCIIColor(ANSI_4BIT_BRIGHT_MAGENTA);

	// gameplay
	ASCIIColor player_1 = ASCIIColor(ANSI_4BIT_BRIGHT_GREEN);
	ASCIIColor player_2 = ASCIIColor(ANSI_4BIT_BRIGHT_RED);
	ASCIIColor player_dead = ASCIIColor(ANSI_4BIT_WHITE);
	ASCIIColor fruit = ASCIIColor(ANSI_4BIT_BRIGHT_MAGENTA);
	ASCIIColor fruit_swallowed = ASCIIColor(ANSI_4BIT_MAGENTA);
	ASCIIColor portal = ASCIIColor(ANSI_4BIT_BLUE);

	// high score entry
	ASCIIColor keyboard = ASCIIColor(ANSI_4BIT_GREEN, ANSI_4BIT_DEFAULT, 27, 0, { 255,255,255 }, { 0,0,0 }, ANSI_8BIT_COLOR_DEPTH);
	ASCIIColor keyboard_selected = ASCIIColor(ANSI_4BIT_DEFAULT, ANSI_4BIT_GREEN, 27, 0, { 255,255,255 }, { 0,0,0 }, ANSI_8BIT_COLOR_DEPTH);

	SNEKColorPalette() {};
};