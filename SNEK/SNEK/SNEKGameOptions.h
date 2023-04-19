#pragma once

#include "Coords2D.h"
#include "SNEKColorPalette.h"


class SNEKGameOptions {
public:
	int playerCount = 1;
	Soil::Coords2D gridSize = { 10,10 };
	SNEKColorPalette colors;

	SNEKGameOptions(int playerCount, Soil::Coords2D gridSize, SNEKColorPalette colors) : playerCount{playerCount}, gridSize{gridSize}, colors{colors} {}
};