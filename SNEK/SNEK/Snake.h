#pragma once
#include <utility>
#include <vector>

class Snake {
public:

	int head[2];					//the snek's head position on the play grid [x,y]

	std::vector<std::pair<int, int>> body;

	int snek_length = 0;			//current length of the snek (used to calculate current Score as well)
	int snek_body[625][2];			//the snek's body segments on the play grid [segment][x,y]
	bool directional_keys[4];		//stores input from directional keys
	bool action_keys;				//stores input from action keys
	char direction_tick;			//tick-resolution direction of player movement (north = n, south = s, east = e, west = w)
	char direction_frame;			//frame-resolution direction of player movement (north = n, south = s, east = e, west = w)
	bool holdW = false;				//tick-resolution storage of which arrow keys have been previously held
	bool holdE = false;				//"		"
	bool holdS = false;				//"		"
	bool holdN = false;				//"		"
	bool holdAction = false;		//was the player's action key held during the previous frame?
	float iProximityToFruit;		//stores each player's distance to the fruit
	bool justGotNewFruit = true;	//did the player just get a new fruit this/last frame?
	int snekSwallowTimer = 1;		//counts the frames since the player last swallowed a fruit, maxes out at player's snek_length + 1
	bool justDied = false;
	int potentialFruitSpot1;
	int potentialFruitSpot2;
	int potentialFruitSpot3;
	bool surroundingObstacles[8];	//stores surrounding space info (true if obstacles exists) 0 is top middle, 1-7 goes clockwise from there

	Snake(int startX, int startY, char startDirection);
	void tick();
};

