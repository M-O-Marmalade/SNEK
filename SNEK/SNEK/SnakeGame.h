#pragma once
#include <algorithm>
#include <string>
#include <vector>
#include <chrono>
#include <fstream>
#include <thread>

#include "Snake.h"
#include "ASCIIOutputCMD.h"
#include "AudioSystem.h"
#include "ColorPalette.h"

class SnakeGame {
private:
	ASCIIGraphics* asciiGraphics;
	ASCIIOutputCMD* asciiOutput;
	AudioSystem* snekAudioSystem;
	ColorPalette colorPalette;
	
	void readScoreFile();

public:
	std::vector<Snake> snakes;
	std::vector<std::vector<char>> gameGrid; //char display[25][25]{ 'z' };		//the Play Grid [x][y] {'z' empty space, '8' snek head, '7' snek body, 'o' fruit, 'X' trap, 'p' portal}		
	
	Coords2D currentFruit;			//location of the current fruit on the game grid [x,y]
	int portalCount = 0;			//amount of portals on the map
	int portalCoordinates[6][2];	//coordinates of the current portals on the map
	bool gotNewFruit = false;
	
	bool gameLose = false;				//current Game Lose state
	bool playAgain = true;					//decides whether or not to play again after losing
	int playerCount = 1;			//amount of players, can be increased at start screen
	int highestCurrentLength = 0;	//highest length out of all current players/sneks
	bool wasPreviousHighScoreFound;
	int oldHighScore;
	bool gotNewHighScore = false;
	int highScore = 0;				//current High Score
	int styleCounter = 0;			//current STYLE Score
	int styleHighScore = 0;			//current Style High Score
	std::string keyboard = "ABCDEFGHIJKLMNOPQRSTUVWXYZ.-_$& ";
	std::string highScoreName;
	std::vector<double> bpmValues = { 54.5f, 62.5f, 75.5f, 89.0f, 100.0f, 127.0f, 137.0f, 152.0f, 164.0f, 172.0f, 181.0f, 200.0f };

	std::chrono::duration<long double, std::nano> fps;
	std::chrono::steady_clock::time_point frameTime;
	std::chrono::steady_clock::time_point tickTime;

	int frameRate = 10;				//frame rate setting
	int currentFrame = 0;			//keeps track of how many frames have passed

	int snekMoveTimelinePosition = 0;		//snakeMoveInstance timeline position
	int snekMoveTimelinePositionMax = 200;	//snakeMoveInstance timeline position max
	bool isScoreUnder11 = true;				//changes to false when highestCurrentLength passes 11
	float snakeMoveReverbLevel = 0.0f;		//reverb level for the snakeMoveInstance sound
	float proximityToFruit;					//stores the closest player's proximity to the current fruit				
	int i16thNote = 1;						//counts each frame and resets back to 1 after reaching 16 (it skips 17 and goes back to 1)
	bool chordsStartToggle = false;
	int currentChord = 4;
	bool hiHatToggle = false;
	bool gotNewHighScoreSoundPlay = false;
	int currentChordBPM = 0;
	int switchChordsCounter = 0;
	bool switchChords = false;
	bool hasFirstSwitchHappened = false;
	bool waitUntilNextDownbeatish = false;
	bool actionKeyHeld = false;		//indicates whether any one player is holding an action key

	std::vector<std::string> bpmNames = {
	"Instruments+FX/BPMs/bpm54_5",
	"Instruments+FX/BPMs/bpm62_5",
	"Instruments+FX/BPMs/bpm75_5",
	"Instruments+FX/BPMs/bpm89",
	"Instruments+FX/BPMs/bpm100",
	"Instruments+FX/BPMs/bpm127",
	"Instruments+FX/BPMs/bpm137",
	"Instruments+FX/BPMs/bpm152",
	"Instruments+FX/BPMs/bpm164",
	"Instruments+FX/BPMs/bpm172",
	"Instruments+FX/BPMs/bpm181",
	"Instruments+FX/BPMs/bpm200"
	};

	
	SnakeGame(int playerCount, int gridWidth, int gridHeight, ASCIIGraphics* asciiGraphics, ASCIIOutputCMD* asciiOutput, AudioSystem* audioSystem);
	void play();
};

