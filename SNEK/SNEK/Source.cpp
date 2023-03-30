//	 __    _    _              _  __   ____
//	/ /   | \  | |     /\     | |/ /  |  __|
//	\ \   |  \ | |    /  \    | | /   | |__
//	 \ \  | | \| |   / /\ \   |   \   |  __|
//	 / /  | |\ \ |  /  __  \  | |\ \  | |__
//	/_/   |_| \__| /__/  \__\ |_| \_\ |____|
// / / BY M. O. MARMALADE / / / / / / / / / / / / /

  ///////////////////////
 // PROGRAM STRUCTURE //
///////////////////////
/*

| Audio/Graphics Init | Splash Screen | Start Screen | Pre-New Game Setup | Read Input | Game Logic | Audio Processing | Graphics Display | Name Entry | Try Again Screen |
|=================================STARTUP=================================|===========================GAME LOOP===========================|===========GAME OVER===========|

*/

//The game is time-synced to a specific BPM (beats-per-minute) at all times, as the game is very music focused

//time/tick/framerate calculations are done in nanoseconds (0.000000001 second, or 1e-9 second).
//BPM to ns (beats-per-minute to nanoseconds) conversions are done as 60,000,000,000ns/xBPM.

//audio timeline (FMOD Studio) calculations are done in milliseconds (0.001 second, or 1e-3 second).
//BPM to ms (beats-per-minute to milliseconds) conversions are done as 60,000ms/xBPM.

  /////////////////////
 // PROJECT OUTLINE //
/////////////////////
// Includes/Namespaces
// Global Variables
// Audio System Setup (FMOD Studio)
// Loading/Preparing Audio Events
// Display Setup
// [Splash Screen Animation]
// [Start Screen]
// Read High Score File
// [New Game Perparation]
// [Game Loop Start]
// Set Framerate
// Tick Clock
// Read Player Input
// Check + Set Direction [Tick Resolution]
// Refresh Display
// Move Body Segments
// Check + Set Direction [Frame Resolution]
// Place Snek Body Into Display Array
// Store Each Snek's Surroundings
// Add Style Points
// Update Style High Score
// Move Player
// Detect if Player Has Hit Map Edge
// Detect if Player Has Hit Themselves
// Calculate Proximity to the Fruit
// Detect if Player Has Hit Fruit
// Set New High Score
// Create Portals
// Pass Player Through Portals
// Place Player Into Display Array
// Place Fruit Into Display Array
// Audio Processing
// Draw Screen
// Color the Screen
// Color the Fruit Pink
// Color Player 1 Green
// Color Player 2 Red
// Game Over Screen
// Name Entry
// Write High Score To File
// "Try Again?" Screen


  //					 //
 // INCLUDES/NAMESPACES //
//					   //
#pragma once
#include <Windows.h>
#include <algorithm>
#include <chrono>
#include <codecvt>
#include <fstream>
#include <string>
#include <thread>
#include <vector>

#include "AudioSystem.h"
#include "ASCIIGraphics.h"
#include "ASCIIOutputCMD.h"
#include "ColorPalette.h"

#include "SnakeGame.h"


  //				  //
 // GLOBAL VARIABLES //
//					//

//LOGIC VARIABLES
char gameGrid[25][25]{'z'};		//the Play Grid [x][y] {'z' empty space, '8' snek head, '7' snek body, 'o' fruit, 'X' trap, 'p' portal}		
bool gameLose;					//current Game Lose state
bool playAgain;					//decides whether or not to play again after losing
int highScore = 0;				//current High Score
int styleCounter = 0;			//current STYLE Score
int styleHighScore = 0;			//current Style High Score
int currentFruit[2];			//location of the current fruit on the game grid [x,y]
int playerCount = 1;			//amount of players, can be increased at start screen
int highestCurrentLength = 0;	//highest length out of all current players/sneks
int portalCount = 0;			//amount of portals on the map
int portalCoordinates[6][2];	//coordinates of the current portals on the map
bool gotNewFruit = false;
int oldHighScore;
bool gotNewHighScore = false;
std::string keyboard = "ABCDEFGHIJKLMNOPQRSTUVWXYZ.-_$& ";
std::string highScoreName;
bool wasPreviousHighScoreFound;
std::vector<double> bpmValues = { 54.5f, 62.5f, 75.5f, 89.0f, 100.0f, 127.0f, 137.0f, 152.0f, 164.0f, 172.0f, 181.0f, 200.0f };

//INPUT VARIABLES
bool arrowKeys[4];				//stores input from arrow keys
bool zKey;						//stores input from Z key


int main() {	

	 //									 //
	// AUDIO SYSTEM SETUP (FMOD Studio) //
   //								   //

	std::vector<std::string> fmodEventNames = {
		"Menu+Songs/SplashJingle",
		"Menu+Songs/ANewChip",
		"Menu+Songs/StartButton",
		"Menu+Songs/FancyBoss",
		"Menu+Songs/ExitGame",
		"Instruments+FX/SnakeFruit",
		"Instruments+FX/SnakeFruit11",
		"Instruments+FX/SnakeMove",
		"Instruments+FX/SnakeLunge",
		"Instruments+FX/Death",
		"Instruments+FX/Snare1",
		"Instruments+FX/808Drum",
		"Instruments+FX/Cymbal",
		"Instruments+FX/KickTight",
		"Instruments+FX/Triangle",
		"Instruments+FX/arp",
		"Instruments+FX/newHighScore"
	};

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

	const bool fmodLiveUpdate = false;
	AudioSystem snekAudioSystem(fmodLiveUpdate);

	snekAudioSystem.loadBanks();

	for (std::string name : fmodEventNames) {
		snekAudioSystem.loadEventInstance(name);
	}

	for (std::string name : bpmNames) {
		snekAudioSystem.loadEventInstance(name);
	}


	  //			   //
	 // DISPLAY SETUP //
	//			  	 //
	
	int nScreenWidth = 80;			//width of the console window (measured in characters, not pixels)
	int nScreenHeight = 25;			//height of the console window (measured in characters, not pixels)

	ColorPalette colorPalette;
	ASCIIGraphics textGraphics(nScreenWidth, nScreenHeight);
	ASCIIOutputCMD textOutputCMD;


	  //						 //
	 // SPLASH SCREEN ANIMATION //
	//						   //

	snekAudioSystem.startEventInstance("Menu+Songs/SplashJingle");
	snekAudioSystem.fmodUpdate();

	// reset the color of the whole screen
	textGraphics.fillColor(colorPalette.standard, 0, 0, textGraphics.width - 1, textGraphics.height - 1);

	// Draw Splash Screen
	bool animation = true;
	int u = 0;
	int charToOverwrite = 996;
	textGraphics.fillColor(colorPalette.bright_cyan, 30, 10, 50, 20);
	while (charToOverwrite <= 1005) {

		textGraphics.textBuffer[charToOverwrite] = "Citrus 64"[u];
		charToOverwrite++;
		u++;
		textOutputCMD.pushOutput(textGraphics);

		std::this_thread::sleep_for(std::chrono::milliseconds(77));
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(222));
	

	//Erase Splash Screen
	animation = true;
	charToOverwrite = 992;	
	while (charToOverwrite <= 1006) {
		
		textGraphics.textBuffer[charToOverwrite] = char(32);
		charToOverwrite++;
		
		textOutputCMD.pushOutput(textGraphics);
		
		std::this_thread::sleep_for(std::chrono::milliseconds(77));	
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(222));


	  //			  //
	 // START SCREEN //
	//				//
	bool startScreen = true;
	bool startScreenToggle = true;
	int startScreenFrameCount = 111;

	std::this_thread::sleep_for(std::chrono::milliseconds(777));

	snekAudioSystem.startEventInstance("Menu+Songs/ANewChip");	//begin start screen playback	(FMOD)
	snekAudioSystem.fmodUpdate();

	

	bool holdKey = false;
	while (startScreen) {

		// draw logo
		textGraphics.drawTextSprite(19, 7, ASCIISprite(
			" __    _    _              _  __   ____\n"
			"/ /   | \\  | |     /\\     | |/ /  |  __|\n"
			"\\ \\   |  \\ | |    /  \\    | | /   | |__\n"
			" \\ \\  | | \\| |   / /\\ \\   |   \\   |  __|\n"
			" / /  | |\\ \\ |  /  __  \\  | |\\ \\  | |__\n"
			"/_/   |_| \\__| /__/  \\__\\ |_| \\_\\ |____|",
			colorPalette.hud));

		// draw player count
		textGraphics.fillColor(colorPalette.grey, 34, 20, 42, 20);
		textGraphics.fillColor(colorPalette.white, 43, 20, 45, 20);
		textGraphics.drawText(34, 20, "Players: <" + std::to_string(playerCount) + ">");

		if (playerCount == 2) {

			textGraphics.fillColor(colorPalette.player_2, 15, 14, 35, 16);
			textGraphics.fillColor(colorPalette.player_1, 36, 14, 80, 16);
			textGraphics.drawText(15, 14, 
				"--------------------      --------------------------\n"
				"| P2: WASD + B-Key |      | P1: Arrow Keys + P-Key |\n"
				"--------------------      --------------------------");
			
		}
		else {

			textGraphics.drawTextSprite(15, 14, ASCIISprite(
				"         --------------------------------           \n"
				"         | Controls: Arrow Keys + Z-Key |           \n"
				"         --------------------------------           ",
				colorPalette.player_1));
		}

		textGraphics.drawTextSprite(35, 22, ASCIISprite("Citrus 64", colorPalette.logo)); // draw studio name

		if (startScreenFrameCount == 111) {
			if (startScreenToggle) {
				startScreenToggle = false;

				// draw "Press Z to start" every 111th frame
				ASCIISprite pressStartSprite("Press [Z] to start", colorPalette.press_start);
				textGraphics.drawTextSprite(31, 18, pressStartSprite);

				// play snakefruitinstance sound for flashing "press start" button (FMOD)
				snekAudioSystem.fmodEventInstances["Instruments+FX/SnakeFruit"]->setPitch(1.0f);
				snekAudioSystem.fmodEventInstances["Instruments+FX/SnakeFruit"]->setVolume(0.7f);
				snekAudioSystem.startEventInstance("Instruments+FX/SnakeFruit");
			}
			else {
				startScreenToggle = true;
				textGraphics.drawText(31, 18, "                  ");
			}

			startScreenFrameCount = 0;
		}

		textOutputCMD.pushOutput(textGraphics);

		startScreenFrameCount++;

		std::this_thread::sleep_for(std::chrono::milliseconds(7));

		for (int o = 0; o < 4; o++) {
			arrowKeys[o] = (0x8000 & GetAsyncKeyState((unsigned char)("\x25\x26\x27\x28"[o]))) != 0;						
		}

		if (arrowKeys[2] && playerCount < 2 && !holdKey) {
			playerCount++;
			snekAudioSystem.fmodEventInstances["Instruments+FX/SnakeFruit"]->setPitch(1.77f);
			snekAudioSystem.fmodEventInstances["Instruments+FX/SnakeFruit"]->setVolume(1.0f);
			snekAudioSystem.startEventInstance("Instruments+FX/SnakeFruit");
			holdKey = true;
		}
		else if (arrowKeys[0] && playerCount > 1 && !holdKey) {
			playerCount--;
			snekAudioSystem.fmodEventInstances["Instruments+FX/SnakeFruit"]->setPitch(0.64f);
			snekAudioSystem.fmodEventInstances["Instruments+FX/SnakeFruit"]->setVolume(1.0f);
			snekAudioSystem.startEventInstance("Instruments+FX/SnakeFruit");
			holdKey = true;
		}
		else if (holdKey && !arrowKeys[0] && !arrowKeys[2]) {
			holdKey = false;
		}

		if (zKey = (0x8000 & GetAsyncKeyState((unsigned char)("Z"[0]))) != 0) {

			startScreen = false;	//begin to exit start screen when Z key is pressed

			snekAudioSystem.stopEventInstance("Menu+Songs/ANewChip", true);	// (FMOD)
			snekAudioSystem.startEventInstance("Menu+Songs/StartButton");
			snekAudioSystem.fmodUpdate();	//play startbutton sound

			

			//Game Start animation//

			std::vector<std::pair<std::string, int>> pressedStartAnimFrames = {
				{"Press [Z] to start", 33},
				{"Press [Z++to start", 33},
				{"Press [+AR+o start", 33},
				{"Press +TART+ start", 33},
				{"Press+START!+start", 33},
				{"Pres+ START! +tart", 33},
				{"Pre+  START!  +art", 33},
				{"Pr+   START!   +rt", 33},
				{"P+    START!    +t", 33},
				{"+     START!     +", 33},
				{"-                -", 177},
				{"-     START!     -", 177},
				{"-                -", 77},
				{"-     START!     -", 1389}
			};

			for (auto& animFrame : pressedStartAnimFrames) {
				textGraphics.drawText(31, 18, animFrame.first);
				textOutputCMD.pushOutput(textGraphics);
				std::this_thread::sleep_for(std::chrono::milliseconds(animFrame.second));
			}
		}

		snekAudioSystem.fmodUpdate();
	}
	
	SnakeGame currentGame(playerCount, 25, 25, &textGraphics, &textOutputCMD, &snekAudioSystem);
	currentGame.play();
	while (currentGame.playAgain) {
		currentGame = SnakeGame(currentGame.playerCount, 25, 25, &textGraphics, &textOutputCMD, &snekAudioSystem);
		currentGame.play();
	}

	return 0;

}