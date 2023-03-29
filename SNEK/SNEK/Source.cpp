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
#include "TextGraphics.h"
#include "TextOutputCMD.h"


  //				  //
 // GLOBAL VARIABLES //
//					//

//LOGIC VARIABLES
char display[25][25]{'z'};		//the Play Grid [x][y] {'z' empty space, '8' snek head, '7' snek body, 'o' fruit, 'X' trap, 'p' portal}		
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
std::wstring keyboard = L"ABCDEFGHIJKLMNOPQRSTUVWXYZ.-_ ←↓";
std::wstring highScoreName;
std::wstring highScoreNameFromFileWide;
std::string highScoreNameFromFileNarrow;
std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
bool wasPreviousHighScoreFound;
std::vector<double> bpmValues = { 54.5f, 62.5f, 75.5f, 89.0f, 100.0f, 127.0f, 137.0f, 152.0f, 164.0f, 172.0f, 181.0f, 200.0f };

//INPUT VARIABLES
bool arrowKeys[4];				//stores input from arrow keys
bool zKey;						//stores input from Z key
bool actionKeyHeld = false;		//indicates whether any one player is holding an action key

//DISPLAY VARIABLES
int frameRate = 10;				//frame rate setting
int currentFrame = 0;			//keeps track of how many frames have passed
int nScreenWidth = 80;			//width of the console window (measured in characters, not pixels)
int nScreenHeight = 25;			//height of the console window (measured in characters, not pixels)

struct ColorPalette
{
	WORD standard = FOREGROUND_GREEN;
	WORD logo = FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
	WORD press_start = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
	WORD player_amount = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
	WORD player_1 = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
	WORD player_2 = FOREGROUND_RED | FOREGROUND_INTENSITY;
	WORD fruit = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
	WORD fruit_swallowed = FOREGROUND_RED | FOREGROUND_BLUE;
	WORD portal = FOREGROUND_BLUE;
	WORD keyboard = FOREGROUND_GREEN;
	WORD keyboard_selected = BACKGROUND_GREEN;
	WORD hud = FOREGROUND_GREEN;


	WORD bright_cyan = FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
	WORD green = FOREGROUND_GREEN;
	WORD bright_green = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
	WORD red = FOREGROUND_RED;
	WORD bright_red = FOREGROUND_RED | FOREGROUND_INTENSITY;
	WORD pink = FOREGROUND_RED | FOREGROUND_BLUE;
	WORD bright_pink = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
	WORD white = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
	WORD grey = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
	WORD black = 0;
};

std::chrono::duration<long double, std::nano> fps;
std::chrono::steady_clock::time_point frameTime;
std::chrono::steady_clock::time_point tickTime;

//SOUND VARIABLES (FMOD)

bool fmodLiveUpdate = false;
bool simpleSound = false;

int snekMoveTimelinePosition = 0;		//snakeMoveInstance timeline position
int snekMoveTimelinePositionMax = 200;	//snakeMoveInstance timeline position max
bool isScoreUnder11 = true;				//changes to false when highestCurrentLength passes 11
float snakeMoveReverbLevel = 0.0f;		//reverb level for the snakeMoveInstance sound
float proximityToFruit;					//stores the closest player's proximity to the current fruit				
int i16thNote = 1;						//counts each frame and resets back to 1 after reaching 16 (it skips 17 and goes back to 1)
bool chordsStartToggle = false;
int currentChord;
bool hiHatToggle = false;
bool gotNewHighScoreSoundPlay;
int currentChordBPM;
int switchChordsCounter;
bool switchChords;
bool hasFirstSwitchHappened;
bool waitUntilNextDownbeatish;

//PLAYER-SPECIFIC STRUCTURE/VARIABLES
struct snek {
	int snek_head[2];				//the snek's head position on the play grid [x,y]
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
};


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

	AudioSystem snekAudioSystem;

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

	ColorPalette colorPalette;
	TextGraphics textGraphics(nScreenWidth, nScreenHeight);
	TextOutputCMD textOutputCMD;

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

		textGraphics.textBuffer[charToOverwrite] = L"Citrus 64"[u];
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
		textGraphics.drawTextSprite(19, 7, TextSprite(
			L" __    _    _              _  __   ____\n"
			L"/ /   | \\  | |     /\\     | |/ /  |  __|\n"
			L"\\ \\   |  \\ | |    /  \\    | | /   | |__\n"
			L" \\ \\  | | \\| |   / /\\ \\   |   \\   |  __|\n"
			L" / /  | |\\ \\ |  /  __  \\  | |\\ \\  | |__\n"
			L"/_/   |_| \\__| /__/  \\__\\ |_| \\_\\ |____|",
			colorPalette.hud));

		// draw player count
		textGraphics.fillColor(colorPalette.grey, 34, 20, 42, 20);
		textGraphics.fillColor(colorPalette.white, 43, 20, 45, 20);
		textGraphics.drawText(34, 20, L"Players: <" + std::to_wstring(playerCount) + L">");

		if (playerCount == 2) {

			textGraphics.fillColor(colorPalette.player_2, 15, 14, 35, 16);
			textGraphics.fillColor(colorPalette.player_1, 36, 14, 80, 16);
			textGraphics.drawText(15, 14, 
				L"--------------------      --------------------------\n"
				L"| P2: WASD + B-Key |      | P1: Arrow Keys + P-Key |\n"
				L"--------------------      --------------------------");
			
		}
		else {

			textGraphics.drawTextSprite(15, 14, TextSprite(
				L"         --------------------------------           \n"
				L"         | Controls: Arrow Keys + Z-Key |           \n"
				L"         --------------------------------           ",
				colorPalette.player_1));
		}

		textGraphics.drawTextSprite(35, 22, TextSprite(L"Citrus 64", colorPalette.logo)); // draw studio name

		if (startScreenFrameCount == 111) {
			if (startScreenToggle) {
				startScreenToggle = false;

				// draw "Press Z to start" every 111th frame
				TextSprite pressStartSprite(L"Press [Z] to start", colorPalette.press_start);
				textGraphics.drawTextSprite(31, 18, pressStartSprite);

				// play snakefruitinstance sound for flashing "press start" button (FMOD)
				snekAudioSystem.fmodEventInstances["Instruments+FX/SnakeFruit"]->setPitch(1.0f);
				snekAudioSystem.fmodEventInstances["Instruments+FX/SnakeFruit"]->setVolume(0.7f);
				snekAudioSystem.startEventInstance("Instruments+FX/SnakeFruit");
			}
			else {
				startScreenToggle = true;
				textGraphics.drawText(31, 18, L"                  ");
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

			std::vector<std::pair<std::wstring, int>> pressedStartAnimFrames = {
				{L"Press [Z] to start", 33},
				{L"Press [Z++to start", 33},
				{L"Press [+AR+o start", 33},
				{L"Press +TART+ start", 33},
				{L"Press+START!+start", 33},
				{L"Pres+ START! +tart", 33},
				{L"Pre+  START!  +art", 33},
				{L"Pr+   START!   +rt", 33},
				{L"P+    START!    +t", 33},
				{L"+     START!     +", 33},
				{L"-                -", 177},
				{L"-     START!     -", 177},
				{L"-                -", 77},
				{L"-     START!     -", 1389}
			};

			for (auto& animFrame : pressedStartAnimFrames) {
				textGraphics.drawText(31, 18, animFrame.first);
				textOutputCMD.pushOutput(textGraphics);
				std::this_thread::sleep_for(std::chrono::milliseconds(animFrame.second));
			}
		}

		snekAudioSystem.fmodUpdate();
	}
	
	  //					  //
	 // READ HIGH SCORE FILE //
	//						//
	std::ifstream scoreFileRead;
	scoreFileRead.open("ScoreFile");
	if (scoreFileRead.is_open()) {
		
		std::string highScoreFromFile;
		getline(scoreFileRead, highScoreFromFile);
		if (highScoreFromFile.length() != 0) {
			highScore = stoi(highScoreFromFile);
		}
		else {
			highScore = 0;
		}

		getline(scoreFileRead, highScoreNameFromFileNarrow);				
		highScoreNameFromFileWide = converter.from_bytes(highScoreNameFromFileNarrow);
		highScoreName = highScoreNameFromFileWide;

		scoreFileRead.close();

		wasPreviousHighScoreFound = true;
	}
	else {
		highScore = 0;
		highScoreName = L" ";
	}
	

	do {
		  //					  //
		 // NEW GAME PREPARATION //
		//					    //

		snek snek1[2];

		snek1[0].snek_head[0] = (24 / (playerCount + 1)) * playerCount + (playerCount - 1);
		snek1[0].snek_head[1] = 12;
		snek1[0].direction_tick = 's';
		snek1[0].direction_frame = 's';

		if (playerCount == 2) {
			snek1[1].snek_head[0] = 7;
			snek1[1].snek_head[1] = 12;
			snek1[1].direction_tick = 's';
			snek1[1].direction_frame = 's';
		}
					
		gameLose = false;			//reset game lose condition
		oldHighScore = highScore;	//update the old high score based on previous game's high score
		fps = std::chrono::duration<long double, std::nano>(15000000000 / bpmValues[0]);			//reset the framerate
		currentFrame = 0;			//reset the frame counter

		for (int p = 0; p < playerCount; p++) {		//reset players' lengths
			snek1[p].snek_length = 0;
		}

		highestCurrentLength = 0;	//reset the highest current length
		styleCounter = 0;	//reset the style counter
		portalCount = 0;	//reset the amount of portals on the map back to 0

		srand(time(0));	//seed the RNG using system time, and make a new fruit location
		for (bool inLoop = true; inLoop;) {

			currentFruit[0] = rand() % 13 + 6;
			currentFruit[1] = rand() % 13 + 6;

			if (currentFruit[0] != 13) {
				inLoop = false;
			}
		}

		//Reset the Game Grid Display//
		for (int x = 0; x < 25; x++) {
			for (int y = 0; y < 25; y++) {
				display[x][y] = 'z';
			}
		}

		//Reset the Screen String Buffer//
		textGraphics.fillText(0, 0, textGraphics.width - 1, textGraphics.height - 1, L' ');

		//Reset FMOD-Related Audio/Sound Variables//
		snekMoveTimelinePosition = 0;
		snekMoveTimelinePositionMax = 200;
		isScoreUnder11 = true;
		snakeMoveReverbLevel = 0.0f;
		i16thNote = 1;
		if (!simpleSound) {
			snekAudioSystem.startEventInstance(bpmNames[0]);
		}		
		snekAudioSystem.fmodEventInstances["Instruments+FX/SnakeMove"]->setParameterByName("SnakeMoveVolume", 1.0f);
		snekAudioSystem.fmodEventInstances["Instruments+FX/Snare1"]->setParameterByName("SnareReverb", 0.0f);
		snekAudioSystem.fmodEventInstances["Instruments+FX/arp"]->setParameterByName("ArpVolume", 0.0f);
		snekAudioSystem.fmodEventInstances["Instruments+FX/Triangle"]->setParameterByName("TriangleDecay", 1.0f);
		snekAudioSystem.fmodEventInstances["Instruments+FX/SnakeFruit"]->setPitch(1.0f);
		snekAudioSystem.fmodEventInstances["Instruments+FX/SnakeFruit"]->setVolume(0.7f);
		snekAudioSystem.fmodResult = snekAudioSystem.fmodSystem->setParameterByName("ChordsSelection", 0.0f);
		snekAudioSystem.fmodResult = snekAudioSystem.fmodSystem->setParameterByName("ChordsReverb", 0.0f);
		//result = system->setVolume(1.0f);
		chordsStartToggle = false;
		currentChord = 4;
		hiHatToggle = false;
		gotNewHighScore = false;
		gotNewHighScoreSoundPlay = false;	
		switchChords = false;
		switchChordsCounter = 0;
		hasFirstSwitchHappened = false;
		currentChordBPM = 0;
		waitUntilNextDownbeatish = false;

		frameTime = std::chrono::steady_clock::now();	//record start time of first frame of the game loop

		  //				   //
		 // [GAME LOOP START] //
		//				     //
		while (gameLose == false) {

			if (playerCount == 2 && currentFrame == 1) {
				frameTime = std::chrono::steady_clock::now();
			}

			  //			   //
			 // SET FRAMERATE //
			//				 //
			if (gotNewFruit) {
				switch (highestCurrentLength) {
				case 1:
					fps = std::chrono::duration<long double, std::nano>(15000000000 / bpmValues[1]);
					break;
				case 7:
					fps = std::chrono::duration<long double, std::nano>(15000000000 / bpmValues[2]);
					break;
				case 11:
					fps = std::chrono::duration<long double, std::nano>(15000000000 / bpmValues[3]);
					break;
				case 20:
					fps = std::chrono::duration<long double, std::nano>(15000000000 / bpmValues[4]);
					break;
				case 30:
					fps = std::chrono::duration<long double, std::nano>(15000000000 / bpmValues[5]);
					break;
				case 40:
					fps = std::chrono::duration<long double, std::nano>(15000000000 / bpmValues[6]);
					break;
				case 50:
					fps = std::chrono::duration<long double, std::nano>(15000000000 / bpmValues[7]);
					break;
				case 60:
					fps = std::chrono::duration<long double, std::nano>(15000000000 / bpmValues[8]);
					break;
				case 70:
					fps = std::chrono::duration<long double, std::nano>(15000000000 / bpmValues[9]);
					break;
				case 80:
					fps = std::chrono::duration<long double, std::nano>(15000000000 / bpmValues[10]);
					break;
				case 90:
					fps = std::chrono::duration<long double, std::nano>(15000000000 / bpmValues[11]);
					break;
				}
			}
			
			  //			//
			 // TICK CLOCK //
			//			  //
			while (std::chrono::steady_clock::now() < frameTime + fps) {
				
				  //				   //
				 // READ PLAYER INPUT //
				//					 //
				for (int k = 0; k < 4; k++) {	//player 1
					snek1[0].directional_keys[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x25\x26\x27\x28"[k]))) != 0;

				}

				for (int k = 0; k < 4; k++) {	//player 2
					snek1[1].directional_keys[k] = (0x8000 & GetAsyncKeyState((unsigned char)("AWDS"[k]))) != 0;

				}

				if (highestCurrentLength > 10) {
					if (playerCount == 1) {
						snek1[0].action_keys = ((0x8000 & GetAsyncKeyState((unsigned char)("Z"[0]))) != 0);
					}
					else {
						snek1[0].action_keys = ((0x8000 & GetAsyncKeyState((unsigned char)("P"[0]))) != 0);
						snek1[1].action_keys = ((0x8000 & GetAsyncKeyState((unsigned char)("V"[0]))) != 0);
					}
					
				}
				else {
					snek1[0].action_keys = false;
					snek1[1].action_keys = false;
				}
							

				  //										 //
				 // CHECK + SET DIRECTION [TICK RESOLUTION] //
				//										   //

				for (int pt = 0; pt < playerCount; pt++) {

					if (snek1[pt].directional_keys[0] && snek1[pt].holdW == false && snek1[pt].direction_frame != 'e') {
						snek1[pt].direction_tick = 'w';
					}

					else if (snek1[pt].directional_keys[1] && snek1[pt].holdN == false && snek1[pt].direction_frame != 's') {
						snek1[pt].direction_tick = 'n';
					}

					else if (snek1[pt].directional_keys[2] && snek1[pt].holdE == false && snek1[pt].direction_frame != 'w') {
						snek1[pt].direction_tick = 'e';
					}

					else if (snek1[pt].directional_keys[3] && snek1[pt].holdS == false && snek1[pt].direction_frame != 'n') {
						snek1[pt].direction_tick = 's';
					}
				}
			}		

			currentFrame++;
			frameTime += std::chrono::duration_cast<std::chrono::nanoseconds>(fps);

			  //				 //
			 // REFRESH DISPLAY //
			//				   //
			for (int x = 0; x < 25; x++) {

				for (int y = 0; y < 25; y++) {

					display[x][y] = 'z';
				}
			}

			  //					//
			 // MOVE BODY SEGMENTS //
			//					  //
			for (int pt = 0; pt < playerCount; pt++) {

				for (int r = snek1[pt].snek_length - 1; r >= 0; r--) {

					if (r > 0) {
						snek1[pt].snek_body[r][0] = snek1[pt].snek_body[r - 1][0];	//move all segments except the segment right before the head
						snek1[pt].snek_body[r][1] = snek1[pt].snek_body[r - 1][1];

					}

					else {
						snek1[pt].snek_body[r][0] = snek1[pt].snek_head[0];		//move the segment right before the head
						snek1[pt].snek_body[r][1] = snek1[pt].snek_head[1];

					}
				}

			}

			  //										  //
			 // CHECK + SET DIRECTION [FRAME RESOLUTION] //
			//											//
			for (int pt = 0; pt < playerCount; pt++) {

				if (snek1[pt].direction_tick == 'w' && snek1[pt].holdW == false && snek1[pt].direction_frame != 'e') {
					snek1[pt].direction_frame = 'w';

					snek1[pt].holdW = true;
					snek1[pt].holdE = false;
					snek1[pt].holdS = false;
					snek1[pt].holdN = false;
				}

				else if (snek1[pt].direction_tick == 'n' && snek1[pt].holdN == false && snek1[pt].direction_frame != 's') {
					snek1[pt].direction_frame = 'n';

					snek1[pt].holdN = true;
					snek1[pt].holdE = false;
					snek1[pt].holdS = false;
					snek1[pt].holdW = false;
				}

				else if (snek1[pt].direction_tick == 'e' && snek1[pt].holdE == false && snek1[pt].direction_frame != 'w') {
					snek1[pt].direction_frame = 'e';

					snek1[pt].holdE = true;
					snek1[pt].holdW = false;
					snek1[pt].holdS = false;
					snek1[pt].holdN = false;
				}

				else if (snek1[pt].direction_tick == 's' && snek1[pt].holdS == false && snek1[pt].direction_frame != 'n') {
					snek1[pt].direction_frame = 's';

					snek1[pt].holdS = true;
					snek1[pt].holdE = false;
					snek1[pt].holdW = false;
					snek1[pt].holdN = false;
				}
			}

			  //								    //
			 // PLACE SNEK BODY INTO DISPLAY ARRAY //
			//									  //
			for (int pt = 0; pt < playerCount; pt++) {

				for (int r = snek1[pt].snek_length - 1; r >= 0; r--) {
					display[snek1[pt].snek_body[r][0]][snek1[pt].snek_body[r][1]] = '7';
				}
			}

			  //								//
			 // STORE EACH SNEK'S SURROUNDINGS //
			//								  //
			for (int pt = 0; pt < playerCount; pt++) {

				//North
				if (display[snek1[pt].snek_head[0]][snek1[pt].snek_head[1] - 1] == 'z' || display[snek1[pt].snek_head[0]][snek1[pt].snek_head[1] - 1] == '+') {
					snek1[pt].surroundingObstacles[0] = false;
				}
				else {
					snek1[pt].surroundingObstacles[0] = true;
				}

				//North-East
				if (display[snek1[pt].snek_head[0] + 1][snek1[pt].snek_head[1] - 1] == 'z' || display[snek1[pt].snek_head[0] + 1][snek1[pt].snek_head[1] - 1] == '+') {
					snek1[pt].surroundingObstacles[1] = false;
				}
				else {
					snek1[pt].surroundingObstacles[1] = true;
				}

				//East
				if (display[snek1[pt].snek_head[0] + 1][snek1[pt].snek_head[1]] == 'z' || display[snek1[pt].snek_head[0] + 1][snek1[pt].snek_head[1]] == '+') {
					snek1[pt].surroundingObstacles[2] = false;
				}
				else {
					snek1[pt].surroundingObstacles[2] = true;
				}

				//South-East
				if (display[snek1[pt].snek_head[0] + 1][snek1[pt].snek_head[1] + 1] == 'z' || display[snek1[pt].snek_head[0] + 1][snek1[pt].snek_head[1] + 1] == '+') {
					snek1[pt].surroundingObstacles[3] = false;
				}
				else {
					snek1[pt].surroundingObstacles[3] = true;
				}

				//South
				if (display[snek1[pt].snek_head[0]][snek1[pt].snek_head[1] + 1] == 'z' || display[snek1[pt].snek_head[0]][snek1[pt].snek_head[1] + 1] == '+') {
					snek1[pt].surroundingObstacles[4] = false;
				}
				else {
					snek1[pt].surroundingObstacles[4] = true;
				}

				//South-West
				if (display[snek1[pt].snek_head[0] - 1][snek1[pt].snek_head[1] + 1] == 'z' || display[snek1[pt].snek_head[0] - 1][snek1[pt].snek_head[1] + 1] == '+') {
					snek1[pt].surroundingObstacles[5] = false;
				}
				else {
					snek1[pt].surroundingObstacles[5] = true;
				}

				//West
				if (display[snek1[pt].snek_head[0] - 1][snek1[pt].snek_head[1]] == 'z' || display[snek1[pt].snek_head[0] - 1][snek1[pt].snek_head[1]] == '+') {
					snek1[pt].surroundingObstacles[6] = false;
				}
				else {
					snek1[pt].surroundingObstacles[6] = true;
				}

				//North-West
				if (display[snek1[pt].snek_head[0] - 1][snek1[pt].snek_head[1] - 1] == 'z' || display[snek1[pt].snek_head[0] - 1][snek1[pt].snek_head[1] - 1] == '+') {
					snek1[pt].surroundingObstacles[7] = false;
				}
				else {
					snek1[pt].surroundingObstacles[7] = true;
				}
			}

			  //				  //
			 // ADD STYLE POINTS //
			//					//
			for (int pt = 0; pt < playerCount; pt++) {
				
				//EAST//
				if (snek1[pt].direction_frame == 'e') {

					//if player jumps over an obstacle to the East, add two style points
					if (snek1[pt].action_keys && snek1[pt].surroundingObstacles[2] && snek1[pt].snek_head[0] + 2 < 25) {
						if (display[snek1[pt].snek_head[0] + 2][snek1[pt].snek_head[1]] == 'z' || display[snek1[pt].snek_head[0] + 2][snek1[pt].snek_head[1]] == '+') {							
							styleCounter += 2;							
						}
					}

					//if player goes in-between two obstacles to the North-East and South-East, add one style point
					else if (snek1[pt].surroundingObstacles[1] && snek1[pt].surroundingObstacles[3] && !snek1[pt].surroundingObstacles[2] && snek1[pt].snek_head[0] + 1 < 25) {
						styleCounter++;					
					}
				}

				//WEST//
				if (snek1[pt].direction_frame == 'w') {

					//if player jumps over an obstacle to the West, add two style points
					if (snek1[pt].action_keys && snek1[pt].surroundingObstacles[6] && snek1[pt].snek_head[0] - 2 > 0) {
						if (display[snek1[pt].snek_head[0] - 2][snek1[pt].snek_head[1]] == 'z' || display[snek1[pt].snek_head[0] - 2][snek1[pt].snek_head[1]] == '+') {
							styleCounter += 2;
						}
					}

					//if player goes in-between two obstacles to the North-West and South-West, add one style point
					else if (snek1[pt].surroundingObstacles[7] && snek1[pt].surroundingObstacles[5] && !snek1[pt].surroundingObstacles[6] && snek1[pt].snek_head[0] - 1 > 0) {
						styleCounter++;
					}
				}

				//NORTH//
				if (snek1[pt].direction_frame == 'n') {

					//if player jumps over an obstacle to the North, add two style points
					if (snek1[pt].action_keys && snek1[pt].surroundingObstacles[0] && snek1[pt].snek_head[1] - 2 > 0) {
						if (display[snek1[pt].snek_head[0]][snek1[pt].snek_head[1] - 2] == 'z' || display[snek1[pt].snek_head[0]][snek1[pt].snek_head[1] - 2] == '+') {
							styleCounter += 2;
						}
					}

					//if player goes in-between two obstacles to the North-West and North-East, add one style point
					else if (snek1[pt].surroundingObstacles[7] && snek1[pt].surroundingObstacles[1] && !snek1[pt].surroundingObstacles[0] && snek1[pt].snek_head[1] - 1 > 0) {
						styleCounter++;
					}
				}

				//SOUTH//
				if (snek1[pt].direction_frame == 's') {

					//if player jumps over an obstacle to the South, add two style points
					if (snek1[pt].action_keys && snek1[pt].surroundingObstacles[4] && snek1[pt].snek_head[1] + 2 < 25) {
						if (display[snek1[pt].snek_head[0]][snek1[pt].snek_head[1] + 2] == 'z' || display[snek1[pt].snek_head[0]][snek1[pt].snek_head[1] + 2] == '+') {
							styleCounter += 2;
						}
					}

					//if player goes in-between two obstacles to the South-West and South-East, add one style point
					else if (snek1[pt].surroundingObstacles[5] && snek1[pt].surroundingObstacles[3] && !snek1[pt].surroundingObstacles[4] && snek1[pt].snek_head[1] + 1 < 25) {
						styleCounter++;
					}
				}				
			}

			  //						 //
			 // UPDATE STYLE HIGH SCORE //
			//						   //
			if (styleCounter > styleHighScore && !gameLose) {
				styleHighScore++;
			}
					   			
			  //			 //
			 // MOVE PLAYER //
			//			   //
			for (int pt = 0; pt < playerCount; pt++) {

				if (snek1[pt].direction_frame == 'e') {
					snek1[pt].snek_head[0]++;

					if (snek1[pt].action_keys) {
						snek1[pt].snek_head[0]++;
					}
				}

				else if (snek1[pt].direction_frame == 'w') {
					snek1[pt].snek_head[0]--;

					if (snek1[pt].action_keys) {
						snek1[pt].snek_head[0]--;
					}
				}

				else if (snek1[pt].direction_frame == 's') {
					snek1[pt].snek_head[1]++;

					if (snek1[pt].action_keys) {
						snek1[pt].snek_head[1]++;
					}
				}

				else if (snek1[pt].direction_frame == 'n') {
					snek1[pt].snek_head[1]--;

					if (snek1[pt].action_keys) {
						snek1[pt].snek_head[1]--;
					}
				}				
			}

			  //								   //
			 // DETECT IF PLAYER HAS HIT MAP EDGE //
			//									 //
			for (int pt = 0; pt < playerCount; pt++) {
				if (snek1[pt].snek_head[0] < 0 || snek1[pt].snek_head[0] > 24 || snek1[pt].snek_head[1] < 0 || snek1[pt].snek_head[1] > 24) {
					snek1[pt].justDied = true;
					gameLose = true;
										
					if (snek1[pt].snek_head[0] > 24) {
						snek1[pt].snek_head[0] = 24;
					}
					else if (snek1[pt].snek_head[0] < 0) {
						snek1[pt].snek_head[0] = 0;
					}
					else if (snek1[pt].snek_head[1] > 24) {
						snek1[pt].snek_head[1] = 24;
					}
					else if (snek1[pt].snek_head[1] < 0) {
						snek1[pt].snek_head[1] = 0;
					}
				}
			}

			
			  //									 //
			 // DETECT IF PLAYER HAS HIT THEMSELVES //
			//									   //
			for (int pt = 0; pt < playerCount; pt++) {
				if (display[snek1[pt].snek_head[0]][snek1[pt].snek_head[1]] == '7' || display[snek1[pt].snek_head[0]][snek1[pt].snek_head[1]] == 'X' || display[snek1[pt].snek_head[0]][snek1[pt].snek_head[1]] == '8') {
					snek1[pt].justDied = true;
					gameLose = true;
				}
			}

			  //								  //
			 // CALCULATE PROXIMITY TO THE FRUIT //
			//									//
			for (int pt = 0; pt < playerCount; pt++) {	//calculate each player's individual proximity
				snek1[pt].iProximityToFruit = 1.0f - ((abs(snek1[pt].snek_head[0] - currentFruit[0]) + abs(snek1[pt].snek_head[1] - currentFruit[1])) / 48.0f);	//1/48 max distance
			}
			if (playerCount > 1 && snek1[1].iProximityToFruit > snek1[0].iProximityToFruit) {	//use whichever one is closer
				proximityToFruit = snek1[1].iProximityToFruit;
			}
			else {
				proximityToFruit = snek1[0].iProximityToFruit;	//if there is only one player, then 
			}			

			
			  //								//
			 // DETECT IF PLAYER HAS HIT FRUIT //
			//								  //
			gotNewFruit = false;

			for (int pt = 0; pt < playerCount; pt++) {

				if (snek1[pt].snek_head[0] == currentFruit[0] && snek1[pt].snek_head[1] == currentFruit[1]) {

					snek1[pt].snek_length++;
					snek1[pt].justGotNewFruit = true;
					gotNewFruit = true;
					snek1[pt].snekSwallowTimer = 0;
										
					for (int e = 0; e < 300; e++) {
						currentFruit[0] = rand() % 25;		//create a potential fruit spot
						currentFruit[1] = rand() % 25;						
											   				
						//check if that spot is currently filled//
						if ((currentFruit[0] != snek1[pt].snek_head[0] && currentFruit[1] != snek1[pt].snek_head[1]) && display[currentFruit[0]][currentFruit[1]] != '7' && display[currentFruit[0]][currentFruit[1]] != 'X' && display[currentFruit[0]][currentFruit[1]] != 'p') {

							//a temp proximity to use in the for loop for the new fruit//
							int proximityToFruitTemp = (abs(snek1[pt].snek_head[0] - currentFruit[0]) + abs(snek1[pt].snek_head[1] - currentFruit[1]));

							//calculate snek's potential spots to be on the beat
							if (i16thNote == 1) {
								snek1[pt].potentialFruitSpot1 = 17;
							}
							else {
								snek1[pt].potentialFruitSpot1 = 17 - i16thNote;
							}

							if (i16thNote >= 5) {
								snek1[pt].potentialFruitSpot2 = 16 - (i16thNote - 5);
							}
							else {
								snek1[pt].potentialFruitSpot2 = 5 - i16thNote;
							}

							if (i16thNote >= 13) {
								snek1[pt].potentialFruitSpot3 = 16 - (i16thNote - 13);
							}
							else {
								snek1[pt].potentialFruitSpot3 = 13 - i16thNote;
							}

							//accept new fruit position if it lands the appropriate distance from the snek who just got the last fruit//
							if (proximityToFruitTemp % 16 == snek1[pt].potentialFruitSpot1 || proximityToFruitTemp % 16 == snek1[pt].potentialFruitSpot2 || proximityToFruitTemp % 16 == snek1[pt].potentialFruitSpot3) {
								break;
							}						
						}
					}

					if (snek1[pt].snek_length == 11) {
						isScoreUnder11 = false;
					}					

					  //					//
					 // SET NEW HIGH SCORE //
					//					  //
					if (snek1[pt].snek_length > highestCurrentLength) {		//update current highest length
						highestCurrentLength = snek1[pt].snek_length;

						if (highestCurrentLength > highScore) {				//update high score
							highScore = highestCurrentLength;

							if (highScore == oldHighScore + 1) {
								gotNewHighScore = true;
								gotNewHighScoreSoundPlay = true;
							}
						}
					}
				}
			}
			
			  //				//
			 // CREATE PORTALS //
			//				  //
			if (highestCurrentLength == 14 && portalCount < 1) {
				portalCount++;

				bool portalsFarEnoughApart = false;
				while (!portalsFarEnoughApart) {

					for (int e = 0; e == 0;) {
						portalCoordinates[0][0] = (rand() % 21) + 2;
						portalCoordinates[0][1] = (rand() % 21) + 2;

						if (portalCoordinates[0][0] != snek1[0].snek_head[0] && portalCoordinates[0][1] != snek1[0].snek_head[1]) {
							if (display[portalCoordinates[0][0]][portalCoordinates[0][1]] != '7' && display[portalCoordinates[0][0]][portalCoordinates[0][1]] != 'X' && display[portalCoordinates[0][0]][portalCoordinates[0][1]] != 'o' && display[portalCoordinates[0][0]][portalCoordinates[0][1]] != 'p') {
								e = 1;
							}
						}
					}

					for (int e = 0; e == 0;) {
						portalCoordinates[1][0] = (rand() % 21) + 2;
						portalCoordinates[1][1] = (rand() % 21) + 2;

						if (portalCoordinates[1][0] != snek1[0].snek_head[0] && portalCoordinates[1][1] != snek1[0].snek_head[1]) {
							if (display[portalCoordinates[1][0]][portalCoordinates[1][1]] != '7' && display[portalCoordinates[1][0]][portalCoordinates[1][1]] != 'X' && display[portalCoordinates[1][0]][portalCoordinates[1][1]] != 'o' && display[portalCoordinates[1][0]][portalCoordinates[1][1]] != 'p') {
								e = 1;
							}
						}
					}

					if (abs(portalCoordinates[0][0] - portalCoordinates[1][0]) + abs(portalCoordinates[0][1] - portalCoordinates[1][1]) > 14) {
						portalsFarEnoughApart = true;
					}
				}

			}

			  //							 //
			 // PASS PLAYER THROUGH PORTALS //
			//							   //
			for (int pt = 0; pt < playerCount; pt++) {
				for (int hh = 0; hh < portalCount * 2; hh++) {
					if (snek1[pt].snek_head[0] == portalCoordinates[hh][0] && snek1[pt].snek_head[1] == portalCoordinates[hh][1]) {
						snek1[pt].snek_head[0] = portalCoordinates[((hh + 1) % 2)][0];
						snek1[pt].snek_head[1] = portalCoordinates[((hh + 1) % 2)][1];	
						break;
					}
				}
			}

			  //								 //
			 // PLACE PLAYER INTO DISPLAY ARRAY //
			//								   //
			for (int pt = 0; pt < playerCount; pt++) {
				display[snek1[pt].snek_head[0]][snek1[pt].snek_head[1]] = '8';
			}

			  //								//
			 // PLACE FRUIT INTO DISPLAY ARRAY //
			//								  //
			display[currentFruit[0]][currentFruit[1]] = '+';

			
			  //				  //
			 // AUDIO PROCESSING //
			//					//		
			
			if (simpleSound) {
				snekAudioSystem.startEventInstance("Instruments+FX/SnakeMove");
			}

			else {
				if (gameLose) {					//play the death sound if the game is lost
					snekAudioSystem.stopEventInstance(bpmNames[currentChordBPM], true);
					snekAudioSystem.startEventInstance("Instruments+FX/Death");
				}

				if (gotNewFruit) {				//if someone gets a new fruit..
					for (int pt = 0; pt < playerCount; pt++) {		//..check each player..
						if (snek1[pt].justGotNewFruit) {			//..to see if they were the one who got the new fruit..					
							if (gotNewHighScoreSoundPlay) {
								snekAudioSystem.startEventInstance("Instruments+FX/newHighScore");
								gotNewHighScoreSoundPlay = false;
							}
							else if (snek1[pt].snek_length == 11) {		//..if they did get a fruit, see if they just got their 11th fruit..
								snekAudioSystem.startEventInstance("Instruments+FX/SnakeFruit11");		//..if they did, then play the 11th fruit sound..
							}
							else if (gotNewFruit && (i16thNote == 3 || i16thNote == 7 || i16thNote == 11 || i16thNote == 15)) {
								snekAudioSystem.startEventInstance("Instruments+FX/Triangle");			//if they got a fruit on an offbeat, play triangle sound
							}
							else if (i16thNote == 5 || i16thNote == 13) {
								snekAudioSystem.startEventInstance("Instruments+FX/Snare1");			//if they got a fruit on an offbeat, play triangle sound
							}
							else if (i16thNote == 1) {
								snekAudioSystem.startEventInstance("Instruments+FX/KickTight");
								snekAudioSystem.startEventInstance("Instruments+FX/Triangle");
								snekAudioSystem.startEventInstance("Instruments+FX/SnakeFruit");
							}
							else {	//..otherwise, play the default fruit eating sound
								snekAudioSystem.startEventInstance("Instruments+FX/SnakeFruit");
							}
						}
					}

					switch (highestCurrentLength) {					//update reverb level and max timeline position from the current highest length
					case 1:
						snekAudioSystem.stopEventInstance(bpmNames[0], true);
						chordsStartToggle = true;
						currentChordBPM = 1;
						if (switchChordsCounter == 0) {
							switchChordsCounter = 1;
						}
						break;

					case 7:
						snekAudioSystem.fmodEventInstances["Instruments+FX/arp"]->setParameterByName("ArpVolume", 0.09f);
						snekAudioSystem.fmodResult = snekAudioSystem.fmodSystem->setParameterByName("ChordsReverb", 0.7f);
						currentChordBPM = 2;
						if (switchChordsCounter == 1) {
							switchChords = true;
							switchChordsCounter = 7;
						}
						break;

					case 11:
						snekMoveTimelinePositionMax += 200;
						snakeMoveReverbLevel = 0.125f;
						snekAudioSystem.fmodEventInstances["Instruments+FX/Snare1"]->setParameterByName("SnareReverb", 0.2f);
						snekAudioSystem.fmodEventInstances["Instruments+FX/arp"]->setParameterByName("ArpVolume", 0.14f);
						snekAudioSystem.fmodEventInstances["Instruments+FX/SnakeMove"]->setParameterByName("SnakeMoveVolume", 1.0f);
						snekAudioSystem.fmodEventInstances["Instruments+FX/Triangle"]->setParameterByName("TriangleDecay", 0.9f);
						snekAudioSystem.fmodResult = snekAudioSystem.fmodSystem->setParameterByName("ChordsReverb", 1.0f);
						hiHatToggle = true;
						currentChordBPM = 3;
						if (switchChordsCounter == 7) {
							switchChords = true;
							switchChordsCounter = 11;
						}
						break;

					case 20:
						snekMoveTimelinePositionMax += 200;
						snakeMoveReverbLevel = 0.250f;
						snekAudioSystem.fmodEventInstances["Instruments+FX/Snare1"]->setParameterByName("SnareReverb", 0.4f);
						snekAudioSystem.fmodEventInstances["Instruments+FX/arp"]->setParameterByName("ArpVolume", 0.17f);
						snekAudioSystem.fmodEventInstances["Instruments+FX/SnakeMove"]->setParameterByName("SnakeMoveVolume", 0.9f);
						snekAudioSystem.fmodEventInstances["Instruments+FX/Triangle"]->setParameterByName("TriangleDecay", 0.8f);
						currentChordBPM = 4;
						if (switchChordsCounter == 11) {
							switchChords = true;
							switchChordsCounter = 20;
						}
						break;

					case 30:
						snekMoveTimelinePositionMax += 200;
						snakeMoveReverbLevel = 0.375f;
						snekAudioSystem.fmodEventInstances["Instruments+FX/Snare1"]->setParameterByName("SnareReverb", 0.5f);
						snekAudioSystem.fmodEventInstances["Instruments+FX/arp"]->setParameterByName("ArpVolume", 0.2f);
						snekAudioSystem.fmodEventInstances["Instruments+FX/SnakeMove"]->setParameterByName("SnakeMoveVolume", 0.88f);
						snekAudioSystem.fmodEventInstances["Instruments+FX/Triangle"]->setParameterByName("TriangleDecay", 0.5f);
						snekAudioSystem.fmodResult = snekAudioSystem.fmodSystem->setParameterByName("ChordsReverb", 1.0f);
						currentChordBPM = 5;
						if (switchChordsCounter == 20) {
							switchChords = true;
							switchChordsCounter = 30;
						}
						break;

					case 40:
						snekMoveTimelinePositionMax += 200;
						snakeMoveReverbLevel = 0.5f;
						snekAudioSystem.fmodEventInstances["Instruments+FX/Snare1"]->setParameterByName("SnareReverb", 0.64f);
						snekAudioSystem.fmodEventInstances["Instruments+FX/arp"]->setParameterByName("ArpVolume", 0.3f);
						snekAudioSystem.fmodEventInstances["Instruments+FX/SnakeMove"]->setParameterByName("SnakeMoveVolume", 0.4f);
						snekAudioSystem.fmodEventInstances["Instruments+FX/Triangle"]->setParameterByName("TriangleDecay", 0.25f);
						snekAudioSystem.fmodResult = snekAudioSystem.fmodSystem->setParameterByName("ChordsReverb", 0.7f);
						currentChordBPM = 6;
						if (switchChordsCounter == 30) {
							switchChords = true;
							switchChordsCounter = 40;
						}
						break;

					case 50:
						snekMoveTimelinePositionMax += 200;
						snakeMoveReverbLevel = 0.625f;
						snekAudioSystem.fmodEventInstances["Instruments+FX/Snare1"]->setParameterByName("SnareReverb", 0.72f);
						snekAudioSystem.fmodEventInstances["Instruments+FX/arp"]->setParameterByName("ArpVolume", 0.45f);
						snekAudioSystem.fmodEventInstances["Instruments+FX/SnakeMove"]->setParameterByName("SnakeMoveVolume", 0.2f);
						snekAudioSystem.fmodEventInstances["Instruments+FX/Triangle"]->setParameterByName("TriangleDecay", 0.15f);
						snekAudioSystem.fmodResult = snekAudioSystem.fmodSystem->setParameterByName("ChordsReverb", 0.3f);
						currentChordBPM = 7;
						if (switchChordsCounter == 40) {
							switchChords = true;
							switchChordsCounter = 50;
						}
						break;

					case 60:
						snekMoveTimelinePositionMax += 200;
						snakeMoveReverbLevel = 0.750f;
						snekAudioSystem.fmodEventInstances["Instruments+FX/Snare1"]->setParameterByName("SnareReverb", 0.8f);
						snekAudioSystem.fmodEventInstances["Instruments+FX/arp"]->setParameterByName("ArpVolume", 0.45f);
						snekAudioSystem.fmodEventInstances["Instruments+FX/SnakeMove"]->setParameterByName("SnakeMoveVolume", 0.4f);
						snekAudioSystem.fmodEventInstances["Instruments+FX/Triangle"]->setParameterByName("TriangleDecay", 0.1f);
						snekAudioSystem.fmodResult = snekAudioSystem.fmodSystem->setParameterByName("ChordsReverb", 0.0f);
						currentChordBPM = 8;
						if (switchChordsCounter == 50) {
							switchChords = true;
							switchChordsCounter = 60;
						}
						break;

					case 70:
						snekMoveTimelinePositionMax += 200;
						snakeMoveReverbLevel = 0.875f;
						snekAudioSystem.fmodEventInstances["Instruments+FX/Snare1"]->setParameterByName("SnareReverb", 0.4f);
						snekAudioSystem.fmodEventInstances["Instruments+FX/arp"]->setParameterByName("ArpVolume", 0.35f);
						snekAudioSystem.fmodEventInstances["Instruments+FX/SnakeMove"]->setParameterByName("SnakeMoveVolume", 0.6f);
						snekAudioSystem.fmodEventInstances["Instruments+FX/Triangle"]->setParameterByName("TriangleDecay", 0.05f);
						currentChordBPM = 9;
						if (switchChordsCounter == 60) {
							switchChords = true;
							switchChordsCounter = 70;
						}
						break;

					case 80:
						snekMoveTimelinePositionMax += 200;
						snakeMoveReverbLevel = 1.0f;
						snekAudioSystem.fmodEventInstances["Instruments+FX/Snare1"]->setParameterByName("SnareReverb", 0.2f);
						snekAudioSystem.fmodEventInstances["Instruments+FX/arp"]->setParameterByName("ArpVolume", 0.20f);
						snekAudioSystem.fmodEventInstances["Instruments+FX/Triangle"]->setParameterByName("TriangleDecay", 0.05f);
						currentChordBPM = 10;
						if (switchChordsCounter == 70) {
							switchChords = true;
							switchChordsCounter = 80;
						}
						break;

					case 90:
						currentChordBPM = 11;
						snekAudioSystem.fmodEventInstances["Instruments+FX/arp"]->setParameterByName("ArpVolume", 0.0f);
						snekAudioSystem.fmodEventInstances["Instruments+FX/Snare1"]->setParameterByName("SnareReverb", 0.0f);
						if (switchChordsCounter == 80) {
							switchChords = true;
							switchChordsCounter = 90;
						}
						break;
					}
				}
				//if nobody got any fruits, then check if either snake is using the action keys..
				else if (snek1[0].action_keys || snek1[1].action_keys) {
				snekAudioSystem.fmodEventInstances["Instruments+FX/SnakeLunge"]->setPitch(proximityToFruit);
				snekAudioSystem.startEventInstance("Instruments+FX/SnakeLunge");
				snekAudioSystem.fmodEventInstances["Instruments+FX/SnakeMove"]->setParameterByName("Reverb Wet", 1.0f);		//set the reverb level high for the move sound
					if (!actionKeyHeld) {
						snekMoveTimelinePosition = (200 + snekMoveTimelinePositionMax);
						actionKeyHeld = true;
					}
				}				

				//if nobody got a fruit, and nobody is holding any action keys, then..
				else {

					if (highestCurrentLength == 0) {
						snekAudioSystem.fmodEventInstances[bpmNames[0]]->setParameterByName("HeartRateDryLevel", proximityToFruit);
					}

					actionKeyHeld = false;		//nobody is holding any action keys anymore

					snekAudioSystem.fmodEventInstances["Instruments+FX/SnakeMove"]->setPitch(proximityToFruit);
					snekAudioSystem.fmodEventInstances["Instruments+FX/SnakeMove"]->setTimelinePosition(snekMoveTimelinePosition);
					snekAudioSystem.fmodEventInstances["Instruments+FX/SnakeMove"]->setParameterByName("Reverb Wet", snakeMoveReverbLevel);										
					snekAudioSystem.startEventInstance("Instruments+FX/SnakeMove");
					

					snekMoveTimelinePosition += 200;
					if (snekMoveTimelinePosition >= snekMoveTimelinePositionMax) {
						snekMoveTimelinePosition = 0;
					}
				}

				//CHORDS//			
				if (chordsStartToggle) {
					if (i16thNote == 1) {
						switch (currentChord) {
						case 1:
							currentChord++;
							break;
						case 2:
							currentChord++;
							break;
						case 3:
							currentChord++;
							break;
						case 4:
							currentChord = 1;
							break;
						}
					}

					if (!hasFirstSwitchHappened && currentChordBPM == 1) {
						snekAudioSystem.startEventInstance(bpmNames[1]);
						i16thNote = 1;
						currentChord = 1;
						hasFirstSwitchHappened = true;
					}

					if (switchChordsCounter > 6 && currentChordBPM > 0) {

						if (switchChords == true) {
							int oldPlaybackPosition;
							int newPlaybackPosition;
							snekAudioSystem.fmodEventInstances[bpmNames[currentChordBPM - 1]]->getTimelinePosition(&oldPlaybackPosition);
							snekAudioSystem.stopEventInstance(bpmNames[currentChordBPM - 1]);

							newPlaybackPosition = (oldPlaybackPosition / (60000.0f / bpmValues[currentChordBPM - 1])) * (60000.0f / bpmValues[currentChordBPM]);

							snekAudioSystem.startEventInstance(bpmNames[currentChordBPM]);
							snekAudioSystem.fmodEventInstances[bpmNames[currentChordBPM]]->setTimelinePosition(newPlaybackPosition);
						}

						switchChords = false;
					}

					if (currentChord == 1 && i16thNote == 1) {
						snekAudioSystem.startEventInstance(bpmNames[currentChordBPM]);
					}

					//ARP//
					switch (currentChord) {
					case 1:
						switch (i16thNote) {
						case 1:
							snekAudioSystem.startEventInstance("Instruments+FX/arp");
							break;
						case 3:
							snekAudioSystem.fmodEventInstances["Instruments+FX/arp"]->setTimelinePosition(170);
							break;
						case 5:
							snekAudioSystem.fmodEventInstances["Instruments+FX/arp"]->setTimelinePosition(338);
							break;
						case 7:
							snekAudioSystem.fmodEventInstances["Instruments+FX/arp"]->setTimelinePosition(507);
							break;
						case 9:
							snekAudioSystem.fmodEventInstances["Instruments+FX/arp"]->setTimelinePosition(675);
							break;
						case 11:
							snekAudioSystem.fmodEventInstances["Instruments+FX/arp"]->setTimelinePosition(844);
							break;
						case 13:
							snekAudioSystem.fmodEventInstances["Instruments+FX/arp"]->setTimelinePosition(1012);
							break;
						case 15:
							snekAudioSystem.fmodEventInstances["Instruments+FX/arp"]->setTimelinePosition(1181);
							break;
						}
						break;

					case 2:
						switch (i16thNote) {
						case 1:
							snekAudioSystem.fmodEventInstances["Instruments+FX/arp"]->setTimelinePosition(2023);
							break;
						case 3:
							snekAudioSystem.fmodEventInstances["Instruments+FX/arp"]->setTimelinePosition(2192);
							break;
						case 5:
							snekAudioSystem.fmodEventInstances["Instruments+FX/arp"]->setTimelinePosition(2361);
							break;
						case 7:
							snekAudioSystem.fmodEventInstances["Instruments+FX/arp"]->setTimelinePosition(2529);
							break;
						case 9:
							snekAudioSystem.fmodEventInstances["Instruments+FX/arp"]->setTimelinePosition(2698);
							break;
						case 11:
							snekAudioSystem.fmodEventInstances["Instruments+FX/arp"]->setTimelinePosition(2866);
							break;
						case 13:
							snekAudioSystem.fmodEventInstances["Instruments+FX/arp"]->setTimelinePosition(3035);
							break;
						case 15:
							snekAudioSystem.fmodEventInstances["Instruments+FX/arp"]->setTimelinePosition(3203);
							break;
						}
						break;

					case 3:
						switch (i16thNote) {
						case 1:
							snekAudioSystem.fmodEventInstances["Instruments+FX/arp"]->setTimelinePosition(4046);
							break;
						case 3:
							snekAudioSystem.fmodEventInstances["Instruments+FX/arp"]->setTimelinePosition(4214);
							break;
						case 5:
							snekAudioSystem.fmodEventInstances["Instruments+FX/arp"]->setTimelinePosition(4383);
							break;
						case 7:
							snekAudioSystem.fmodEventInstances["Instruments+FX/arp"]->setTimelinePosition(4552);
							break;
						case 9:
							snekAudioSystem.fmodEventInstances["Instruments+FX/arp"]->setTimelinePosition(4720);
							break;
						case 11:
							snekAudioSystem.fmodEventInstances["Instruments+FX/arp"]->setTimelinePosition(4889);
							break;
						case 13:
							snekAudioSystem.fmodEventInstances["Instruments+FX/arp"]->setTimelinePosition(5057);
							break;
						case 15:
							snekAudioSystem.fmodEventInstances["Instruments+FX/arp"]->setTimelinePosition(5226);
							break;
						}
						break;

					case 4:
						switch (i16thNote) {
						case 1:
							snekAudioSystem.fmodEventInstances["Instruments+FX/arp"]->setTimelinePosition(6068);
							break;
						case 3:
							snekAudioSystem.fmodEventInstances["Instruments+FX/arp"]->setTimelinePosition(6237);
							break;
						case 5:
							snekAudioSystem.fmodEventInstances["Instruments+FX/arp"]->setTimelinePosition(6405);
							break;
						case 7:
							snekAudioSystem.fmodEventInstances["Instruments+FX/arp"]->setTimelinePosition(6574);
							break;
						case 9:
							snekAudioSystem.fmodEventInstances["Instruments+FX/arp"]->setTimelinePosition(6743);
							break;
						case 11:
							snekAudioSystem.fmodEventInstances["Instruments+FX/arp"]->setTimelinePosition(6911);
							break;
						case 13:
							snekAudioSystem.fmodEventInstances["Instruments+FX/arp"]->setTimelinePosition(7080);
							break;
						case 15:
							snekAudioSystem.fmodEventInstances["Instruments+FX/arp"]->setTimelinePosition(7248);
							break;
						}
						break;
					}

				}

				//Update 16th note counter//			
				i16thNote++;
				if (i16thNote == 17) {
					i16thNote = 1;
				}
			}
			
			snekAudioSystem.fmodUpdate(); //update FMOD system	
						
			  //			 //
			 // DRAW SCREEN //	
			//			   //
			for (int t = 0; t < 25; t++) {
				for (int q = 0; q < 25; q++) {

					if (display[q][t] == 'z') {
						textGraphics.drawText(q, t, L" ");

					}

					else if (display[q][t] == '7') {
						textGraphics.drawText(q, t, L"8");
					}

					else if (display[q][t] == '8' && snek1[0].direction_frame == 'n') {
						textGraphics.drawText(q, t, L"^");
					}

					else if (display[q][t] == '8' && snek1[0].direction_frame == 's') {
						textGraphics.drawText(q, t, L"v");
					}

					else if (display[q][t] == '8' && snek1[0].direction_frame == 'w') {
						textGraphics.drawText(q, t, L"<");
					}

					else if (display[q][t] == '8' && snek1[0].direction_frame == 'e') {
						textGraphics.drawText(q, t, L">");
					}

					else if (display[q][t] == 'p') {
						textGraphics.drawText(q, t, L"O");
					}

					else {
						textGraphics.drawText(q, t, L"+");
					}
				}
			}

			textGraphics.fillColor(colorPalette.hud, 25, 0, 25, textGraphics.height - 1);
			textGraphics.fillText(25, 0, 25, textGraphics.height - 1, L'|');

			textGraphics.drawTextSprite(32, 0, TextSprite(
				L"       __    _    _              _  __   ____\n"
				L"      / /   | \\  | |     /\\     | |/ /  |  __|\n"
				L"      \\ \\   |  \\ | |    /  \\    | | /   | |__\n"
				L"       \\ \\  | | \\| |   / /\\ \\   |   \\   |  __|\n"
				L"       / /  | |\\ \\ |  /  __  \\  | |\\ \\  | |__\n"
				L"      /_/   |_| \\__| /__/  \\__\\ |_| \\_\\ |____|",
				colorPalette.hud
			));

			if (playerCount == 1) {
				textGraphics.drawText(33, 7, L"SCORE: " + std::to_wstring(highestCurrentLength) );
			}
			else {
				textGraphics.drawText(30, 7, L"P1 SCORE: " + std::to_wstring(snek1[0].snek_length));
				textGraphics.drawText(30, 8, L"P2 SCORE: " + std::to_wstring(snek1[1].snek_length));
			}

			textGraphics.drawText(47, 7, L"HIGH SCORE: " + std::to_wstring(highScore));

			textGraphics.drawText(33, 9, L"STYLE: " + std::to_wstring(styleCounter));
			textGraphics.drawText(47, 9, L"HIGH STYLE: " + std::to_wstring(styleHighScore));

			for (int i = 0; i < playerCount; i++) {
				WORD playerColor = i == 0 ? colorPalette.player_1 : colorPalette.player_2;
				
				if (snek1[i].justDied) {
					textGraphics.drawTextSprite(snek1[i].snek_head[0], snek1[i].snek_head[1], TextSprite(L"X", colorPalette.white));
				}
				else {
					switch (snek1[i].direction_frame) {
					case 'n':
						textGraphics.drawTextSprite(snek1[i].snek_head[0], snek1[i].snek_head[1], TextSprite(L"^", playerColor));
						break;
					case 's':
						textGraphics.drawTextSprite(snek1[i].snek_head[0], snek1[i].snek_head[1], TextSprite(L"v", playerColor));
						break;
					case 'e':
						textGraphics.drawTextSprite(snek1[i].snek_head[0], snek1[i].snek_head[1], TextSprite(L">", playerColor));
						break;
					case 'w':
						textGraphics.drawTextSprite(snek1[i].snek_head[0], snek1[i].snek_head[1], TextSprite(L"<", playerColor));
						break;
					}
				}				
			}

			if (portalCount) {
				textGraphics.drawTextSprite(portalCoordinates[0][0], portalCoordinates[0][1], TextSprite(L"O", colorPalette.portal));
				textGraphics.drawTextSprite(portalCoordinates[1][0], portalCoordinates[1][1], TextSprite(L"O", colorPalette.portal));
			}

			if (wasPreviousHighScoreFound) {
				textGraphics.drawTextSprite(63, 6, TextSprite(
					L" -------------\n"
					L"|             |\n"
					L" -------------",
					colorPalette.hud));
				textGraphics.drawText((11 - highScoreName.length()) / 2 + 65, 7, highScoreName);
			}			

			if (playerCount == 2 && currentFrame == 1) {

				textGraphics.drawTextSprite(14, 8, TextSprite(
					L"Player 1\n"
					L"   |    \n"
					L"   |    \n"
					L"   V    ",
					colorPalette.player_1
				));

				textGraphics.drawTextSprite(4, 8, TextSprite(
					L"Player 2\n"
					L"   |    \n"
					L"   |    \n"
					L"   V    ",
					colorPalette.player_2
				));
			}

			  //				  //
			 // COLOR THE SCREEN //
			//				    //

			//reset entire display color to green//
			//drawColor(colorPalette.green, attributes, 0, 0, nScreenWidth - 1, nScreenHeight - 1);
			/*for (int yt = 0; yt < nScreenWidth * nScreenHeight; yt++) {		
				attributes[yt] = FOREGROUND_GREEN;
			}*/

			  //					  //
			 // COLOR THE FRUIT PINK //
			//						//
			textGraphics.fillColor(colorPalette.fruit, currentFruit[0], currentFruit[1], currentFruit[0], currentFruit[1]);

			  //					  //
			 // COLOR PLAYER 1 GREEN //
			//						//			
			
			if (!snek1[0].justDied) {
				textGraphics.attributeBuffer[snek1[0].snek_head[0] + (snek1[0].snek_head[1] * 80)] = colorPalette.player_1;

				if (!snek1[0].justGotNewFruit) {
					for (int l = 0; l < snek1[0].snek_length; l++) {
						textGraphics.attributeBuffer[snek1[0].snek_body[l][0] + (snek1[0].snek_body[l][1] * 80)] = colorPalette.player_1;
					}
					if (snek1[0].snekSwallowTimer <= snek1[0].snek_length) {
						textGraphics.attributeBuffer[snek1[0].snek_body[snek1[0].snekSwallowTimer - 1][0] + (snek1[0].snek_body[snek1[0].snekSwallowTimer - 1][1] * 80)] = colorPalette.fruit_swallowed;
						snek1[0].snekSwallowTimer++;
					}
				}
				else {
					snek1[0].justGotNewFruit = false;
					for (int l = 0; l < snek1[0].snek_length - 1; l++) {
						textGraphics.attributeBuffer[snek1[0].snek_body[l][0] + (snek1[0].snek_body[l][1] * 80)] = colorPalette.player_1;
					}
					if (snek1[0].snekSwallowTimer == 0) {
						textGraphics.attributeBuffer[snek1[0].snek_head[0] + (snek1[0].snek_head[1] * 80)] = colorPalette.fruit_swallowed;
						snek1[0].snekSwallowTimer++;
					}
				}
			}
			else {
				textGraphics.attributeBuffer[snek1[0].snek_head[0] + (snek1[0].snek_head[1] * 80)] = colorPalette.white;
				for (int l = 0; l < snek1[0].snek_length - 1; l++) {
					textGraphics.attributeBuffer[snek1[0].snek_body[l][0] + (snek1[0].snek_body[l][1] * 80)] = colorPalette.white;
				}
			}
				
			  //					//
			 // COLOR PLAYER 2 RED //
			//					  //
			if (playerCount == 2) {
				if (!snek1[1].justDied) {
					textGraphics.attributeBuffer[snek1[1].snek_head[0] + (snek1[1].snek_head[1] * 80)] = colorPalette.player_2;

					if (!snek1[1].justGotNewFruit) {
						for (int l = 0; l < snek1[1].snek_length; l++) {
							textGraphics.attributeBuffer[snek1[1].snek_body[l][0] + (snek1[1].snek_body[l][1] * 80)] = colorPalette.player_2;
						}
						if (snek1[1].snekSwallowTimer <= snek1[1].snek_length) {
							textGraphics.attributeBuffer[snek1[1].snek_body[snek1[1].snekSwallowTimer - 1][0] + (snek1[1].snek_body[snek1[1].snekSwallowTimer - 1][1] * 80)] = colorPalette.fruit_swallowed;
							snek1[1].snekSwallowTimer++;
						}
					}
					else {
						snek1[1].justGotNewFruit = false;
						for (int l = 0; l < snek1[1].snek_length - 1; l++) {
							textGraphics.attributeBuffer[snek1[1].snek_body[l][0] + (snek1[1].snek_body[l][1] * 80)] = colorPalette.player_2;
						}
						if (snek1[1].snekSwallowTimer == 0) {
							textGraphics.attributeBuffer[snek1[1].snek_head[0] + (snek1[1].snek_head[1] * 80)] = colorPalette.fruit_swallowed;
							snek1[1].snekSwallowTimer++;
						}
					}
				}
				else {
					textGraphics.attributeBuffer[snek1[1].snek_head[0] + (snek1[1].snek_head[1] * 80)] = colorPalette.white;
					for (int l = 0; l < snek1[1].snek_length - 1; l++) {
						textGraphics.attributeBuffer[snek1[1].snek_body[l][0] + (snek1[1].snek_body[l][1] * 80)] = colorPalette.white;
					}
				}
			}			
			
			textOutputCMD.pushOutput(textGraphics);
						
			// delay for first frame if in 2 player mode //
			if (playerCount == 2 && currentFrame == 1)
			std::this_thread::sleep_for(std::chrono::milliseconds(3000));
		}

		  //				  //
		 // GAME OVER SCREEN //
		//					//

		snekAudioSystem.stopEventInstance(bpmNames[currentChordBPM]);
		snekAudioSystem.fmodUpdate(); //update FMOD system	

		std::this_thread::sleep_for(std::chrono::milliseconds(700));

		  //			//
		 // NAME ENTRY //
		//			  //

		//if a player got a new high score...
		if (gotNewHighScore) {
			bool nameEntry = true;
			int currentSelChar = 0;
			bool holdNameEntryLeft = false;
			bool holdNameEntryRight = false;
			bool holdNameEntryUp = false;
			bool holdNameEntryDown = false;
			bool holdNameEntryZ = false;
			highScoreName.resize(0);
			snekAudioSystem.fmodEventInstances["Instruments+FX/SnakeFruit"]->setVolume(1.0f);

			
			textGraphics.drawText(32, 9, L"NEW HIGH SCORE! ENTER YOUR NAME:");
			textGraphics.fillText(65, 7, 76, 7, L' ');

			//..display the keyboard..
			for (int y = 0; y < 32; y++) {
				if (y < 8) {
					textGraphics.textBuffer[40 + (13 * 80) + (y * 2)] = keyboard[y];
				}
				else if (y < 16) {
					textGraphics.textBuffer[24 + (15 * 80) + (y * 2)] = keyboard[y];
				}
				else if (y < 24) {
					textGraphics.textBuffer[8 + (17 * 80) + (y * 2)] = keyboard[y];
				}
				else {
					textGraphics.textBuffer[-8 + (19 * 80) + (y * 2)] = keyboard[y];
				}				
			}

			//..and let them enter their name..
			while (nameEntry) {	

				//CHECK INPUT//
				for (int k = 0; k < 4; k++) {
					snek1[0].directional_keys[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x25\x26\x27\x28"[k]))) != 0;
				}
				snek1[0].action_keys = ((0x8000 & GetAsyncKeyState((unsigned char)("Z"[0]))) != 0);
				
				//APPLY INPUT//

				//..if they press the left arrow key..
				if (!holdNameEntryLeft && snek1[0].directional_keys[0]) {
					if (currentSelChar - 1 > -1) {
						currentSelChar--;
						snekAudioSystem.fmodEventInstances["Instruments+FX/SnakeFruit"]->setPitch(2.0f);
						snekAudioSystem.startEventInstance("Instruments+FX/SnakeFruit");
					}
					holdNameEntryLeft = true;
				}
				//..if they release the left arrow key..
				else if (!snek1[0].directional_keys[0]) {
					holdNameEntryLeft = false;
				}

				//..if they press the right arrow key..
				if (!holdNameEntryRight && snek1[0].directional_keys[2]) {
					if (currentSelChar + 1 < 32) {
						currentSelChar++;
						snekAudioSystem.fmodEventInstances["Instruments+FX/SnakeFruit"]->setPitch(2.0f);
						snekAudioSystem.startEventInstance("Instruments+FX/SnakeFruit");
					}
					holdNameEntryRight = true;
				}
				//..if they release the right arrow key..
				else if (!snek1[0].directional_keys[2]) {
					holdNameEntryRight = false;
				}

				//..if they press the up arrow key..
				if (!holdNameEntryUp && snek1[0].directional_keys[1]) {
					if (currentSelChar - 8 > -1) {
						currentSelChar -= 8;
						snekAudioSystem.fmodEventInstances["Instruments+FX/SnakeFruit"]->setPitch(2.0f);
						snekAudioSystem.startEventInstance("Instruments+FX/SnakeFruit");
					}
					holdNameEntryUp = true;
				}
				//..if they release the up arrow key..
				else if (!snek1[0].directional_keys[1]) {
					holdNameEntryUp = false;
				}

				//..if they press the down arrow key..
				if (!holdNameEntryDown && snek1[0].directional_keys[3]) {
					if (currentSelChar + 8 < 32) {
						currentSelChar += 8;
						snekAudioSystem.fmodEventInstances["Instruments+FX/SnakeFruit"]->setPitch(2.0f);
						snekAudioSystem.startEventInstance("Instruments+FX/SnakeFruit");
					}						
					holdNameEntryDown = true;
				}
				//..if they release the down arrow key..
				else if (!snek1[0].directional_keys[3]) {
					holdNameEntryDown = false;
				}				

				//..if they press the Z key..
				if (!holdNameEntryZ && snek1[0].action_keys) {					
					if (currentSelChar < 29 && highScoreName.length() < 11) {
						highScoreName.append(1, keyboard[currentSelChar]);
						snekAudioSystem.fmodEventInstances["Instruments+FX/SnakeFruit"]->setPitch(1.0f);
						snekAudioSystem.startEventInstance("Instruments+FX/SnakeFruit");
					}
					else if (currentSelChar == 29 && highScoreName.length() < 11 && highScoreName.length() != 0) {
						highScoreName.append(1, keyboard[currentSelChar]);
						snekAudioSystem.fmodEventInstances["Instruments+FX/SnakeFruit"]->setPitch(1.0f);
						snekAudioSystem.startEventInstance("Instruments+FX/SnakeFruit");
					}
					else if (currentSelChar == 30) {
						if (highScoreName.size() > 0) {
							highScoreName.resize(highScoreName.size() - 1);
							snekAudioSystem.fmodEventInstances["Instruments+FX/SnakeFruit"]->setPitch(0.3f);
							snekAudioSystem.startEventInstance("Instruments+FX/SnakeFruit");
						}						
					}
					else if (currentSelChar == 31) {
						nameEntry = false;
						snekAudioSystem.startEventInstance("Instruments+FX/SnakeFruit11");
					}
					
					holdNameEntryZ = true;
				}
				// ..if they release the Z key..
				else if (!snek1[0].action_keys) {
					holdNameEntryZ = false;
				}


				// erase name display
				textGraphics.fillText(65, 7, 76, 7, L' ');


				// display their name as they type it
				textGraphics.drawText(63, 6, 
					L" -------------\n"
					L"|             |\n"
					L" -------------");


				if (highScoreName.length() == 11) {
					textGraphics.textBuffer.replace(65 + (80 * 7), highScoreName.length(), highScoreName);
				}
				else {
					textGraphics.textBuffer.replace(((11 - highScoreName.length()) / 2) + 65 + (80 * 7), highScoreName.length() + 1, highScoreName + L" ");
				}

				//color the whole right side of the screen green
				textGraphics.fillColor(colorPalette.hud, 26, 0, textGraphics.width - 1, textGraphics.height - 1);

				//invert the color of the currently selected keyboard character
				if (currentSelChar < 8) {
					textGraphics.attributeBuffer[40 + (13 * 80) + (currentSelChar * 2)] = colorPalette.keyboard_selected;
				}
				else if (currentSelChar < 16) {
					textGraphics.attributeBuffer[24 + (15 * 80) + (currentSelChar * 2)] = colorPalette.keyboard_selected;
				}
				else if (currentSelChar < 24) {
					textGraphics.attributeBuffer[8 + (17 * 80) + (currentSelChar * 2)] = colorPalette.keyboard_selected;
				}
				else {
					textGraphics.attributeBuffer[-8 + (19 * 80) + (currentSelChar * 2)] = colorPalette.keyboard_selected;
				}

				//DISPLAY THE SCREEN//
				textOutputCMD.pushOutput(textGraphics);

				snekAudioSystem.fmodUpdate(); //update FMOD system	
			}

			  //						  //
			 // WRITE HIGH SCORE TO FILE //
			//							//
			std::ofstream scoreFileWrite;
			scoreFileWrite.open("ScoreFile", std::ios::trunc);
			scoreFileWrite << std::to_string(highScore) << std::endl << converter.to_bytes(highScoreName);
			scoreFileWrite.close();	

			wasPreviousHighScoreFound = true;
			textGraphics.drawText(32, 9, L"           HIGH SCORE SAVED!    ");
		}			

		  //					 //
		 // "TRY AGAIN?" SCREEN //
		//					   //

		textGraphics.fillText(40, 13, 79, 20, L' ');


		// color the whole right side of the screen green
		textGraphics.fillColor(colorPalette.hud, 26, 0, nScreenWidth, nScreenHeight);
				
		Sleep(100);
		
		if (playerCount == 2) {
			textGraphics.drawText(44, 12, L"PLAYER " + std::to_wstring(snek1[0].justDied ? 1 : 2) + L" DIED!");
		}
		else {
			textGraphics.fillText(44, 21, 62, 21, L' ');
		}
		
		textGraphics.fillText(33, 17, 66, 17, L' ');

		snekAudioSystem.startEventInstance("Menu+Songs/FancyBoss");	// (FMOD)
		snekAudioSystem.fmodUpdate();	//begin FMOD sound generation/song playback

		textGraphics.drawText(47, 15, L"GAME OVER");
		textGraphics.drawText(46, 18, L"Players: <" + std::to_wstring(playerCount) + L">");
		textGraphics.drawText(40, 20, L">Press [Z] to play again");
		textGraphics.drawText(40, 21, L">Press [X] to quit");
		//screenString.replace((nScreenHeight * nScreenWidth) - 753, 9, L"GAME OVER");
		//screenString.replace((nScreenHeight * nScreenWidth) - 360, 24, L">Press [Z] to play again");
		//screenString.replace((nScreenHeight * nScreenWidth) - 280, 18, L">Press [X] to quit");
		//screenString.replace((18 * 80) + 46, 12, L"Players: <" + std::to_wstring(playerCount) + L">");

		textOutputCMD.pushOutput(textGraphics);

		bool gameOverMessage = true;

		bool xKey;

		std::this_thread::sleep_for(std::chrono::milliseconds(527));

		while (gameOverMessage) {

			for (int o = 0; o < 4; o++) {
				arrowKeys[o] = (0x8000 & GetAsyncKeyState((unsigned char)("\x25\x26\x27\x28"[o]))) != 0;
			}

			if (arrowKeys[2] && playerCount < 2 && !holdKey) {
				playerCount++; 
				snekAudioSystem.fmodEventInstances["Instruments+FX/SnakeFruit"]->setPitch(1.77f);
				snekAudioSystem.startEventInstance("Instruments+FX/SnakeFruit");
				snekAudioSystem.fmodUpdate();
				holdKey = true;
			}

			else if (arrowKeys[0] && playerCount > 1 && !holdKey) {
				playerCount--;
				snekAudioSystem.fmodEventInstances["Instruments+FX/SnakeFruit"]->setPitch(0.64f);
				snekAudioSystem.startEventInstance("Instruments+FX/SnakeFruit");
				snekAudioSystem.fmodUpdate();
				holdKey = true;
			}

			if (holdKey && !arrowKeys[0] && !arrowKeys[2]) {
				holdKey = false;
			}

			textGraphics.drawText(46, 18, L"Players: <" + std::to_wstring(playerCount) + L">");
			//screenString.replace((18 * 80) + 46, 12, L"Players: <" + std::to_wstring(playerCount) + L">");

			textOutputCMD.pushOutput(textGraphics);
								
			if (zKey = (0x8000 & GetAsyncKeyState((unsigned char)("Z"[0]))) != 0) {
				gameOverMessage = false;
				playAgain = true;
					
				snekAudioSystem.stopEventInstance("Menu+Songs/FancyBoss");		//(FMOD)
				snekAudioSystem.fmodEventInstances["Instruments+FX/SnakeFruit"]->setPitch(1.0f);
				snekAudioSystem.startEventInstance("Instruments+FX/SnakeFruit");
				snekAudioSystem.fmodUpdate();
			}



			if (xKey = (0x8000 & GetAsyncKeyState((unsigned char)("X"[0]))) != 0) {
				gameOverMessage = false;
				playAgain = false;

				snekAudioSystem.stopEventInstance("Menu+Songs/FancyBoss", true);		//(FMOD)
				snekAudioSystem.startEventInstance("Menu+Songs/ExitGame");

				snekAudioSystem.fmodUpdate();
				std::this_thread::sleep_for(std::chrono::milliseconds(2671));
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	}

	while (playAgain);

	return 0;

}