//		 __    _    _              _  __   ____
//		/ /   | \  | |     /\     | |/ /  |  __|
//		\ \   |  \ | |    /  \    | | /   | |__
//		 \ \  | | \| |   / /\ \   |   \   |  __|
//		 / /  | |\ \ |  /  __  \  | |\ \  | |__
//		/_/   |_| \__| /__/  \__\ |_| \_\ |____|
// / / BY M. O. MARMALADE / / / / / / / / / / / / /

// PROJECT OUTLINE //

// Includes 
// Variable Declarations
// Audio System Setup (FMOD Studio)
// Loading/Preparing Audio Events


  //		  //
 // Includes //
//			//

#include <Windows.h>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <cstdlib>
#include "fmod.hpp"
#include "fmod_studio.hpp"

using namespace std;

  //					   //
 // Variable Declarations //
//						 //

//LOGIC VARIABLES
char display[25][25]{'z'};		//the Play Grid [x][y] {'z' empty space, '8' snek head, '7' snek body, 'o' fruit, 'X' trap, }
int snekBody[625][2];			//the snek's body segments on the play grid [segment][x,y]
bool gameLose;					//current Game Lose state
bool playAgain;					//decides whether or not to play again after losing
int highScore = 0;				//current High Score
int styleHighScore = 0;			//current Style High Score

//INPUT VARIABLES
bool arrowKeys[4];				//stores input from arrow keys
bool zKey;						//stores input from Z key
char direction1 = 's';			//tick-resolution direction of player movement (north = n, south = s, east = e, west = w)
char direction = 's';			//frame-resolution direction of player movement (north = n, south = s, east = e, west = w)
bool holdW = false;				//tick-resolution storage of which arrow keys have been previously held
bool holdE = false;				//"		"
bool holdS = false;				//"		"
bool holdN = false;				//"		"

//DISPLAY VARIABLES
int currentFrame = 0;			//keeps track of how many frames have passed
int currentTick = 0;			//keeps track of how many ticks have passed
int nScreenWidth = 80;			//width of the console window (measured in characters, not pixels)
int nScreenHeight = 25;			//height of the console window (measured in characters, not pixels)

// UNUSED
//int currentTrap = 0;			//current number of traps on the game grid
//int trapLocations[200][2];	//locations of the traps on the game grid
//int r = 4;					//used for counting the number of traps whose locations have been set
//int actualTrapCount;
//bool alternator1 = true;


void SleepinnnThang() {					//framerate for animation that plays after pressing Z to start game
	this_thread::sleep_for(33ms);
}


int main() {
	 //									 //
	// Audio System Setup (FMOD Studio) //
   //								   //
	FMOD_RESULT result;																	//create an FMOD Result
	FMOD::Studio::System* system = NULL;												//create a pointer to a studio system object
	result = FMOD::Studio::System::create(&system);										//create the Studio System object using the pointer
	result = system->initialize(256, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, 0);		//initialize the system for audio playback


	FMOD::Studio::Bank* masterBank = NULL;																
	system->loadBankFile("media/Master.bank", FMOD_STUDIO_LOAD_BANK_NORMAL, &masterBank);

	FMOD::Studio::Bank* stringsBank = NULL;																
	system->loadBankFile("media/Master.strings.bank", FMOD_STUDIO_LOAD_BANK_NORMAL, &stringsBank);
																							
	FMOD::Studio::Bank* musicandFX = NULL;																
	result = system->loadBankFile("media/MusicandFX.bank", FMOD_STUDIO_LOAD_BANK_NORMAL, &musicandFX);
	//										  //
	// TC1.1 - Loading/Preparing Audio Events //
	//										  //
	FMOD::Studio::EventDescription* splashJingleDescription = NULL;			//Splash Jingle (Citrus Studios splash screen)
	system->getEvent("event:/SplashJingle", &splashJingleDescription);

	FMOD::Studio::EventInstance* splashJingleInstance = NULL;
	splashJingleDescription->createInstance(&splashJingleInstance);

	FMOD::Studio::EventDescription* aNewChipDescription = NULL;				//ANewChip
	system->getEvent("event:/ANewChip", &aNewChipDescription);

	FMOD::Studio::EventInstance* aNewChipInstance = NULL;
	aNewChipDescription->createInstance(&aNewChipInstance);

	FMOD::Studio::EventDescription* snakeFruitDescription = NULL;			//SnakeFruit (pickup fruit sound effect)
	system->getEvent("event:/SnakeFruit", &snakeFruitDescription);

	FMOD::Studio::EventInstance* snakeFruitInstance = NULL;
	snakeFruitDescription->createInstance(&snakeFruitInstance);

	FMOD::Studio::EventDescription* snakeFruitDescription11 = NULL;			//SnakeFruit11 (snake eyes score sound)
	system->getEvent("event:/SnakeFruit11", &snakeFruitDescription11);

	FMOD::Studio::EventInstance* snakeFruitInstance11 = NULL;
	snakeFruitDescription11->createInstance(&snakeFruitInstance11);

	FMOD::Studio::EventDescription* snakeMoveDescription = NULL;			//SnakeMove (movement sound effect)
	system->getEvent("event:/SnakeMove", &snakeMoveDescription);

	FMOD::Studio::EventInstance* snakeMoveInstance = NULL;
	snakeMoveDescription->createInstance(&snakeMoveInstance);

	FMOD::Studio::EventDescription* startButtonDescription = NULL;			//StartButton (game start sound effect)
	system->getEvent("event:/StartButton", &startButtonDescription);

	FMOD::Studio::EventInstance* startButtonInstance = NULL;
	startButtonDescription->createInstance(&startButtonInstance);
	
	FMOD::Studio::EventDescription* fancyBossDescription = NULL;			//FancyBoss (game over song)
	system->getEvent("event:/FancyBoss", &fancyBossDescription);

	FMOD::Studio::EventInstance* fancyBossInstance = NULL;
	fancyBossDescription->createInstance(&fancyBossInstance);

	FMOD::Studio::EventDescription* snakeLungeDescription = NULL;			//SnakeLunge (lunge sound effect)
	system->getEvent("event:/SnakeLunge", &snakeLungeDescription);

	FMOD::Studio::EventInstance* snakeLungeInstance = NULL;
	snakeLungeDescription->createInstance(&snakeLungeInstance);
	
	FMOD::Studio::EventDescription* deathDescription = NULL;				//Death (death collision sound)
	system->getEvent("event:/Death", &deathDescription);

	FMOD::Studio::EventInstance* deathInstance = NULL;
	deathDescription->createInstance(&deathInstance);
	
	/*FMOD::Studio::EventDescription* proximitySoundDescription = NULL;
	system->getEvent("event:/ProximitySound", &proximitySoundDescription);

	FMOD::Studio::EventInstance* proximitySoundInstance = NULL;
	proximitySoundDescription->createInstance(&proximitySoundInstance);*/

	  //			   //
	 // Display Setup //
	//			  	 //

	

	//create screen buffer//
	char *screen = new char[nScreenWidth * nScreenHeight];		//create char[] buffer to be used in console buffer updates
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;

	string screenString;								 //character array to be displayed to the screen	
	screenString.resize(nScreenWidth * nScreenHeight);	//set size of screen char array/string//
	
	for (int u = 0; u < (nScreenHeight * nScreenWidth); u++) {	//converts string buffer to char[] buffer to be written to console window
		screen[u] = screenString[u];
	}
	   	
	//disable the cursor visibility//
	CONSOLE_CURSOR_INFO cursorInfo;					
	GetConsoleCursorInfo(hConsole, &cursorInfo);
	cursorInfo.bVisible = false;	
	SetConsoleCursorInfo(hConsole, &cursorInfo);

	/*DWORD fontSize = 0;
	GetConsoleFontSize(hConsole, fontSize);
		
	SMALL_RECT *screenWindowCoordinates = new SMALL_RECT;
	screenWindowCoordinates->Top = 15;
	screenWindowCoordinates->Left = 0;
	screenWindowCoordinates->Bottom = 0;
	screenWindowCoordinates->Right = 79;
	SetConsoleWindowInfo(hConsole, true, screenWindowCoordinates);
	
	CHAR_INFO wAttributes;
	wAttributes.Attributes = FOREGROUND_GREEN;
	SetConsoleTextAttribute(hConsole, wAttributes);*/
							
	splashJingleInstance->start();	//Begin Splash Screen (FMOD)
	system->update();

	  //						 //
	 // Splash Screen Animation //
	//						   //
	bool animation = true;
	int u = 0;
	int charToOverwrite = 992;
	while (animation) {			//Draw Splash Screen

		screen[charToOverwrite] = "Citrus Studios"[u];
		charToOverwrite++;
		u++;
		WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);

		this_thread::sleep_for(77ms);

		if (charToOverwrite == 1006) {
			animation = false;
			this_thread::sleep_for(222ms);
		}
	}	
	
	animation = true;
	charToOverwrite = 992;	
	while (animation) {			//Erase Splash Screen
		
		screen[charToOverwrite] = char(32);
		charToOverwrite++;
		
		WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
		
		this_thread::sleep_for(77ms);
		
		if (charToOverwrite == 1006) {
			animation = false;
			this_thread::sleep_for(222ms);
		}
		
	}

	  //			  //
	 // Start Screen //
	//				//
	bool startScreen = true;
	bool startScreenToggle = true;
	int startScreenFrameCount = 111;

	this_thread::sleep_for(777ms);

	aNewChipInstance->start();	//begin start screen playback	
	system->update();

	while (startScreen) {

		/*for (int u = 0; u < nScreenHeight * nScreenWidth; u++) {	//clear display each frame

			screenString[u] = char(32);

		}*/

		screenString.replace((7 * 80) + 20, 38,   "__    _    _              _  __   ____");		//draw logo each frame
		screenString.replace((8 * 80) + 19, 40,  "/ /   | \\  | |     /\\     | |/ /  |  __|");
		screenString.replace((9 * 80) + 19, 39,  "\\ \\   |  \\ | |    /  \\    | | /   | |__");
		screenString.replace((10 * 80) + 20, 39,  "\\ \\  | | \\| |   / /\\ \\   |   \\   |  __|");
		screenString.replace((11 * 80) + 20, 38,  "/ /  | |\\ \\ |  /  __  \\  | |\\ \\  | |__");
		screenString.replace((12 * 80) + 19, 40, "/_/   |_| \\__| /__/  \\__\\ |_| \\_\\ |____|");

		screenString.replace((22 * 80) + 33, 14, "Citrus Studios");		//draw studio name each frame

		if (startScreenFrameCount == 111) {
			switch (startScreenToggle) {
			case true:
				startScreenToggle = false;
				screenString.replace((18 * 80) + 31, 18, "Press [Z] to start");	//draw "Press Z to start" every 111th frame

				snakeFruitInstance->start();	//play snakefruitinstance sound for flashing "press start" button
				system->update();
				break;
			case false:
				startScreenToggle = true;
				screenString.replace((18 * 80) + 31, 18, "                  ");

				break;
			}

			startScreenFrameCount = 0;
		}

		for (int u = 0; u < (nScreenHeight * nScreenWidth); u++) {	//convert string buffer to char[] buffer
			screen[u] = screenString[u];
		}

		WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);	//display the char[] buffer
					
		startScreenFrameCount++;

		this_thread::sleep_for(7ms);

		if (zKey = (0x8000 & GetAsyncKeyState((unsigned char)("Z"[0]))) != 0) {

			startScreen = false;	//begin to exit start screen when Z key is pressed

			aNewChipInstance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);
			startButtonInstance->start();

			system->update();	//play startbutton sound

			

			//Game Start animation//

			screenString.replace((18 * 80) + 31, 18, "Press [Z] to start");

			for (int u = 0; u < (nScreenHeight * nScreenWidth); u++) {
				screen[u] = screenString[u];

			}

			WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);

			SleepinnnThang();
					   
			screenString.replace((18 * 80) + 31, 18, "Press [Z++to start");

			for (int u = 0; u < (nScreenHeight * nScreenWidth); u++) {
				screen[u] = screenString[u];

			}

			WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);

			SleepinnnThang();

			screenString.replace((18 * 80) + 31, 18, "Press [+AR+o start");

			for (int u = 0; u < (nScreenHeight * nScreenWidth); u++) {
				screen[u] = screenString[u];

			}

			WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);

			SleepinnnThang();

			screenString.replace((18 * 80) + 31, 18, "Press +TART+ start");

			for (int u = 0; u < (nScreenHeight * nScreenWidth); u++) {
				screen[u] = screenString[u];

			}

			WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);

			SleepinnnThang();

			screenString.replace((18 * 80) + 31, 18, "Press+START!+start");

			for (int u = 0; u < (nScreenHeight * nScreenWidth); u++) {
				screen[u] = screenString[u];

			}

			WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);

			SleepinnnThang();

			screenString.replace((18 * 80) + 31, 18, "Pres+ START! +tart");

			for (int u = 0; u < (nScreenHeight * nScreenWidth); u++) {
				screen[u] = screenString[u];

			}

			WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);

			SleepinnnThang();

			screenString.replace((18 * 80) + 31, 18, "Pre+  START!  +art");

			for (int u = 0; u < (nScreenHeight * nScreenWidth); u++) {
				screen[u] = screenString[u];

			}

			WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);

			SleepinnnThang();

			screenString.replace((18 * 80) + 31, 18, "Pr+   START!   +rt");

			for (int u = 0; u < (nScreenHeight * nScreenWidth); u++) {
				screen[u] = screenString[u];

			}

			WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);

			SleepinnnThang();

			screenString.replace((18 * 80) + 31, 18, "P+    START!    +t");

			for (int u = 0; u < (nScreenHeight * nScreenWidth); u++) {
				screen[u] = screenString[u];

			}

			WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);

			SleepinnnThang();

			screenString.replace((18 * 80) + 31, 18, "+     START!     +");

			for (int u = 0; u < (nScreenHeight * nScreenWidth); u++) {
				screen[u] = screenString[u];

			}

			WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);

			SleepinnnThang();

			screenString.replace((18 * 80) + 31, 18, "-                -");

			for (int u = 0; u < (nScreenHeight * nScreenWidth); u++) {
				screen[u] = screenString[u];

			}

			WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);

			this_thread::sleep_for(177ms);

			screenString.replace((18 * 80) + 31, 18, "-     START!     -");

			for (int u = 0; u < (nScreenHeight * nScreenWidth); u++) {
				screen[u] = screenString[u];

			}

			WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);

			this_thread::sleep_for(177ms);

			screenString.replace((18 * 80) + 31, 18, "-                -");

			for (int u = 0; u < (nScreenHeight * nScreenWidth); u++) {
				screen[u] = screenString[u];

			}

			WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);

			this_thread::sleep_for(77ms);

			screenString.replace((18 * 80) + 31, 18, "-     START!     -");

			for (int u = 0; u < (nScreenHeight * nScreenWidth); u++) {
				screen[u] = screenString[u];

			}

			WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);



			//exit

			

		}
	}

	

	this_thread::sleep_for(1389ms);

	//some random maths and stuff//
	//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
	// 40 x 6
	// 25 - 6 = 19
	//19 / 2 = 9.5
	//9 low, 40 in
	//8 low, 39 in

	do {
		  //						  //
		 // PRE-NEW-GAME PREPARATION //
		//						    //		

		srand(time(0));	//seed the RNG using system time
		int currentFruit[2]{ rand() % 25, rand() % 25 };	//location of the current fruit on the game grid [x,y]

		gameLose = false;	//reset game lose condition

		int snekHead[2] = { 12,12 };	//the snek's head position on the play grid [x,y]
		
		int snekLength = 0;	//current length of the snek (used to calculate current Score as well)

		int frameRate = 10;	//frame rate setting

		int styleCounter = 0;	//current STYLE Score

		//currentTrap = 0;
		//r = 0;
		//actualTrapCount = 0;

		for (int x = 0; x < 25; x++) {			//reset the game display
			
			for (int y = 0; y < 25; y++) {
				display[x][y] = 'z';

			}
		}

		for (int n = 0; n < nScreenHeight * nScreenWidth; n++){
			screenString.replace(n, 1, " ");
		}

		for (int u = 0; u < (nScreenHeight * nScreenWidth); u++) {
			screen[u] = char(32);

		}

		//FMOD-Related Variables//
		int snekMoveTimelinePosition = 0;	//FMOD snakemovesound timeline position
		int snekMoveTimelinePositionMax = 200;
		bool isScoreUnder11 = true;
		float snakeMoveReverbLevel = 0.0f;
		float proximityToFruit;
		bool wasZKeyHeld = false;

		  //		   //
		 // GAME LOOP //
		//			 //
		while (gameLose == false) {			

			  //			   //
			 // SET FRAMERATE //
			//				 //
			if (snekLength < 11) {
				frameRate = 10;

			}

			else if (snekLength > 10 && snekLength < 30) {
				frameRate = 9;

			}

			else if (snekLength > 29 && snekLength < 50) {
				frameRate = 8;

			}

			else if (snekLength > 49 && snekLength < 72) {
				frameRate = 7;

			}

			else if (snekLength > 71 && snekLength < 100) {
				frameRate = 6;

			}

			else if (snekLength > 99 && snekLength < 123) {
				frameRate = 5;

			}

			else if (snekLength > 122 && snekLength < 150) {
				frameRate = 4;

			}

			else if (snekLength > 149) {
				frameRate = 3;

			}

			  //			//
			 // TICK CLOCK //
			//			  //
			for (int q = 0; q < frameRate; q++) {

				if (snekLength == 0) {
					this_thread::sleep_for(27ms);
				}

				else if (snekLength > 0 && snekLength < 7) {
					this_thread::sleep_for(17ms);
				}

				else if (snekLength > 6 && snekLength < 11) {
					this_thread::sleep_for(15ms);
				}

				else if (snekLength > 10 && snekLength < 20) {
					this_thread::sleep_for(14ms);
				}

				else if (snekLength > 19 && snekLength < 30) {
					this_thread::sleep_for(13ms);
				}

				else if (snekLength > 29 && snekLength < 40) {
					this_thread::sleep_for(12ms);
				}

				else if (snekLength > 39 && snekLength < 50) {
					this_thread::sleep_for(11ms);
				}

				else if (snekLength > 49 && snekLength < 65) {
					this_thread::sleep_for(10ms);
				}

				else if (snekLength > 64 && snekLength < 80) {
					this_thread::sleep_for(9ms);
				}

				else if (snekLength > 79 && snekLength < 100) {
					this_thread::sleep_for(8ms);
				}

				else if (snekLength > 99) {
					this_thread::sleep_for(7ms);
				}
				
				  //				   //
				 // READ PLAYER INPUT //
				//					 //
				for (int k = 0; k < 4; k++) {
					arrowKeys[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x25\x26\x27\x28"[k]))) != 0;

				}

				zKey = ((0x8000 & GetAsyncKeyState((unsigned char)("Z"[0]))) != 0) && snekLength > 10;
				

				  //					   //
				 // CHECK + SET DIRECTION //
				//						 //
				if (arrowKeys[0] && holdW == false && direction != 'e') {
					direction1 = 'w';
				}

				else if (arrowKeys[1] && holdN == false && direction != 's') {
					direction1 = 'n';
				}

				else if (arrowKeys[2] && holdE == false && direction != 'w') {
					direction1 = 'e';
				}

				else if (arrowKeys[3] && holdS == false && direction != 'n') {
					direction1 = 's';
				}

				currentTick++;
			}
			
			currentFrame++;		

			//REFRESH DISPLAY//

			for (int x = 0; x < 25; x++) {

				for (int y = 0; y < 25; y++) {

					display[x][y] = 'z';
				}
			}

			  //					//
			 // MOVE BODY SEGMENTS //
			//					  //
			for (int r = snekLength; r > 0; r--) {

				if (r > 1) {
					snekBody[r][0] = snekBody[r - 1][0];	//move all segments except the segment right before the head
					snekBody[r][1] = snekBody[r - 1][1];

				}

				else {
					snekBody[r][0] = snekHead[0];		//move the segment right before the head
					snekBody[r][1] = snekHead[1];

				}
			}

			  //					   //
			 // CHECK + SET DIRECTION //
			//						 //
			if (direction1 == 'w' && holdW == false && direction != 'e') {
				direction = 'w';

				holdW = true;
				holdE = false;
				holdS = false;
				holdN = false;
			}

			else if (direction1 == 'n' && holdN == false && direction != 's') {
				direction = 'n';

				holdN = true;
				holdE = false;
				holdS = false;
				holdW = false;
			}

			else if (direction1 == 'e' && holdE == false && direction != 'w') {
				direction = 'e';

				holdE = true;
				holdW = false;
				holdS = false;
				holdN = false;
			}

			else if (direction1 == 's' && holdS == false && direction != 'n') {
				direction = 's';

				holdS = true;
				holdE = false;
				holdW = false;
				holdN = false;
			}

			  //								    //
			 // PLACE SNEK BODY INTO DISPLAY ARRAY //
			//									  //
			for (int r = snekLength; r > 0; r--) {
				display[snekBody[r][0]][snekBody[r][1]] = '7';

			}

			  //				  //
			 // ADD STYLE POINTS //
			//					//
			
			//E
			if (direction == 'e' && ((zKey) && display[snekHead[0] + 1][snekHead[1]] == '8' || display[snekHead[0] + 1][snekHead[1]] == 'X' || display[snekHead[0] + 1][snekHead[1]] == '7' && display[snekHead[0] + 2][snekHead[1]] == 'z' || display[snekHead[0] + 2][snekHead[1]] == '+' && snekHead[0] + 2 < 25)) {
				styleCounter++;
				styleCounter++;
			}

			if (direction == 'e' && ((display[snekHead[0] + 1][snekHead[1] + 1] == '8' || display[snekHead[0] + 1][snekHead[1] + 1] == 'X' || display[snekHead[0] + 1][snekHead[1] + 1] == '7') && (display[snekHead[0] + 1][snekHead[1] - 1] == '8' || display[snekHead[0] + 1][snekHead[1] - 1] == 'X' || display[snekHead[0] + 1][snekHead[1] - 1] == '7')) && (display[snekHead[0] + 1][snekHead[1]] == 'z' || display[snekHead[0] + 1][snekHead[1]] == '+')) {
				styleCounter++;
			}

			//W
			if (direction == 'w' && ((zKey) && display[snekHead[0] - 1][snekHead[1]] == '8' || display[snekHead[0] - 1][snekHead[1]] == 'X' || display[snekHead[0] - 1][snekHead[1]] == '7' && display[snekHead[0] - 2][snekHead[1]] == 'z' || display[snekHead[0] - 2][snekHead[1]] == '+' && snekHead[0] - 2 >= 0)) {
				styleCounter++;
				styleCounter++;
			}

			if (direction == 'w' && ((display[snekHead[0] - 1][snekHead[1] + 1] == '8' || display[snekHead[0] - 1][snekHead[1] + 1] == 'X' || display[snekHead[0] - 1][snekHead[1] + 1] == '7') && (display[snekHead[0] - 1][snekHead[1] - 1] == '8' || display[snekHead[0] - 1][snekHead[1] - 1] == 'X' || display[snekHead[0] - 1][snekHead[1] - 1] == '7')) && (display[snekHead[0] - 1][snekHead[1]] == 'z' || display[snekHead[0] - 1][snekHead[1]] == '+')) {
				styleCounter++;
			}

			//S
			if (direction == 's' && ((zKey) && display[snekHead[0]][snekHead[1] + 1] == '8' || display[snekHead[0]][snekHead[1] + 1] == 'X' || display[snekHead[0]][snekHead[1] + 1] == '7' && display[snekHead[0]][snekHead[1] + 2] == 'z' || display[snekHead[0]][snekHead[1] + 2] == '+' && snekHead[1] + 2 < 25)) {
				styleCounter++;
				styleCounter++;
			}

			if (direction == 's' && ((display[snekHead[0] + 1][snekHead[1] + 1] == '8' || display[snekHead[0] + 1][snekHead[1] + 1] == 'X' || display[snekHead[0] + 1][snekHead[1] + 1] == '7') && (display[snekHead[0] - 1][snekHead[1] + 1] == '8' || display[snekHead[0] - 1][snekHead[1] + 1] == 'X' || display[snekHead[0] - 1][snekHead[1] + 1] == '7')) && (display[snekHead[0]][snekHead[1] + 1] == 'z' || display[snekHead[0]][snekHead[1] + 1] == '+')) {
				styleCounter++;
			}

			//N
			if (direction == 'n' && ((zKey) && display[snekHead[0]][snekHead[1] - 1] == '8' || display[snekHead[0]][snekHead[1] - 1] == 'X' || display[snekHead[0]][snekHead[1] - 1] == '7' && display[snekHead[0]][snekHead[1] - 2] == 'z' || display[snekHead[0]][snekHead[1] - 2] == '+' && snekHead[1] - 2 >= 0)) {
				styleCounter++;
				styleCounter++;
			}

			if (direction == 'n' && ((display[snekHead[0] + 1][snekHead[1] - 1] == '8' || display[snekHead[0] + 1][snekHead[1] - 1] == 'X' || display[snekHead[0] + 1][snekHead[1] - 1] == '7') && (display[snekHead[0] - 1][snekHead[1] - 1] == '8' || display[snekHead[0] - 1][snekHead[1] - 1] == 'X' || display[snekHead[0] - 1][snekHead[1] - 1] == '7')) && (display[snekHead[0]][snekHead[1] - 1] == 'z' || display[snekHead[0]][snekHead[1] - 1] == '+')) {
				styleCounter++;
			}
					   			
			  //			 //
			 // MOVE PLAYER //
			//			   //
			if (direction == 'e') {
				snekHead[0]++;
				
				if (zKey) {
					snekHead[0]++;
				}
			}

			else if (direction == 'w') {
				snekHead[0]--;

				if (zKey) {
					snekHead[0]--;
				}
			}

			else if (direction == 's') {
				snekHead[1]++;

				if (zKey) {
					snekHead[1]++;
				}
			}

			else if (direction == 'n') {
				snekHead[1]--;

				if (zKey) {
					snekHead[1]--;
				}
			}

			  //								   //
			 // DETECT IF PLAYER HAS HIT MAP EDGE //
			//									 //
			if (snekHead[0] < 0 || snekHead[0] > 24 || snekHead[1] < 0 || snekHead[1] > 24) {
				gameLose = true;

				deathInstance->start();
				system->update();

				break;
						
			}

			  //									 //
			 // DETECT IF PLAYER HAS HIT THEMSELVES //
			//									   //
			if (display[snekHead[0]][snekHead[1]] == '7' || display[snekHead[0]][snekHead[1]] == 'X') {
				gameLose = true;

				deathInstance->start();
				system->update();

				break;
			}

			  //						 //
			 // UPDATE STYLE HIGH SCORE //
			//						   //
			if (styleCounter > styleHighScore) {
				styleHighScore++;

			}

			  //								  //
			 // Calculate Proximity to the Fruit //
			//									//
			proximityToFruit = 1.0f - ((abs(snekHead[0] - currentFruit[0]) + abs(snekHead[1] - currentFruit[1])) / 48.0f);	//1/48 max distance

			  //								//
			 // DETECT IF PLAYER HAS HIT FRUIT //
			//								  //
			if (snekHead[0] == currentFruit[0] && snekHead[1] == currentFruit[1]) {
				
				snekLength++;

				if (snekLength == 11) {
					snakeFruitInstance11->start();	//FMOD
					snekMoveTimelinePositionMax += 200;
					snakeMoveReverbLevel = 0.125f;
					isScoreUnder11 = false;
				}
				else {
					snakeFruitInstance->start();	//FMOD	
				}
				
				switch (snekLength) {
					case 20:
						snekMoveTimelinePositionMax += 200;
						snakeMoveReverbLevel = 0.250f;
						break;

					case 30:
						snekMoveTimelinePositionMax += 200;
						snakeMoveReverbLevel = 0.375f;
						break;

					case 40:
						snekMoveTimelinePositionMax += 200;
						snakeMoveReverbLevel = 0.5f;
						break;

					case 50:
						snekMoveTimelinePositionMax += 200;
						snakeMoveReverbLevel = 0.625f;
						break;

					case 60:
						snekMoveTimelinePositionMax += 200;
						snakeMoveReverbLevel = 0.750f;
						break;

					case 70:
						snekMoveTimelinePositionMax += 200;
						snakeMoveReverbLevel = 0.875f;
						break;

					case 80:
						snekMoveTimelinePositionMax += 200;
						snakeMoveReverbLevel = 1.0f;
						break;
				}

				  //					//
				 // SET NEW HIGH SCORE //
				//					  //
				if (snekLength > highScore) {
					
					highScore++;
				}

				for (int e = 0; e == 0;) {
					currentFruit[0] = rand() % 25;
					currentFruit[1] = rand() % 25;

					if ((currentFruit[0] != snekHead[0] && currentFruit[1] != snekHead[1]) && display[currentFruit[0]][currentFruit[1]] != '7' && display[currentFruit[0]][currentFruit[1]] != 'X') {
						e = 1;
					}
				}
								
			}

			else if (zKey && snekLength > 10) {
				snakeLungeInstance->setPitch(proximityToFruit);
				snakeLungeInstance->start();	//FMOD
				snakeMoveInstance->setParameterByName("Reverb Wet", 1.0f);
				if (!wasZKeyHeld) {
					snekMoveTimelinePosition += 200;
					wasZKeyHeld = true;
				}
			}

			else {
						
				wasZKeyHeld = false;

				snakeMoveInstance->setPitch(proximityToFruit);
				snakeMoveInstance->setTimelinePosition(snekMoveTimelinePosition);
				snakeMoveInstance->setParameterByName("Reverb Wet", snakeMoveReverbLevel);
				
				if (isScoreUnder11 || snakeMoveInstance->getPlaybackState(NULL) == FMOD_STUDIO_PLAYBACK_SUSTAINING || snakeMoveInstance->getPlaybackState(NULL) == FMOD_STUDIO_PLAYBACK_STOPPED){
					
					snakeMoveInstance->start();	//FMOD

				}

				snekMoveTimelinePosition += 200;

				if (snekMoveTimelinePosition >= snekMoveTimelinePositionMax) {
					
					snekMoveTimelinePosition = 0;

				}
			}
						
			

			//PLACE PLAYER INTO DISPLAY ARRAY//

			display[snekHead[0]][snekHead[1]] = '8';

			//PLACE FRUIT INTO DISPLAY ARRAY//

			display[currentFruit[0]][currentFruit[1]] = '+';

			
/*			//DETERMINE TRAP LOCATIONS/////////////

			if (currentTrap < snekLength) {
				currentTrap++;

				if (currentTrap == 3) {

					for (int e = 0; e == 0;) {
						trapLocations[0][0] = rand() % 25;
						trapLocations[0][1] = rand() % 25;

						if ((trapLocations[0][0] != snekHead[0] && trapLocations[0][1] != snekHead[1]) && display[trapLocations[0][0]][trapLocations[0][1]] != '7' && display[trapLocations[0][0]][trapLocations[0][1]] != '+') {

							actualTrapCount++;

							e = 1;
						}
					}
				}

				if (currentTrap == 7) {

					for (int e = 0; e == 0;) {
						trapLocations[1][0] = rand() % 25;
						trapLocations[1][1] = rand() % 25;

						if ((trapLocations[1][0] != snekHead[0] && trapLocations[1][1] != snekHead[1]) && display[trapLocations[1][0]][trapLocations[1][1]] != '7' && display[trapLocations[1][0]][trapLocations[1][1]] != '+') {

							actualTrapCount++;

							e = 1;
						}
					}
				}

				if (currentTrap == 11) {

					for (int e = 0; e == 0;) {
						trapLocations[2][0] = rand() % 25;
						trapLocations[2][1] = rand() % 25;

						if ((trapLocations[2][0] != snekHead[0] && trapLocations[2][1] != snekHead[1]) && display[trapLocations[2][0]][trapLocations[2][1]] != '7' && display[trapLocations[2][0]][trapLocations[2][1]] != '+') {

							actualTrapCount++;

							e = 1;
						}
					}

					for (int e = 0; e == 0;) {
						trapLocations[3][0] = rand() % 25;
						trapLocations[3][1] = rand() % 25;

						if ((trapLocations[3][0] != snekHead[0] && trapLocations[3][1] != snekHead[1]) && display[trapLocations[3][0]][trapLocations[3][1]] != '7' && display[trapLocations[3][0]][trapLocations[3][1]] != '+') {

							actualTrapCount++;

							e = 1;
						}
					}
				}

				if (currentTrap > 11) {

					if (alternator1 = true) {
						alternator1 = false;

						for (int e = 0; e == 0;) {



							trapLocations[r][0] = rand() % 25;
							trapLocations[r][1] = rand() % 25;

							if ((trapLocations[r][0] != snekHead[0] && trapLocations[r][1] != snekHead[1]) && display[trapLocations[r][0]][trapLocations[r][1]] != '7' && display[trapLocations[r][0]][trapLocations[r][1]] != '+') {

								actualTrapCount++;
								r++;
								e = 1;




							}
						}
					}

					else {
						alternator1 = true;

					}
				}



			}
			*/
			//PLACE TRAPS INTO DISPLAY ARRAY/////////////
			/*
			for (int w = 0; w < actualTrapCount; w++) {
				display[trapLocations[w][0]][trapLocations[w][1]] = 'X';

			}
			*/
			//DETERMINE IF PLAYER HAS HIT A TRAP//////
/*
			if (actualTrapCount < 0 && display[snekHead[0]][snekHead[1]] == 'X') {
				gameLose = true;
				break;

			}
			*/
			
			  //			 //
			 // DRAW SCREEN //	
			//			   //
			for (int t = 0; t < 25; t++) {
				int q = 0;

				for (q; q < 25; q++) {

					if (display[q][t] == 'z') {
						screenString[q + (80 * t)] = ' ';

					}

					else if (display[q][t] == '7') {
						screenString[q + (80 * t)] = '8';
					}

					else if (display[q][t] == '8' && direction == 'n') {
						screenString[q + (80 * t)] = '^';
					}

					else if (display[q][t] == '8' && direction == 's') {
						screenString[q + (80 * t)] = 'v';
					}

					else if (display[q][t] == '8' && direction == 'w') {
						screenString[q + (80 * t)] = '<';
					}

					else if (display[q][t] == '8' && direction == 'e') {
						screenString[q + (80 * t)] = '>';
					}

					else {
						screenString[q + (80 * t)] = display[q][t];
					}
				}

				screenString[q + (80 * t)] = '|';

				if (t == 0 && q == 25) {
					screenString.replace(1 + q + (80 * t), 45, "       __    _    _              _  __   ____");
				}

				else if (t == 1 && q == 25) {
					screenString.replace(1 + q + (80 * t), 46, "      / /   | \\  | |     /\\     | |/ /  |  __|");
				}
				
				else if (t == 2 && q == 25) {
					screenString.replace(1 + q + (80 * t), 45, "      \\ \\   |  \\ | |    /  \\    | | /   | |__");
				}

				else if (t == 3 && q == 25) {
					screenString.replace(1 + q + (80 * t), 46, "       \\ \\  | | \\| |   / /\\ \\   |   \\   |  __|");
				}

				else if (t == 4 && q == 25) {
					screenString.replace(1 + q + (80 * t), 45, "       / /  | |\\ \\ |  /  __  \\  | |\\ \\  | |__");
				}

				else if (t == 5 && q == 25) {
					screenString.replace(1 + q + (80 * t), 46, "      /_/   |_| \\__| /__/  \\__\\ |_| \\_\\ |____|");
				}

				else if (t == 7 && q == 25) {
					
					screenString.replace(1 + q + (80 * t), 14, "       SCORE: ");
					
					string snekLengthString = to_string(snekLength);
										
					screenString.replace(15 + q + (80 * t), snekLengthString.length(), snekLengthString);

					screenString.replace(15 + snekLengthString.length() + q + (80 * t), 17, "     HIGH SCORE: ");

					
					
					string highScoreString = to_string(highScore);

					screenString.replace(32 + snekLengthString.length() + q + (80 * t), highScoreString.length(), highScoreString);

				}

				else if (t == 9 && q == 25 && snekLength > 10) {

					screenString.replace(1 + q + (80 * t), 14, "       STYLE: ");

					string styleCounterString = to_string(styleCounter);

					screenString.replace(15 + q + (80 * t), styleCounterString.length(), styleCounterString);

					screenString.replace(16 + styleCounterString.length() + q + (80 * t), 17, "     HIGH STYLE: ");

					string highStyleString = to_string(styleHighScore);

					screenString.replace(33 + styleCounterString.length() + q + (80 * t), highStyleString.length(), highStyleString);

				}

				else if (t == 17 && q == 25) {
					screenString.replace(8 + q + (80 * t), 33, "use arrow keys ^ v < > to control");
				}

				/*else if (t == 19 && q == 25) {
					//cout << "	use arrow keys ^ v < > to control";
					screenString.replace(8 + q + (80 * t), 18, "Press [S] to pause");
				}
				*/

				else if (t == 21 && q == 25 && snekLength > 10) {
					screenString.replace(14 + q + (80 * t), 18, "use Z key to lunge");
				}

			}

			for (int u = 0; u < (nScreenHeight * nScreenWidth); u++) {
				screen[u] = screenString[u];
			}

			WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
			
			system->update(); //update FMOD system
		}

		this_thread::sleep_for(1347ms);
		
		screenString.replace(14 + 25 + (80 * 21), 18, "                  ");
		screenString.replace(8 + 25 + (80 * 17), 33, "                                 ");

		fancyBossInstance->start();

		system->update(); //begin FMOD sound generation/song playback

			screenString.replace((nScreenHeight * nScreenWidth) - 753, 9, "GAME OVER");
			screenString.replace((nScreenHeight * nScreenWidth) - 360, 24, ">Press [Z] to play again");
			screenString.replace((nScreenHeight * nScreenWidth) - 280, 18, ">Press [X] to quit");

			for (int u = 0; u < (nScreenHeight * nScreenWidth); u++) {
				screen[u] = screenString[u];

			}

			WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);

			bool gameOverMessage = true;

			bool xKey;

			this_thread::sleep_for(527ms);

			while (gameOverMessage) {
								
				if (zKey = (0x8000 & GetAsyncKeyState((unsigned char)("Z"[0]))) != 0) {
					gameOverMessage = false;
					playAgain = true;
					
					fancyBossInstance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);
					system->update();
				}



				if (xKey = (0x8000 & GetAsyncKeyState((unsigned char)("X"[0]))) != 0) {
					gameOverMessage = false;
					playAgain = false;

					fancyBossInstance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);
					system->update();
				}

				this_thread::sleep_for(10ms);
			}
	}

	while (playAgain);

	return 0;

}


  //		  //
 // JUNKYARD //
//			//
/*for (int q = 0; q < 25; q++) {

	cout << q << " = " << display[q] << endl;

}*/

/*for (int q = 0; q < 25; q++) {

	display[q] = 0;

}*/

/*
cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snekHead[0] == z) { cout << "="; } z++; //1 
cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snekHead[0] == z) { cout << "="; } z++; //2 
cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snekHead[0] == z) { cout << "="; } z++; //3 
cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snekHead[0] == z) { cout << "="; } z++; //4 
cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snekHead[0] == z) { cout << "="; } z++; //5 
cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snekHead[0] == z) { cout << "="; } z++; //6 
cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snekHead[0] == z) { cout << "="; } z++; //7 
cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snekHead[0] == z) { cout << "="; } z++; //8 
cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snekHead[0] == z) { cout << "="; } z++; //9 
cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snekHead[0] == z) { cout << "="; } z++; //10
cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snekHead[0] == z) { cout << "="; } z++; //11
cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snekHead[0] == z) { cout << "="; } z++; //12
cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snekHead[0] == z) { cout << "="; } z++; //13
cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snekHead[0] == z) { cout << "="; } z++; //14
cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snekHead[0] == z) { cout << "="; } z++; //15
cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snekHead[0] == z) { cout << "="; } z++; //16
cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snekHead[0] == z) { cout << "="; } z++; //17
cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snekHead[0] == z) { cout << "="; } z++; //18
cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snekHead[0] == z) { cout << "="; } z++; //19
cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snekHead[0] == z) { cout << "="; } z++; //20
cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snekHead[0] == z) { cout << "="; } z++; //21
cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snekHead[0] == z) { cout << "="; } z++; //22
cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snekHead[0] == z) { cout << "="; } z++; //23
cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snekHead[0] == z) { cout << "="; } z++; //24
cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snekHead[0] == z) { cout << "="; } z++; //25

}
*/

/*
//PAUSE/////

startKey = (0x8000 & GetAsyncKeyState((unsigned char)("S"[0]))) != 0;
if (startKey) {
	bool paused = true;
	this_thread::sleep_for(777ms);
	while (paused) {
		startKey = (0x8000 & GetAsyncKeyState((unsigned char)("S"[0]))) != 0;
		if (startKey) {
			paused = false;
		}
	}
}

*/

/*
if (snekHead[0] >= 25) {
	direction = 'w';

}

else if (snekHead[0] <= 1) {
	direction = 'e';

}
*/

/*if (snekHead[0] - 1 < 0 && direction == 'w' || snekHead[0] + 1 > 25 && direction == 'e' || snekHead[1] - 1 < 0 && direction == 'n' || snekHead[1] + 1 > 25 && direction == 's') {
				gameLose = true;

			}*/