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
#include <vector>
#include <iostream>

#include "Soil.h"

#include "SNEKGameSession.h"
#include "SNEKAudioSystem.h"

using namespace std::chrono_literals;

void testFunc() {
	Soil::ASCIIGraphics asciiGraphics(80, 25);
	Soil::ASCIIOutputCMD asciiOutputCMD;

	std::vector<std::chrono::microseconds> frameTimes(120, 0us);
	int i = 0;
	std::chrono::steady_clock::time_point lastFrameTime = std::chrono::steady_clock::now();
	while ((0x8000 & GetAsyncKeyState((unsigned char)('Z'))) == 0) {
		frameTimes[i] = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - lastFrameTime);
		lastFrameTime = std::chrono::steady_clock::now();
		i = (i + 1) % 120;
		std::chrono::microseconds uspf = 0us;
		for (auto& t : frameTimes) {
			uspf += t;
		}
		uspf /= 120.0f;
		float fps = 1000000.0f / uspf.count();

		//asciiGraphics.clearScreen();
		asciiGraphics.drawTextSprite(0, 0, 
		                             Soil::ASCIISprite("FPS: " + std::to_string(fps), 
		                                               Soil::ASCIIColor(Soil::ANSI_4BIT_FG_GREEN, 
		                                                                226, 
		                                                                14, 
		                                                                Soil::ANSITrueColor(255, 0, 0), 
		                                                                Soil::ANSITrueColor(0, 0, 255), 
		                                                                Soil::ANSI_4BIT_COLOR
		                                                               )
		                                              )
		);
		asciiGraphics.drawTextSprite(1, 1,
			Soil::ASCIISprite(U"4-bit Heizölrückstoßabdämpfung",
				Soil::ASCIIColor(Soil::ANSI_4BIT_FG_GREEN | Soil::ANSI_4BIT_FG_BLUE,
					226,
					14,
					Soil::ANSITrueColor(255, 0, 0),
					Soil::ANSITrueColor(0, 0, 255),
					Soil::ANSI_4BIT_COLOR
				)
			)
		);

		asciiGraphics.drawTextSprite(2, 2,
			Soil::ASCIISprite(U"8-bit Ξεσκεπάζω τὴν ψυχοφθόρα βδελυγμία",
				Soil::ASCIIColor(Soil::ANSI_4BIT_FG_GREEN | Soil::ANSI_4BIT_FG_BLUE,
					226,
					14,
					Soil::ANSITrueColor(255, 0, 0),
					Soil::ANSITrueColor(0, 0, 255),
					Soil::ANSI_8BIT_COLOR
				)
			)
		);

		asciiGraphics.drawTextSprite(3, 3,
			Soil::ASCIISprite(U"24-bit ｶいろはにほへとちりぬるを\nイロハニホヘト\nจงฝ่าฟันพัฒนาวิชาการ",
				Soil::ASCIIColor(Soil::ANSI_4BIT_FG_GREEN | Soil::ANSI_4BIT_FG_BLUE,
					226,
					14,
					Soil::ANSITrueColor(255, 0, 0),
					Soil::ANSITrueColor(0, 0, 255),
					Soil::ANSI_24BIT_COLOR
				)
			)
		);

		asciiGraphics.drawTextSprite(0, 10,
			Soil::ASCIISprite(U"ｶｶｶｶｶｶｶｶｶｶｶｶｶｶｶｶｶｶｶｶｶｶｶｶｶｶｶｶｶｶｶｶｶｶｶｶｶｶｶｶｶｶｶｶｶｶｶｶｶｶｶｶｶｶｶｶｶｶｶｶｶｶｶｶｶｶｶｶｶｶｶｶｶｶｶｶｶｶｶｶ",
				Soil::ASCIIColor(Soil::ANSI_4BIT_FG_GREEN | Soil::ANSI_4BIT_FG_BLUE,
					226,
					14,
					Soil::ANSITrueColor(255, 0, 0),
					Soil::ANSITrueColor(0, 0, 255),
					Soil::ANSI_24BIT_COLOR
				)
			)
		);

		asciiGraphics.drawTextSprite(0, 12,
			Soil::ASCIISprite(U"カカカカカカカカカカカカカカカカカカカカカカカカカカカカカカカカカカカカカカカカカカカカカカカカカカカカカカカカカカカカカカカカカカカカカカカカカカカカカカカカ",
				Soil::ASCIIColor(Soil::ANSI_4BIT_FG_GREEN | Soil::ANSI_4BIT_FG_BLUE,
					226,
					14,
					Soil::ANSITrueColor(255, 0, 0),
					Soil::ANSITrueColor(0, 0, 255),
					Soil::ANSI_24BIT_COLOR
				)
			)
		);

		asciiOutputCMD.pushOutput(asciiGraphics, Soil::ANSI_4BIT_COLOR);
	}
}

int main() {

	testFunc();

	// seed the RNG using system time
	srand(time(0));

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

	SNEKAudioSystem snekAudioSystem(false);
	snekAudioSystem.loadMasterBank("media/Master.bank");
	snekAudioSystem.loadStringsBank("media/Master.strings.bank");
	snekAudioSystem.loadBank("media/MusicandFX.bank");

	for (std::string name : fmodEventNames) {
		snekAudioSystem.loadEventInstance(name);
	}


	  //			   //
	 // DISPLAY SETUP //
	//			  	 //

	SNEKColorPalette colorPalette;
	Soil::ASCIIGraphics asciiGraphics(80, 25);
	Soil::ASCIIOutputCMD asciiOutputCMD;


	  //             //
	 // INPUT SETUP //
	//             //
	Soil::InputManager inputManager;


	  //						 //
	 // SPLASH SCREEN ANIMATION //
	//						   //
	snekAudioSystem.startEventInstance("Menu+Songs/SplashJingle");
	snekAudioSystem.fmodUpdate();

	// reset the color of the whole screen
	asciiGraphics.fillColor(0, 0, asciiGraphics.width - 1, asciiGraphics.height - 1, colorPalette.standard);

	// Draw Splash Screen
	asciiGraphics.fillColor(30, 10, 50, 20, colorPalette.bright_cyan);
	std::u32string logoString = U"Citrus 64";
	int startingXCoord = asciiGraphics.width / 2 - (logoString.size() / 2);
	for (int i = 0; i < logoString.size(); i++) {
		asciiGraphics.drawText(startingXCoord + i, asciiGraphics.height / 2, logoString[i]);
		asciiOutputCMD.pushOutput(asciiGraphics, Soil::ANSI_4BIT_COLOR);
		std::this_thread::sleep_for(77ms);
	}

	std::this_thread::sleep_for(222ms);

	//Erase Splash Screen
	for (int i = 0; i < logoString.size(); i++) {
		asciiGraphics.drawText(startingXCoord + i, asciiGraphics.height / 2, U' ');
		asciiOutputCMD.pushOutput(asciiGraphics, Soil::ANSI_4BIT_COLOR);
		std::this_thread::sleep_for(77ms);
	}

	std::this_thread::sleep_for(1s);


	  //			  //
	 // START SCREEN //
	//				//
	bool startScreen = true;
	bool startScreenToggle = true;
	int startScreenFrameCount = 111;

	inputManager.addKeys("Z" + std::string(1,VK_RIGHT) + std::string(1,VK_LEFT));

	snekAudioSystem.startEventInstance("Menu+Songs/ANewChip");	//begin start screen playback	(FMOD)
	snekAudioSystem.fmodUpdate();

	int playerCount = 1;

	bool holdKey = false;
	while (startScreen) {

		// get input state for this frame
		inputManager.getBufferState();

		// draw logo
		asciiGraphics.drawTextSprite(19, 7, Soil::ASCIISprite(
			" __    _    _              _  __   ____\n"
			"/ /   | \\  | |     /\\     | |/ /  |  __|\n"
			"\\ \\   |  \\ | |    /  \\    | | /   | |__\n"
			" \\ \\  | | \\| |   / /\\ \\   |   \\   |  __|\n"
			" / /  | |\\ \\ |  /  __  \\  | |\\ \\  | |__\n"
			"/_/   |_| \\__| /__/  \\__\\ |_| \\_\\ |____|",
			colorPalette.hud));

		// draw player count
		asciiGraphics.fillColor(34, 20, 42, 20, colorPalette.grey);
		asciiGraphics.fillColor(43, 20, 45, 20, colorPalette.white);
		asciiGraphics.drawText(34, 20, "Players: <" + std::to_string(playerCount) + ">");

		if (playerCount == 2) {

			asciiGraphics.fillColor(15, 14, 35, 16, colorPalette.player_2);
			asciiGraphics.fillColor(36, 14, 80, 16, colorPalette.player_1);
			asciiGraphics.drawText(15, 14, 
				"--------------------      --------------------------\n"
				"| P2: WASD + V-Key |      | P1: Arrow Keys + P-Key |\n"
				"--------------------      --------------------------");
			
		}
		else {

			asciiGraphics.drawTextSprite(15, 14, Soil::ASCIISprite(
				"         --------------------------------           \n"
				"         | Controls: Arrow Keys + Z-Key |           \n"
				"         --------------------------------           ",
				colorPalette.player_1));
		}

		asciiGraphics.drawTextSprite(35, 22, Soil::ASCIISprite("Citrus 64", colorPalette.logo)); // draw studio name

		if (startScreenFrameCount == 111) {
			if (startScreenToggle) {
				startScreenToggle = false;

				// draw "Press Z to start" every 111th frame
				Soil::ASCIISprite pressStartSprite("Press [Z] to start", colorPalette.press_start);
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

		asciiOutputCMD.pushOutput(asciiGraphics, Soil::ANSI_4BIT_COLOR);

		startScreenFrameCount++;

		std::this_thread::sleep_for(7ms);

		if (inputManager.isKeyPressed(VK_RIGHT) && playerCount < 2 && !holdKey) {
			playerCount++;
			snekAudioSystem.fmodEventInstances["Instruments+FX/SnakeFruit"]->setPitch(1.77f);
			snekAudioSystem.fmodEventInstances["Instruments+FX/SnakeFruit"]->setVolume(1.0f);
			snekAudioSystem.startEventInstance("Instruments+FX/SnakeFruit");
			holdKey = true;
		}
		else if (inputManager.isKeyPressed(VK_LEFT) && playerCount > 1 && !holdKey) {
			playerCount--;
			snekAudioSystem.fmodEventInstances["Instruments+FX/SnakeFruit"]->setPitch(0.64f);
			snekAudioSystem.fmodEventInstances["Instruments+FX/SnakeFruit"]->setVolume(1.0f);
			snekAudioSystem.startEventInstance("Instruments+FX/SnakeFruit");
			holdKey = true;
		}
		else if (holdKey && !inputManager.isKeyPressed(VK_RIGHT) && !inputManager.isKeyPressed(VK_LEFT)) {
			holdKey = false;
		}

		if (inputManager.isKeyPressed('Z')) {

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
				asciiOutputCMD.pushOutput(asciiGraphics, Soil::ANSI_4BIT_COLOR);
				std::this_thread::sleep_for(std::chrono::milliseconds(animFrame.second));
			}
		}

		snekAudioSystem.fmodUpdate();
	}
	
	SNEKGameOptions options(playerCount, { 23,23 }, colorPalette);

	SNEKGameSession currentGame(options, &asciiGraphics, &asciiOutputCMD, &snekAudioSystem, &inputManager);
	currentGame.play();
	while (currentGame.playAgain) {
		currentGame = SNEKGameSession(options, &asciiGraphics, &asciiOutputCMD, &snekAudioSystem, &inputManager);
		currentGame.play();
	}

	return 0;

}