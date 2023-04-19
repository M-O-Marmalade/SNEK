#pragma once

#include "Coords2D.h"
#include "ColorPalette.h"


class SnakeGameOptions {
public:
	int playerCount = 1;
	Soil::Coords2D gridSize = { 10,10 };
	ColorPalette colors;

	SnakeGameOptions(int playerCount, Soil::Coords2D gridSize, ColorPalette colors) : playerCount{playerCount}, gridSize{gridSize}, colors{colors} {}
};