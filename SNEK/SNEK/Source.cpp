//	 __    _    _              _  __   ____
//	/ /   | \  | |     /\     | |/ /  |  __|
//	\ \   |  \ | |    /  \    | | /   | |__
//	 \ \  | | \| |   / /\ \   |   \   |  __|
//	 / /  | |\ \ |  /  __  \  | |\ \  | |__
//	/_/   |_| \__| /__/  \__\ |_| \_\ |____|
// / / BY M. O. MARMALADE / / / / / / / / / / / / /

  //                     //
 // INCLUDES/NAMESPACES //
//                     //

#pragma once
#define NOMINMAX
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

	AudioSystem snekAudioSystem(false);
	snekAudioSystem.loadMasterBank("media/Master.bank");
	snekAudioSystem.loadStringsBank("media/Master.strings.bank");
	snekAudioSystem.loadBank("media/MusicandFX.bank");

	for (std::string name : fmodEventNames) {
		snekAudioSystem.loadEventInstance(name);
	}


	  //			   //
	 // DISPLAY SETUP //
	//			  	 //

	ColorPalette colorPalette;
	ASCIIGraphics asciiGraphics(80, 25);
	ASCIIOutputCMD asciiOutputCMD;


	  //						 //
	 // SPLASH SCREEN ANIMATION //
	//						   //

	snekAudioSystem.startEventInstance("Menu+Songs/SplashJingle");
	snekAudioSystem.fmodUpdate();

	// reset the color of the whole screen
	asciiGraphics.fillColor(colorPalette.standard, 0, 0, asciiGraphics.width - 1, asciiGraphics.height - 1);

	// Draw Splash Screen
	bool animation = true;
	int u = 0;
	int charToOverwrite = 996;
	asciiGraphics.fillColor(colorPalette.bright_cyan, 30, 10, 50, 20);
	while (charToOverwrite <= 1005) {

		asciiGraphics.textBuffer[charToOverwrite] = "Citrus 64"[u];
		charToOverwrite++;
		u++;
		asciiOutputCMD.pushOutput(asciiGraphics);

		std::this_thread::sleep_for(std::chrono::milliseconds(77));
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(222));
	

	//Erase Splash Screen
	animation = true;
	charToOverwrite = 992;	
	while (charToOverwrite <= 1006) {
		
		asciiGraphics.textBuffer[charToOverwrite] = char(32);
		charToOverwrite++;
		
		asciiOutputCMD.pushOutput(asciiGraphics);
		
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

	int playerCount = 1;

	bool holdKey = false;
	while (startScreen) {

		// draw logo
		asciiGraphics.drawTextSprite(19, 7, ASCIISprite(
			" __    _    _              _  __   ____\n"
			"/ /   | \\  | |     /\\     | |/ /  |  __|\n"
			"\\ \\   |  \\ | |    /  \\    | | /   | |__\n"
			" \\ \\  | | \\| |   / /\\ \\   |   \\   |  __|\n"
			" / /  | |\\ \\ |  /  __  \\  | |\\ \\  | |__\n"
			"/_/   |_| \\__| /__/  \\__\\ |_| \\_\\ |____|",
			colorPalette.hud));

		// draw player count
		asciiGraphics.fillColor(colorPalette.grey, 34, 20, 42, 20);
		asciiGraphics.fillColor(colorPalette.white, 43, 20, 45, 20);
		asciiGraphics.drawText(34, 20, "Players: <" + std::to_string(playerCount) + ">");

		if (playerCount == 2) {

			asciiGraphics.fillColor(colorPalette.player_2, 15, 14, 35, 16);
			asciiGraphics.fillColor(colorPalette.player_1, 36, 14, 80, 16);
			asciiGraphics.drawText(15, 14, 
				"--------------------      --------------------------\n"
				"| P2: WASD + B-Key |      | P1: Arrow Keys + P-Key |\n"
				"--------------------      --------------------------");
			
		}
		else {

			asciiGraphics.drawTextSprite(15, 14, ASCIISprite(
				"         --------------------------------           \n"
				"         | Controls: Arrow Keys + Z-Key |           \n"
				"         --------------------------------           ",
				colorPalette.player_1));
		}

		asciiGraphics.drawTextSprite(35, 22, ASCIISprite("Citrus 64", colorPalette.logo)); // draw studio name

		if (startScreenFrameCount == 111) {
			if (startScreenToggle) {
				startScreenToggle = false;

				// draw "Press Z to start" every 111th frame
				ASCIISprite pressStartSprite("Press [Z] to start", colorPalette.press_start);
				asciiGraphics.drawTextSprite(31, 18, pressStartSprite);

				// play snakefruitinstance sound for flashing "press start" button (FMOD)
				snekAudioSystem.fmodEventInstances["Instruments+FX/SnakeFruit"]->setPitch(1.0f);
				snekAudioSystem.fmodEventInstances["Instruments+FX/SnakeFruit"]->setVolume(0.7f);
				snekAudioSystem.startEventInstance("Instruments+FX/SnakeFruit");
			}
			else {
				startScreenToggle = true;
				asciiGraphics.drawText(31, 18, "                  ");
			}

			startScreenFrameCount = 0;
		}

		asciiOutputCMD.pushOutput(asciiGraphics);

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
				asciiGraphics.drawText(31, 18, animFrame.first);
				asciiOutputCMD.pushOutput(asciiGraphics);
				std::this_thread::sleep_for(std::chrono::milliseconds(animFrame.second));
			}
		}

		snekAudioSystem.fmodUpdate();
	}
	
	SnakeGame currentGame(playerCount, 25, 25, &asciiGraphics, &asciiOutputCMD, &snekAudioSystem);
	currentGame.play();
	while (currentGame.playAgain) {
		currentGame = SnakeGame(currentGame.playerCount, 25, 25, &asciiGraphics, &asciiOutputCMD, &snekAudioSystem);
		currentGame.play();
	}

	return 0;

}