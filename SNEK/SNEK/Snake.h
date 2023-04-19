#pragma once
#define NOMINMAX
#include <Windows.h>
#include <utility>
#include <vector>

#include "ASCIIColor.h"
#include "SnakeControlScheme.h"
#include "Coords2D.h"


class Snake {
public:
	SnakeControlScheme controls;

	Soil::ASCIIColor color;

	Soil::Coords2D head;					//the snek's head position on the play grid
	std::vector<Soil::Coords2D> body;		//the snek's body segments on the play grid
	
	Soil::Coords2D direction_tick;			//tick-resolution direction of player movement (north = n, south = s, east = e, west = w)
	Soil::Coords2D direction_frame;			//frame-resolution direction of player movement (north = n, south = s, east = e, west = w)
	bool holdW = false;				//tick-resolution storage of which arrow keys have been previously held
	bool holdE = false;				//"		"
	bool holdS = false;				//"		"
	bool holdN = false;				//"		"
	bool holdAction = false;		//was the player's action key held during the previous frame?
	bool justGotNewFruit = false;	//did the player just get a new fruit this/last frame?
	int snekSwallowTimer = 1;		//counts the frames since the player last swallowed a fruit, maxes out at player's `body.size() + 1`
	bool justDied = false;
	int potentialFruitSpot1;
	int potentialFruitSpot2;
	int potentialFruitSpot3;
	bool surroundingObstacles[8];	//stores surrounding space info (true if obstacles exists) 0 is top middle, 1-7 goes clockwise from there

	Snake(Soil::ASCIIColor color, Soil::Coords2D startPosition, Soil::Coords2D startDirection, SnakeControlScheme controlScheme);
};

