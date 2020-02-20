//	 __    _    _              _  __   ____
//	/ /   | \  | |     /\     | |/ /  |  __|
//	\ \   |  \ | |    /  \    | | /   | |__
//	 \ \  | | \| |   / /\ \   |   \   |  __|
//	 / /  | |\ \ |  /  __  \  | |\ \  | |__
//	/_/   |_| \__| /__/  \__\ |_| \_\ |____|
// / / BY M. O. MARMALADE / / / / / / / / / / / / /

  /////////////////////
 // PROJECT OUTLINE //
/////////////////////
// Includes/Namespaces
// Variable Declarations
// Audio System Setup (FMOD Studio)
// Loading/Preparing Audio Events
// Display Setup
// Splash Screen Animation
// Start Screen
// Read High Score From File
// Pre-New Game Perparation
// [Game Loop Start]
// Set Framerate
// Tick Clock
// Read Player Input
// Check + Set Direction [Tick Resolution]
// Refresh Display
// Move Body Segments
// Check + Set Direction [Frame Resolution]
// Place Snek Body Into Display Array
// Add Style Points
// Move Player
// Detect if Player Has Hit Map Edge
// Detect if Player Has Hit Themselves
// Update Style High Score
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
// Write High Score To File

  //					 //
 // INCLUDES/NAMESPACES //
//					   //
#include <Windows.h>
#include <Wincon.h>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <fstream>
#include <codecvt>
#include <fmod.hpp>
#include <fmod_studio.hpp>

using namespace std;

  //					   //
 // VARIABLE DECLARATIONS //
//						 //

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
wstring keyboard = L"ABCDEFGHIJKLMNOPQRSTUVWXYZ.-_ ←↓";
wstring highScoreName;
wstring highScoreNameFromFileWide;
string highScoreNameFromFileNarrow;
wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;
bool wasPreviousHighScoreFound;

//INPUT VARIABLES
bool arrowKeys[4];				//stores input from arrow keys
bool zKey;						//stores input from Z key
bool actionKeyHeld = false;		//indicates whether any one player is holding an action key

//DISPLAY VARIABLES
int frameRate = 10;				//frame rate setting
int currentFrame = 0;			//keeps track of how many frames have passed
int currentTick = 0;			//keeps track of how many ticks have passed
int nScreenWidth = 80;			//width of the console window (measured in characters, not pixels)
int nScreenHeight = 25;			//height of the console window (measured in characters, not pixels)

//SOUND VARIABLES (FMOD)
int snekMoveTimelinePosition = 0;		//snakeMoveInstance timeline position
int snekMoveTimelinePositionMax = 200;	//snakeMoveInstance timeline position max
bool isScoreUnder11 = true;				//changes to false when highestCurrentLength passes 11
float snakeMoveReverbLevel = 0.0f;		//reverb level for the snakeMoveInstance sound
float proximityToFruit;					//stores the closest player's proximity to the current fruit				
int i16thNote = 1;						//counts each frame and resets back to 1 after reaching 16 (it skips 17 and goes back to 1)
bool chordsStartToggle = false;
int currentChord = 0;
bool hiHatToggle = false;
bool gotNewHighScoreSoundPlay;

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

void SleepinnnThang() {					//framerate for animation that plays after pressing Z to start game
	this_thread::sleep_for(33ms);
}

int main() {	

	 //									 //
	// AUDIO SYSTEM SETUP (FMOD Studio) //
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

	  //							    //
	 // LOADING/PREPARING AUDIO EVENTS //
	//							      //
	FMOD::Studio::EventDescription* splashJingleDescription = NULL;			//Splash Jingle (Citrus Studios splash screen) (FMOD)
	system->getEvent("event:/Menu+Songs/SplashJingle", &splashJingleDescription);

	FMOD::Studio::EventInstance* splashJingleInstance = NULL;
	splashJingleDescription->createInstance(&splashJingleInstance);

	FMOD::Studio::EventDescription* aNewChipDescription = NULL;				//ANewChip (start screen song)
	system->getEvent("event:/Menu+Songs/ANewChip", &aNewChipDescription);

	FMOD::Studio::EventInstance* aNewChipInstance = NULL;
	aNewChipDescription->createInstance(&aNewChipInstance);

	FMOD::Studio::EventDescription* startButtonDescription = NULL;			//StartButton (game start sound effect)
	system->getEvent("event:/Menu+Songs/StartButton", &startButtonDescription);

	FMOD::Studio::EventInstance* startButtonInstance = NULL;
	startButtonDescription->createInstance(&startButtonInstance);

	FMOD::Studio::EventDescription* fancyBossDescription = NULL;			//FancyBoss (game over song)
	system->getEvent("event:/Menu+Songs/FancyBoss", &fancyBossDescription);

	FMOD::Studio::EventInstance* fancyBossInstance = NULL;
	fancyBossDescription->createInstance(&fancyBossInstance);

	FMOD::Studio::EventDescription* exitGameDescription = NULL;				//Exit Game (Citrus Studios splash screen) (FMOD)
	system->getEvent("event:/Menu+Songs/ExitGame", &exitGameDescription);

	FMOD::Studio::EventInstance* exitGameInstance = NULL;
	exitGameDescription->createInstance(&exitGameInstance);

	FMOD::Studio::EventDescription* snakeFruitDescription = NULL;			//SnakeFruit (pickup fruit sound effect)
	system->getEvent("event:/Instruments+FX/SnakeFruit", &snakeFruitDescription);

	FMOD::Studio::EventInstance* snakeFruitInstance = NULL;
	snakeFruitDescription->createInstance(&snakeFruitInstance);

	FMOD::Studio::EventDescription* snakeFruitDescription11 = NULL;			//SnakeFruit11 (snake eyes score sound)
	system->getEvent("event:/Instruments+FX/SnakeFruit11", &snakeFruitDescription11);

	FMOD::Studio::EventInstance* snakeFruitInstance11 = NULL;
	snakeFruitDescription11->createInstance(&snakeFruitInstance11);

	FMOD::Studio::EventDescription* snakeMoveDescription = NULL;			//SnakeMove (movement sound effect)
	system->getEvent("event:/Instruments+FX/SnakeMove", &snakeMoveDescription);

	FMOD::Studio::EventInstance* snakeMoveInstance = NULL;
	snakeMoveDescription->createInstance(&snakeMoveInstance);	

	FMOD::Studio::EventDescription* snakeLungeDescription = NULL;			//SnakeLunge (lunge sound effect)
	system->getEvent("event:/Instruments+FX/SnakeLunge", &snakeLungeDescription);

	FMOD::Studio::EventInstance* snakeLungeInstance = NULL;
	snakeLungeDescription->createInstance(&snakeLungeInstance);
	
	FMOD::Studio::EventDescription* deathDescription = NULL;				//Death (death collision sound)
	system->getEvent("event:/Instruments+FX/Death", &deathDescription);

	FMOD::Studio::EventInstance* deathInstance = NULL;
	deathDescription->createInstance(&deathInstance);

	FMOD::Studio::EventDescription* criticalDescription = NULL;				//Critical (snake only has a head sound)
	system->getEvent("event:/Instruments+FX/Critical", &criticalDescription);

	FMOD::Studio::EventInstance* criticalInstance = NULL;
	criticalDescription->createInstance(&criticalInstance);

	FMOD::Studio::EventDescription* kickDescription = NULL;					//Kick Drum
	system->getEvent("event:/Instruments+FX/Kick", &kickDescription);

	FMOD::Studio::EventInstance* kickInstance = NULL;
	kickDescription->createInstance(&kickInstance);

	FMOD::Studio::EventDescription* kick2Description = NULL;				//Kick Drum 2
	system->getEvent("event:/Instruments+FX/Kick2", &kick2Description);

	FMOD::Studio::EventInstance* kick2Instance = NULL;
	kick2Description->createInstance(&kick2Instance);

	FMOD::Studio::EventDescription* snare1Description = NULL;				//Snare Drum 1
	system->getEvent("event:/Instruments+FX/Snare1", &snare1Description);

	FMOD::Studio::EventInstance* snare1Instance = NULL;
	snare1Description->createInstance(&snare1Instance);

	FMOD::Studio::EventDescription* snare2Description = NULL;				//Snare Drum 2
	system->getEvent("event:/Instruments+FX/Snare2", &snare2Description);

	FMOD::Studio::EventInstance* snare2Instance = NULL;
	snare2Description->createInstance(&snare2Instance);

	FMOD::Studio::EventDescription* a808DrumDescription = NULL;				//808 Drum
	system->getEvent("event:/Instruments+FX/808Drum", &a808DrumDescription);

	FMOD::Studio::EventInstance* a808DrumInstance = NULL;
	a808DrumDescription->createInstance(&a808DrumInstance);

	FMOD::Studio::EventDescription* cymbalDescription = NULL;				//Cymbal
	system->getEvent("event:/Instruments+FX/Cymbal", &cymbalDescription);

	FMOD::Studio::EventInstance* cymbalInstance = NULL;
	cymbalDescription->createInstance(&cymbalInstance);

	FMOD::Studio::EventDescription* badBossAngelDescription = NULL;				//badBossAngel
	system->getEvent("event:/Instruments+FX/badbossangel", &badBossAngelDescription);

	FMOD::Studio::EventInstance* badBossAngelInstance = NULL;
	badBossAngelDescription->createInstance(&badBossAngelInstance);

	FMOD::Studio::EventDescription* triangleDescription = NULL;				//triangle
	system->getEvent("event:/Instruments+FX/Triangle", &triangleDescription);

	FMOD::Studio::EventInstance* triangleInstance = NULL;
	triangleDescription->createInstance(&triangleInstance);

	FMOD::Studio::EventDescription* chordsDescription = NULL;				//chords
	system->getEvent("event:/Instruments+FX/chords", &chordsDescription);

	FMOD::Studio::EventInstance* chordsInstance = NULL;
	chordsDescription->createInstance(&chordsInstance);

	FMOD::Studio::EventDescription* arpDescription = NULL;				//arp
	system->getEvent("event:/Instruments+FX/arp", &arpDescription);

	FMOD::Studio::EventInstance* arpInstance = NULL;
	arpDescription->createInstance(&arpInstance);

	FMOD::Studio::EventDescription* closedHiHatDescription = NULL;				//closedHiHat
	system->getEvent("event:/Instruments+FX/ClosedHiHat", &closedHiHatDescription);

	FMOD::Studio::EventInstance* closedHiHatInstance = NULL;
	closedHiHatDescription->createInstance(&closedHiHatInstance);

	FMOD::Studio::EventDescription* newHighScoreDescription = NULL;				//newHighScore
	system->getEvent("event:/Instruments+FX/newHighScore", &newHighScoreDescription);

	FMOD::Studio::EventInstance* newHighScoreInstance = NULL;
	newHighScoreDescription->createInstance(&newHighScoreInstance);
	
	/*FMOD::Studio::EventDescription* proximitySoundDescription = NULL;
	system->getEvent("event:/ProximitySound", &proximitySoundDescription);

	FMOD::Studio::EventInstance* proximitySoundInstance = NULL;
	proximitySoundDescription->createInstance(&proximitySoundInstance);*/

	  //			   //
	 // DISPLAY SETUP //
	//			  	 //	

	//create screen buffer//
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;

	wstring screenString;								 //character array to be displayed to the screen	
	screenString.resize(nScreenWidth * nScreenHeight);	//set size of screen char array/string//

	vector<WORD> attributes(nScreenWidth * nScreenHeight, FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);	//stores colors for display
		   	
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
	*/
							
	splashJingleInstance->start();	//Begin Splash Screen (FMOD)
	system->update();

	  //						 //
	 // SPLASH SCREEN ANIMATION //
	//						   //
	bool animation = true;
	int u = 0;
	int charToOverwrite = 992;	

	while (animation) {			//Draw Splash Screen

		screenString[charToOverwrite] = L"Citrus Studios"[u];
		charToOverwrite++;
		u++;
		WriteConsoleOutputCharacter(hConsole, screenString.c_str(), nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
		WriteConsoleOutputAttribute(hConsole, &attributes[0], nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);

		this_thread::sleep_for(77ms);

		if (charToOverwrite == 1006) {
			animation = false;
			this_thread::sleep_for(222ms);
		}
	}	
	
	animation = true;
	charToOverwrite = 992;	
	while (animation) {			//Erase Splash Screen
		
		screenString[charToOverwrite] = char(32);
		charToOverwrite++;
		
		WriteConsoleOutputCharacter(hConsole, screenString.c_str(), nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
		
		this_thread::sleep_for(77ms);
		
		if (charToOverwrite == 1006) {
			animation = false;
			this_thread::sleep_for(222ms);
		}
		
	}

	  //			  //
	 // START SCREEN //
	//				//
	bool startScreen = true;
	bool startScreenToggle = true;
	int startScreenFrameCount = 111;

	this_thread::sleep_for(777ms);

	aNewChipInstance->start();	//begin start screen playback	(FMOD)
	system->update();


	
	for (int yt = 0; yt < 1040; yt++) {
		attributes[yt] = FOREGROUND_GREEN;
	}
	for (int yt = 0; yt < 960; yt++) {
		attributes[yt + 1040] = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
	}
	for (int yt = 1440; yt < 1520; yt++) {
		attributes[yt] = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
	}
	for (int yt = 1600; yt < 1638; yt++) {
		attributes[yt] = FOREGROUND_RED | FOREGROUND_INTENSITY;
	}
	for (int yt = 1625; yt < 1651; yt++) {
		attributes[yt] = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN;
	}
	attributes[1643] = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
	attributes[1644] = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
	attributes[1645] = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
	for (int yt = 1651; yt < 1680; yt++) {
		attributes[yt] = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
	}
	for (int yt = 1680; yt < 2000; yt++) {
		attributes[yt] = FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
	}

	WriteConsoleOutputAttribute(hConsole, &attributes[0], nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);

	bool holdKey = false;
	while (startScreen) {

		/*for (int u = 0; u < nScreenHeight * nScreenWidth; u++) {	//clear display each frame

			screenString[u] = char(32);

		}*/

		screenString.replace((7 * 80) + 20, 38,   L"__    _    _              _  __   ____");		//draw logo each frame
		screenString.replace((8 * 80) + 19, 40,  L"/ /   | \\  | |     /\\     | |/ /  |  __|");
		screenString.replace((9 * 80) + 19, 39,  L"\\ \\   |  \\ | |    /  \\    | | /   | |__");
		screenString.replace((10 * 80) + 20, 39,  L"\\ \\  | | \\| |   / /\\ \\   |   \\   |  __|");
		screenString.replace((11 * 80) + 20, 38,  L"/ /  | |\\ \\ |  /  __  \\  | |\\ \\  | |__");
		screenString.replace((12 * 80) + 19, 40, L"/_/   |_| \\__| /__/  \\__\\ |_| \\_\\ |____|");

		screenString.replace((20 * 80) + 34, 12, L"Players: <" + to_wstring(playerCount) + L">");

		if (playerCount == 2) {
			screenString.replace((14 * 80) + 24, 32, L"                                ");
			screenString.replace((15 * 80) + 24, 32, L"                                ");
			screenString.replace((16 * 80) + 24, 32, L"                                ");
			screenString.replace((14 * 80) + 41, 26, L"--------------------------");
			screenString.replace((15 * 80) + 41, 26, L"| P1: Arrow Keys + P-Key |");
			screenString.replace((16 * 80) + 41, 26, L"--------------------------");
			screenString.replace((14 * 80) + 15, 20, L"--------------------");
			screenString.replace((15 * 80) + 15, 20, L"| P2: WASD + B-Key |");
			screenString.replace((16 * 80) + 15, 20, L"--------------------");
			
			for (int yt = 14 * 80; yt < (14 * 80) + 35; yt++) {
				attributes[yt] = FOREGROUND_RED | FOREGROUND_INTENSITY;
			}
			for (int yt = 15 * 80; yt < (15 * 80) + 35; yt++) {
				attributes[yt] = FOREGROUND_RED | FOREGROUND_INTENSITY;
			}
			for (int yt = 16 * 80; yt < (16 * 80) + 35; yt++) {
				attributes[yt] = FOREGROUND_RED | FOREGROUND_INTENSITY;
			}
		}
		else {			
			screenString.replace((14 * 80) + 41, 26, L"                          ");
			screenString.replace((15 * 80) + 41, 26, L"                          ");
			screenString.replace((16 * 80) + 41, 26, L"                          ");
			screenString.replace((14 * 80) + 15, 20, L"                    ");
			screenString.replace((15 * 80) + 15, 20, L"                    ");
			screenString.replace((16 * 80) + 15, 20, L"                    ");
			screenString.replace((14 * 80) + 24, 32, L"--------------------------------");
			screenString.replace((15 * 80) + 24, 32, L"| Controls: Arrow Keys + Z-Key |");
			screenString.replace((16 * 80) + 24, 32, L"--------------------------------");

			for (int yt = 14 * 80; yt < (14 * 80) + 35; yt++) {
				attributes[yt] = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
			}
			for (int yt = 15 * 80; yt < (15 * 80) + 35; yt++) {
				attributes[yt] = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
			}
			for (int yt = 16 * 80; yt < (16 * 80) + 35; yt++) {
				attributes[yt] = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
			}

		}

		screenString.replace((22 * 80) + 33, 14, L"Citrus Studios");		//draw studio name each frame

		if (startScreenFrameCount == 111) {
			switch (startScreenToggle) {
			case true:
				startScreenToggle = false;
				screenString.replace((18 * 80) + 31, 18, L"Press [Z] to start");	//draw "Press Z to start" every 111th frame

				snakeFruitInstance->setPitch(1.0f);
				snakeFruitInstance->setVolume(0.7f);
				snakeFruitInstance->start();	//play snakefruitinstance sound for flashing "press start" button (FMOD)
				system->update();
				break;
			case false:
				startScreenToggle = true;
				screenString.replace((18 * 80) + 31, 18, L"                  ");

				break;
			}

			startScreenFrameCount = 0;
		}				

		WriteConsoleOutputCharacter(hConsole, screenString.c_str(), nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);	//display the char[] buffer
		WriteConsoleOutputAttribute(hConsole, &attributes[0], nScreenWidth* nScreenHeight, { 0,0 }, & dwBytesWritten);

		startScreenFrameCount++;

		this_thread::sleep_for(7ms);

		for (int o = 0; o < 4; o++) {
			arrowKeys[o] = (0x8000 & GetAsyncKeyState((unsigned char)("\x25\x26\x27\x28"[o]))) != 0;						
		}

		if (arrowKeys[2] && playerCount < 2 && !holdKey) {
			playerCount++;
			snakeFruitInstance->setPitch(1.77f);
			snakeFruitInstance->setVolume(1.0f);
			snakeFruitInstance->start();
			system->update();
			holdKey = true;
		}
		else if (arrowKeys[0] && playerCount > 1 && !holdKey) {
			playerCount--;
			snakeFruitInstance->setPitch(0.64f);
			snakeFruitInstance->setVolume(1.0f);
			snakeFruitInstance->start();
			system->update();
			holdKey = true;
		}
		else if (holdKey && !arrowKeys[0] && !arrowKeys[2]) {
			holdKey = false;
		}
		

		if (zKey = (0x8000 & GetAsyncKeyState((unsigned char)("Z"[0]))) != 0) {

			startScreen = false;	//begin to exit start screen when Z key is pressed

			aNewChipInstance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);	//FMOD)
			startButtonInstance->start();

			system->update();	//play startbutton sound

			

			//Game Start animation//

			screenString.replace((18 * 80) + 31, 18, L"Press [Z] to start");

			WriteConsoleOutputCharacter(hConsole, screenString.c_str(), nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);

			SleepinnnThang();
					   
			screenString.replace((18 * 80) + 31, 18, L"Press [Z++to start");

			WriteConsoleOutputCharacter(hConsole, screenString.c_str(), nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);

			SleepinnnThang();

			screenString.replace((18 * 80) + 31, 18, L"Press [+AR+o start");
			
			WriteConsoleOutputCharacter(hConsole, screenString.c_str(), nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);

			SleepinnnThang();

			screenString.replace((18 * 80) + 31, 18, L"Press +TART+ start");

			WriteConsoleOutputCharacter(hConsole, screenString.c_str(), nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);

			SleepinnnThang();

			screenString.replace((18 * 80) + 31, 18, L"Press+START!+start");

			WriteConsoleOutputCharacter(hConsole, screenString.c_str(), nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);

			SleepinnnThang();

			screenString.replace((18 * 80) + 31, 18, L"Pres+ START! +tart");

			WriteConsoleOutputCharacter(hConsole, screenString.c_str(), nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);

			SleepinnnThang();

			screenString.replace((18 * 80) + 31, 18, L"Pre+  START!  +art");

			WriteConsoleOutputCharacter(hConsole, screenString.c_str(), nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);

			SleepinnnThang();

			screenString.replace((18 * 80) + 31, 18, L"Pr+   START!   +rt");

			WriteConsoleOutputCharacter(hConsole, screenString.c_str(), nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);

			SleepinnnThang();

			screenString.replace((18 * 80) + 31, 18, L"P+    START!    +t");

			WriteConsoleOutputCharacter(hConsole, screenString.c_str(), nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);

			SleepinnnThang();

			screenString.replace((18 * 80) + 31, 18, L"+     START!     +");

			WriteConsoleOutputCharacter(hConsole, screenString.c_str(), nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);

			SleepinnnThang();

			screenString.replace((18 * 80) + 31, 18, L"-                -");

			WriteConsoleOutputCharacter(hConsole, screenString.c_str(), nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);

			this_thread::sleep_for(177ms);

			screenString.replace((18 * 80) + 31, 18, L"-     START!     -");

			WriteConsoleOutputCharacter(hConsole, screenString.c_str(), nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);

			this_thread::sleep_for(177ms);

			screenString.replace((18 * 80) + 31, 18, L"-                -");

			WriteConsoleOutputCharacter(hConsole, screenString.c_str(), nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);

			this_thread::sleep_for(77ms);

			screenString.replace((18 * 80) + 31, 18, L"-     START!     -");

			WriteConsoleOutputCharacter(hConsole, screenString.c_str(), nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
		}
	}	

	this_thread::sleep_for(1389ms);
	   
	  //					  //
	 // READ HIGH SCORE FILE //
	//						//
	ifstream scoreFileRead;
	scoreFileRead.open("ScoreFile");
	if (scoreFileRead.is_open()) {
		
		string highScoreFromFile;
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
		  //						  //
		 // PRE-NEW GAME PREPARATION //
		//						    //	
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
		frameRate = 100;			//reset the framerate
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
		for (int n = 0; n < nScreenHeight * nScreenWidth; n++) {	
			screenString.replace(n, 1, L" ");
		}

		//Reset FMOD-Related Audio/Sound Variables//
		snekMoveTimelinePosition = 0;
		snekMoveTimelinePositionMax = 200;
		isScoreUnder11 = true;
		snakeMoveReverbLevel = 0.0f;
		i16thNote = 1;
		badBossAngelInstance->start();
		snakeMoveInstance->setParameterByName("SnakeMoveVolume", 1.0f);
		snare2Instance->setParameterByName("SnareReverb", 0.0f);
		arpInstance->setParameterByName("ArpVolume", 0.0f);
		snakeFruitInstance->setPitch(1.0f);
		snakeFruitInstance->setVolume(1.0f);
		chordsStartToggle = false;
		hiHatToggle = false;
		gotNewHighScore = false;
		gotNewHighScoreSoundPlay = false;
			   
		  //				   //
		 // [GAME LOOP START] //
		//				     //
		while (gameLose == false) {			

			  //			   //
			 // SET FRAMERATE //
			//				 //
			if (gotNewFruit = true) {
				switch (highestCurrentLength) {
				case 1:
					frameRate = 32;
					break;
				case 7:
					frameRate = 27;
					break;
				case 11:
					frameRate = 22;
					break;
				case 20:
					frameRate = 18;
					break;
				case 30:
					frameRate = 16;
					break;
				case 40:
					frameRate = 14;
					break;
				case 50:
					frameRate = 13;
					break;
				case 60:
					frameRate = 12;
					break;
				case 70:
					frameRate = 11;
					break;
				case 80:
					frameRate = 10;
					break;
				case 90:
					frameRate = 9;
					break;
				}
			}
			
			  //			//
			 // TICK CLOCK //
			//			  //
			for (int q = 0; q < frameRate; q++) {
				
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
				
				this_thread::sleep_for(7ms);

				currentTick++;	
				
			}
			
			currentFrame++;	

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

			  //						 //
			 // UPDATE STYLE HIGH SCORE //
			//						   //
			if (styleCounter > styleHighScore && !gameLose) {
				styleHighScore++;
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
			/*
			enter 0		//some random maths for portal traversal
			exit 1

			(0 + 1) % 2


			enter 1
			exit 0

			(1 + 1) % 2
			
			*/

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
			if (gameLose) {					//play the death sound if the game is lost
				badBossAngelInstance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);
				a808DrumInstance->stop(FMOD_STUDIO_STOP_IMMEDIATE);
				deathInstance->start();
			}

			if (gotNewFruit) {				//if someone gets a new fruit..
				for (int pt = 0; pt < playerCount; pt++) {		//..check each player..
					if (snek1[pt].justGotNewFruit) {			//..to see if they were the one who got the new fruit..					
						if (gotNewHighScoreSoundPlay) {
							newHighScoreInstance->start();
							gotNewHighScoreSoundPlay = false;
						}						
						else if (snek1[pt].snek_length == 11) {		//..if they did get a fruit, see if they just got their 11th fruit..
							snakeFruitInstance11->start();		//..if they did, then play the 11th fruit sound..
						}						
						else if (gotNewFruit && (i16thNote == 3 || i16thNote == 7 || i16thNote == 11 || i16thNote == 15)) {
							triangleInstance->start();			//if they got a fruit on an offbeat, play triangle sound
						}
						else {									//..otherwise, play the default fruit eating sound
							snakeFruitInstance->start();
						}						
					}
				}

				switch (highestCurrentLength) {					//update reverb level and max timeline position from the current highest length
				case 1:
					badBossAngelInstance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);
					chordsStartToggle = true;
					break;
				case 7:
					arpInstance->setParameterByName("ArpVolume", 0.09f);
					break;
				case 11:
					snekMoveTimelinePositionMax += 200;
					snakeMoveReverbLevel = 0.125f;
					snare2Instance->setParameterByName("SnareReverb", 0.2f);
					arpInstance->setParameterByName("ArpVolume", 0.14f);
					snakeMoveInstance->setParameterByName("SnakeMoveVolume", 1.0f);
					hiHatToggle = true;
					break;

				case 20:
					snekMoveTimelinePositionMax += 200;
					snakeMoveReverbLevel = 0.250f;
					snare2Instance->setParameterByName("SnareReverb", 0.4f);
					arpInstance->setParameterByName("ArpVolume", 0.17f);
					snakeMoveInstance->setParameterByName("SnakeMoveVolume", 0.9f);
					break;

				case 30:
					snekMoveTimelinePositionMax += 200;
					snakeMoveReverbLevel = 0.375f;
					snare2Instance->setParameterByName("SnareReverb", 0.5f);
					arpInstance->setParameterByName("ArpVolume", 0.2f);
					snakeMoveInstance->setParameterByName("SnakeMoveVolume", 0.88f);
					break;

				case 40:
					snekMoveTimelinePositionMax += 200;
					snakeMoveReverbLevel = 0.5f;
					snare2Instance->setParameterByName("SnareReverb", 0.64f);
					arpInstance->setParameterByName("ArpVolume", 0.3f);
					snakeMoveInstance->setParameterByName("SnakeMoveVolume", 0.7f);
					break;

				case 50:
					snekMoveTimelinePositionMax += 200;
					snakeMoveReverbLevel = 0.625f;
					snare2Instance->setParameterByName("SnareReverb", 0.72f);
					arpInstance->setParameterByName("ArpVolume", 0.45f);
					snakeMoveInstance->setParameterByName("SnakeMoveVolume", 0.4f);
					break;

				case 60:
					snekMoveTimelinePositionMax += 200;
					snakeMoveReverbLevel = 0.750f;
					snare2Instance->setParameterByName("SnareReverb", 0.8f);
					arpInstance->setParameterByName("ArpVolume", 0.6f);
					snakeMoveInstance->setParameterByName("SnakeMoveVolume", 0.2f);
					break;

				case 70:
					snekMoveTimelinePositionMax += 200;
					snakeMoveReverbLevel = 0.875f;
					snare2Instance->setParameterByName("SnareReverb", 0.9f);
					arpInstance->setParameterByName("ArpVolume", 0.8f);
					snakeMoveInstance->setParameterByName("SnakeMoveVolume", 0.0f);
					break;

				case 80:
					snekMoveTimelinePositionMax += 200;
					snakeMoveReverbLevel = 1.0f;
					snare2Instance->setParameterByName("SnareReverb", 1.0f);
					arpInstance->setParameterByName("ArpVolume", 0.9f);
					break;
				}
			}	
			//if nobody got any fruits, then check if either snake is using the action keys..
			else if (snek1[0].action_keys || snek1[1].action_keys) {
				snakeLungeInstance->setPitch(proximityToFruit);
				snakeLungeInstance->start();
				snakeMoveInstance->setParameterByName("Reverb Wet", 1.0f);		//set the reverb level high for the move sound
				if (!actionKeyHeld) {
					snekMoveTimelinePosition = (200 + snekMoveTimelinePositionMax);
					actionKeyHeld = true;
				}
			}

			
			//if nobody got a fruit, and nobody is holding any action keys, then..
			else {

				actionKeyHeld = false;		//nobody is holding any action keys anymore

				snakeMoveInstance->setPitch(proximityToFruit);
				snakeMoveInstance->setTimelinePosition(snekMoveTimelinePosition);
				snakeMoveInstance->setParameterByName("Reverb Wet", snakeMoveReverbLevel);
				
				if (isScoreUnder11) {
					snakeMoveInstance->start();
				}

				snekMoveTimelinePosition += 200;
				if (snekMoveTimelinePosition >= snekMoveTimelinePositionMax) {
					snekMoveTimelinePosition = 0;
				}
			}



			//DRUMS//			
			if (gotNewFruit && i16thNote == 1) {
				a808DrumInstance->start();
				cymbalInstance->start();
			}
			if (i16thNote == 1 || i16thNote == 11 || i16thNote == 16) {
				kickInstance->start();
			}
			
			if (i16thNote == 5 || i16thNote == 13) {
				if (gotNewFruit) {
					snare2Instance->start();
				}
				else {
					snare1Instance->start();
				}
			}
			if (hiHatToggle) {
				if (i16thNote % 2 == 1) {					
					closedHiHatInstance->start();
					closedHiHatInstance->setVolume(0.4f);
				}
				else {					
					closedHiHatInstance->start();
					closedHiHatInstance->setVolume(0.1f);
				}
			}
			
			//CHORDS//
			if (chordsStartToggle) {
				if (i16thNote == 1) {
					switch (currentChord) {
					case 0:
						chordsInstance->start();
						currentChord++;
						break;
					case 1:
						chordsInstance->setTimelinePosition(2023);
						currentChord++;
						break;
					case 2:
						chordsInstance->setTimelinePosition(4046);
						currentChord++;
						break;
					case 3:
						chordsInstance->setTimelinePosition(6068);
						currentChord = 0;
						break;
					}
				}					
			

				//ARP//
				switch (currentChord) {
				case 1:
					switch (i16thNote) {
					case 1:
						arpInstance->start();
						break;
					case 3:
						arpInstance->setTimelinePosition(170);
						break;
					case 5:
						arpInstance->setTimelinePosition(338);
						break;
					case 7:
						arpInstance->setTimelinePosition(507);
						break;
					case 9:
						arpInstance->setTimelinePosition(675);
						break;
					case 11:
						arpInstance->setTimelinePosition(844);
						break;
					case 13:
						arpInstance->setTimelinePosition(1012);
						break;
					case 15:
						arpInstance->setTimelinePosition(1181);
						break;
					}
					break;

				case 2:
					switch (i16thNote) {
					case 1:
						arpInstance->setTimelinePosition(2023);
						break;
					case 3:
						arpInstance->setTimelinePosition(2192);
						break;
					case 5:
						arpInstance->setTimelinePosition(2361);
						break;
					case 7:
						arpInstance->setTimelinePosition(2529);
						break;
					case 9:
						arpInstance->setTimelinePosition(2698);
						break;
					case 11:
						arpInstance->setTimelinePosition(2866);
						break;
					case 13:
						arpInstance->setTimelinePosition(3035);
						break;
					case 15:
						arpInstance->setTimelinePosition(3203);
						break;				
					}
					break;

				case 3:
					switch (i16thNote) {
					case 1:
						arpInstance->setTimelinePosition(4046);
						break;
					case 3:
						arpInstance->setTimelinePosition(4214);
						break;
					case 5:
						arpInstance->setTimelinePosition(4383);
						break;
					case 7:
						arpInstance->setTimelinePosition(4552);
						break;
					case 9:
						arpInstance->setTimelinePosition(4720);
						break;
					case 11:
						arpInstance->setTimelinePosition(4889);
						break;
					case 13:
						arpInstance->setTimelinePosition(5057);
						break;
					case 15:
						arpInstance->setTimelinePosition(5226);
						break;					
					}
					break;

				case 0:
					switch (i16thNote) {
					case 1:
						arpInstance->setTimelinePosition(6068);
						break;
					case 3:
						arpInstance->setTimelinePosition(6237);
						break;
					case 5:
						arpInstance->setTimelinePosition(6405);
						break;
					case 7:
						arpInstance->setTimelinePosition(6574);
						break;
					case 9:
						arpInstance->setTimelinePosition(6743);
						break;
					case 11:
						arpInstance->setTimelinePosition(6911);
						break;
					case 13:
						arpInstance->setTimelinePosition(7080);
						break;
					case 15:
						arpInstance->setTimelinePosition(7248);
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

					else if (display[q][t] == '8' && snek1[0].direction_frame == 'n') {
						screenString[q + (80 * t)] = '^';
					}

					else if (display[q][t] == '8' && snek1[0].direction_frame == 's') {
						screenString[q + (80 * t)] = 'v';
					}

					else if (display[q][t] == '8' && snek1[0].direction_frame == 'w') {
						screenString[q + (80 * t)] = '<';
					}

					else if (display[q][t] == '8' && snek1[0].direction_frame == 'e') {
						screenString[q + (80 * t)] = '>';
					}

					else if (display[q][t] == 'p') {
						screenString[q + (80 * t)] = 'O';
					}

					else {
						screenString[q + (80 * t)] = display[q][t];
					}
				}

				screenString[q + (80 * t)] = '|';

				if (t == 0 && q == 25) {
					screenString.replace(1 + q + (80 * t), 45, L"       __    _    _              _  __   ____");
				}

				else if (t == 1 && q == 25) {
					screenString.replace(1 + q + (80 * t), 46, L"      / /   | \\  | |     /\\     | |/ /  |  __|");
				}
				
				else if (t == 2 && q == 25) {
					screenString.replace(1 + q + (80 * t), 45, L"      \\ \\   |  \\ | |    /  \\    | | /   | |__");
				}

				else if (t == 3 && q == 25) {
					screenString.replace(1 + q + (80 * t), 46, L"       \\ \\  | | \\| |   / /\\ \\   |   \\   |  __|");
				}

				else if (t == 4 && q == 25) {
					screenString.replace(1 + q + (80 * t), 45, L"       / /  | |\\ \\ |  /  __  \\  | |\\ \\  | |__");
				}

				else if (t == 5 && q == 25) {
					screenString.replace(1 + q + (80 * t), 46, L"      /_/   |_| \\__| /__/  \\__\\ |_| \\_\\ |____|");
				}

				else if (t == 7 && q == 25 && playerCount == 1) {
					
					screenString.replace(1 + q + (80 * t), 14, L"       SCORE: ");
					
					wstring snekLengthString = to_wstring(highestCurrentLength);
										
					screenString.replace(15 + q + (80 * t), snekLengthString.length(), snekLengthString);

					screenString.replace(15 + snekLengthString.length() + q + (80 * t), 17, L"     HIGH SCORE: ");

					
					
					wstring highScoreString = to_wstring(highScore);

					screenString.replace(32 + snekLengthString.length() + q + (80 * t), highScoreString.length(), highScoreString);

				}

				else if (t == 7 && q == 25 && playerCount == 2) {

					screenString.replace(1 + q + (80 * t), 14, L"    P1 SCORE: ");

					wstring snekLengthString0 = to_wstring(snek1[0].snek_length);

					screenString.replace(15 + q + (80 * t), snekLengthString0.length(), snekLengthString0);

					screenString.replace(1 + q + (80 * (t + 1)), 14, L"    P2 SCORE: ");

					wstring snekLengthString1 = to_wstring(snek1[1].snek_length);

					screenString.replace(15 + q + (80 * (t + 1)), snekLengthString1.length(), snekLengthString1);

					screenString.replace(15 + snekLengthString0.length() + q + (80 * t), 17, L"     HIGH SCORE: ");
										
					wstring highScoreString = to_wstring(highScore);

					screenString.replace(32 + snekLengthString0.length() + q + (80 * t), highScoreString.length(), highScoreString);

				}

				else if (t == 9 && q == 25 && highestCurrentLength > 10) {

					screenString.replace(1 + q + (80 * t), 14, L"       STYLE: ");

					wstring styleCounterString = to_wstring(styleCounter);

					screenString.replace(15 + q + (80 * t), styleCounterString.length(), styleCounterString);

					screenString.replace(16 + styleCounterString.length() + q + (80 * t), 17, L"     HIGH STYLE: ");

					wstring highStyleString = to_wstring(styleHighScore);

					screenString.replace(33 + styleCounterString.length() + q + (80 * t), highStyleString.length(), highStyleString);

				}

				/*else if (t == 17 && q == 25 && playerCount == 1) {
					screenString.replace(8 + q + (80 * t), 33, L"use arrow keys ^ v < > to control");
				}					

				else if (t == 21 && q == 25 && highestCurrentLength > 10 && playerCount == 1) {
					screenString.replace(14 + q + (80 * t), 18, L"use Z key to lunge");
				}*/
								
			}

			for (int pt = 0; pt < playerCount; pt++) {
				switch (snek1[pt].direction_frame) {
				case 'n':
					screenString[snek1[pt].snek_head[0] + (80 * snek1[pt].snek_head[1])] = '^';
					break;
				case 's':
					screenString[snek1[pt].snek_head[0] + (80 * snek1[pt].snek_head[1])] = 'v';
					break;
				case 'e':
					screenString[snek1[pt].snek_head[0] + (80 * snek1[pt].snek_head[1])] = '>';
					break;
				case 'w':
					screenString[snek1[pt].snek_head[0] + (80 * snek1[pt].snek_head[1])] = '<';
					break;
				}
			}

			for (int yt = 0; yt < portalCount*2; yt++) {				
				screenString[portalCoordinates[yt][0] + (80 * portalCoordinates[yt][1])] = 'O';
			}

			if (wasPreviousHighScoreFound) {
				screenString.replace(64 + (80 * 6), 13, L"-------------");
				screenString.replace(63 + (80 * 7), 2, L"| ");
				screenString.replace(((11 - highScoreName.length()) / 2) + 65 + (80 * 7), highScoreName.length(), highScoreName);
				screenString.replace(76 + (80 * 7), 2, L" |");
				screenString.replace(64 + (80 * 8), 13, L"-------------");
			}			

			if (playerCount == 2 && currentFrame == 1) {

				screenString.replace(10 * 80 + 14, 8, L"Player 1");
				screenString.replace(11 * 80 + 17, 1, L"|");				

				screenString.replace(10 * 80 + 4, 8, L"Player 2");
				screenString.replace(11 * 80 + 7, 1, L"|");	
			}
			

			WriteConsoleOutputCharacter(hConsole, screenString.c_str(), nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);

			  //				  //
			 // COLOR THE SCREEN //
			//				    //

			//reset entire display color to green//
			for (int yt = 0; yt < nScreenWidth * nScreenHeight; yt++) {		
				attributes[yt] = FOREGROUND_GREEN;
			}

			  //					  //
			 // COLOR THE FRUIT PINK //
			//						//
			attributes[currentFruit[0] + (currentFruit[1] * 80)] = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY;		

			  //					  //
			 // COLOR PLAYER 1 GREEN //
			//						//			
			
			if (!snek1[0].justDied) {
				attributes[snek1[0].snek_head[0] + (snek1[0].snek_head[1] * 80)] = FOREGROUND_GREEN | FOREGROUND_INTENSITY;

				if (!snek1[0].justGotNewFruit) {
					for (int l = 0; l < snek1[0].snek_length; l++) {
						attributes[snek1[0].snek_body[l][0] + (snek1[0].snek_body[l][1] * 80)] = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
					}
					if (snek1[0].snekSwallowTimer <= snek1[0].snek_length) {
						attributes[snek1[0].snek_body[snek1[0].snekSwallowTimer - 1][0] + (snek1[0].snek_body[snek1[0].snekSwallowTimer - 1][1] * 80)] = FOREGROUND_BLUE | FOREGROUND_RED;
						snek1[0].snekSwallowTimer++;
					}
				}
				else {
					snek1[0].justGotNewFruit = false;
					for (int l = 0; l < snek1[0].snek_length - 1; l++) {
						attributes[snek1[0].snek_body[l][0] + (snek1[0].snek_body[l][1] * 80)] = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
					}
					if (snek1[0].snekSwallowTimer == 0) {
						attributes[snek1[0].snek_head[0] + (snek1[0].snek_head[1] * 80)] = FOREGROUND_BLUE | FOREGROUND_RED;
						snek1[0].snekSwallowTimer++;
					}
				}
			}
			else {
				attributes[snek1[0].snek_head[0] + (snek1[0].snek_head[1] * 80)] = FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY;
				for (int l = 0; l < snek1[0].snek_length - 1; l++) {
					attributes[snek1[0].snek_body[l][0] + (snek1[0].snek_body[l][1] * 80)] = FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY;
				}
			}
				
			  //					//
			 // COLOR PLAYER 2 RED //
			//					  //
			if (playerCount == 2) {
				if (!snek1[1].justDied) {
					attributes[snek1[1].snek_head[0] + (snek1[1].snek_head[1] * 80)] = FOREGROUND_RED | FOREGROUND_INTENSITY;

					if (!snek1[1].justGotNewFruit) {
						for (int l = 0; l < snek1[1].snek_length; l++) {
							attributes[snek1[1].snek_body[l][0] + (snek1[1].snek_body[l][1] * 80)] = FOREGROUND_RED | FOREGROUND_INTENSITY;
						}
						if (snek1[1].snekSwallowTimer <= snek1[1].snek_length) {
							attributes[snek1[1].snek_body[snek1[1].snekSwallowTimer - 1][0] + (snek1[1].snek_body[snek1[1].snekSwallowTimer - 1][1] * 80)] = FOREGROUND_BLUE | FOREGROUND_RED;
							snek1[1].snekSwallowTimer++;
						}
					}
					else {
						snek1[1].justGotNewFruit = false;
						for (int l = 0; l < snek1[1].snek_length - 1; l++) {
							attributes[snek1[1].snek_body[l][0] + (snek1[1].snek_body[l][1] * 80)] = FOREGROUND_RED | FOREGROUND_INTENSITY;
						}
						if (snek1[1].snekSwallowTimer == 0) {
							attributes[snek1[1].snek_head[0] + (snek1[1].snek_head[1] * 80)] = FOREGROUND_BLUE | FOREGROUND_RED;
							snek1[1].snekSwallowTimer++;
						}
					}
				}
				else {
					attributes[snek1[1].snek_head[0] + (snek1[1].snek_head[1] * 80)] = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
					for (int l = 0; l < snek1[1].snek_length - 1; l++) {
						attributes[snek1[1].snek_body[l][0] + (snek1[1].snek_body[l][1] * 80)] = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
					}
				}
			}
			
			
			WriteConsoleOutputAttribute(hConsole, &attributes[0], nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
						
			// delay for first frame if in 2 player mode //
			if (playerCount == 2 && currentFrame == 1)
			Sleep(3000);
			
			system->update(); //update FMOD system			
		}

		  //				  //
		 // GAME OVER SCREEN //
		//					//

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

			

			screenString.replace((80 * 9) + 32, 32, L"NEW HIGH SCORE! ENTER YOUR NAME:");
			screenString.replace(65 + (80 * 7), 11, L"           ");

			//..display the keyboard..
			for (int y = 0; y < 32; y++) {
				if (y < 8) {
					screenString[40 + (13 * 80) + (y * 2)] = keyboard[y];
				}
				else if (y < 16) {
					screenString[24 + (15 * 80) + (y * 2)] = keyboard[y];
				}
				else if (y < 24) {
					screenString[8 + (17 * 80) + (y * 2)] = keyboard[y];
				}
				else {
					screenString[-8 + (19 * 80) + (y * 2)] = keyboard[y];
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
					}
					else if (currentSelChar == 29 && highScoreName.length() < 11 && highScoreName.length() != 0) {
						highScoreName.append(1, keyboard[currentSelChar]);
					}
					else if (currentSelChar == 30) {
						if (highScoreName.size() > 0) {
							highScoreName.resize(highScoreName.size() - 1);
						}						
					}
					else if (currentSelChar == 31) {
						nameEntry = false;
					}
					
					holdNameEntryZ = true;
				}
				//..if they release the Z key..
				else if (!snek1[0].action_keys) {
					holdNameEntryZ = false;
				}


				//erase name display
				screenString.replace(65 + (80 * 7), 11, L"           ");


				//display their name as they type it
				screenString.replace(64 + (80 * 6), 13, L"-------------");
				screenString.replace(63 + (80 * 7), 2, L"| ");

				if (highScoreName.length() == 11) {
					screenString.replace(65 + (80 * 7), highScoreName.length(), highScoreName);
				}
				else {
					screenString.replace(((11 - highScoreName.length()) / 2) + 65 + (80 * 7), highScoreName.length() + 1, highScoreName + L" ");
				}				

				screenString.replace(76 + (80 * 7), 2, L" |");
				screenString.replace(64 + (80 * 8), 13, L"-------------");
				//screenString.replace((7 * 80) + 64 + highScoreName.length(), 1, L" ");			//delete any extra characters from backspace/delete inputs
							   				 			  

				//color the whole right side of the screen green
				for (int p = 0; p < 25; p++) {
					for (int t = 26; t < 80; t++) {
						attributes[(p * 80) + t] = FOREGROUND_GREEN;
					}					
				}

				//invert the color of the currently selected keyboard character
				if (currentSelChar < 8) {
					attributes[40 + (13 * 80) + (currentSelChar * 2)] = BACKGROUND_GREEN;
				}
				else if (currentSelChar < 16) {
					attributes[24 + (15 * 80) + (currentSelChar * 2)] = BACKGROUND_GREEN;
				}
				else if (currentSelChar < 24) {
					attributes[8 + (17 * 80) + (currentSelChar * 2)] = BACKGROUND_GREEN;
				}
				else {
					attributes[-8 + (19 * 80) + (currentSelChar * 2)] = BACKGROUND_GREEN;
				}

				//DISPLAY THE SCREEN//
				WriteConsoleOutputCharacter(hConsole, screenString.c_str(), nScreenWidth* nScreenHeight, { 0,0 }, & dwBytesWritten);
				WriteConsoleOutputAttribute(hConsole, &attributes[0], nScreenWidth* nScreenHeight, { 0,0 }, & dwBytesWritten);
			}

			  //						  //
			 // WRITE HIGH SCORE TO FILE //
			//							//
			ofstream scoreFileWrite;
			scoreFileWrite.open("ScoreFile", ios::trunc);
			scoreFileWrite << to_string(highScore) << endl << converter.to_bytes(highScoreName);
			scoreFileWrite.close();	

			wasPreviousHighScoreFound = true;
		}			

		screenString.replace((80 * 9) + 32, 32, L"           HIGH SCORE SAVED!    ");
		screenString.replace(40 + (13 * 80), 39, L"                                       ");
		screenString.replace(40 + (14 * 80), 39, L"                                       ");
		screenString.replace(40 + (15 * 80), 39, L"                                       ");
		screenString.replace(40 + (16 * 80), 39, L"                                       ");
		screenString.replace(40 + (17 * 80), 39, L"                                       ");
		screenString.replace(40 + (18 * 80), 39, L"                                       ");
		screenString.replace(40 + (19 * 80), 39, L"                                       ");
		screenString.replace(40 + (20 * 80), 39, L"                                       ");


		//color the whole right side of the screen green
		for (int p = 0; p < 25; p++) {
			for (int t = 26; t < 80; t++) {
				attributes[(p * 80) + t] = FOREGROUND_GREEN;
			}
		}
				
		Sleep(100);
		
		if (playerCount == 2) {
			if (snek1[0].justDied) {
				screenString.replace(19 + 25 + (80 * 12), 18, L"PLAYER 1 DIED!    ");
			}
			else {
				screenString.replace(19 + 25 + (80 * 12), 18, L"PLAYER 2 DIED!    ");
			}
		}
		else {
			screenString.replace(14 + 25 + (80 * 21), 18, L"                  ");
		}
		
		screenString.replace(8 + 25 + (80 * 17), 33, L"                                 ");

		fancyBossInstance->start();			//(FMOD)
		system->update(); //begin FMOD sound generation/song playback

			screenString.replace((nScreenHeight * nScreenWidth) - 753, 9, L"GAME OVER");
			screenString.replace((nScreenHeight * nScreenWidth) - 360, 24, L">Press [Z] to play again");
			screenString.replace((nScreenHeight * nScreenWidth) - 280, 18, L">Press [X] to quit");
			screenString.replace((18 * 80) + 46, 12, L"Players: <" + to_wstring(playerCount) + L">");

			WriteConsoleOutputCharacter(hConsole, screenString.c_str(), nScreenWidth * nScreenHeight, { 0,0 }, & dwBytesWritten);
			WriteConsoleOutputAttribute(hConsole, &attributes[0], nScreenWidth * nScreenHeight, { 0,0 }, & dwBytesWritten);

			bool gameOverMessage = true;

			bool xKey;

			this_thread::sleep_for(527ms);

			while (gameOverMessage) {

				for (int o = 0; o < 4; o++) {
					arrowKeys[o] = (0x8000 & GetAsyncKeyState((unsigned char)("\x25\x26\x27\x28"[o]))) != 0;
				}

				if (arrowKeys[2] && playerCount < 2 && !holdKey) {
					playerCount++; 
					snakeFruitInstance->setPitch(1.77f);
					snakeFruitInstance->start();
					system->update();
					holdKey = true;
				}

				else if (arrowKeys[0] && playerCount > 1 && !holdKey) {
					playerCount--;
					snakeFruitInstance->setPitch(0.64f);
					snakeFruitInstance->start();
					system->update();
					holdKey = true;
				}

				if (holdKey && !arrowKeys[0] && !arrowKeys[2]) {
					holdKey = false;
				}

				screenString.replace((18 * 80) + 46, 12, L"Players: <" + to_wstring(playerCount) + L">");

				WriteConsoleOutputCharacter(hConsole, screenString.c_str(), nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
								
				if (zKey = (0x8000 & GetAsyncKeyState((unsigned char)("Z"[0]))) != 0) {
					gameOverMessage = false;
					playAgain = true;
					
					fancyBossInstance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);		//(FMOD)
					snakeFruitInstance->setPitch(1.0f);
					snakeFruitInstance->start();
					system->update();
				}



				if (xKey = (0x8000 & GetAsyncKeyState((unsigned char)("X"[0]))) != 0) {
					gameOverMessage = false;
					playAgain = false;

					fancyBossInstance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);		//(FMOD)
					exitGameInstance->start();

					system->update();
					this_thread::sleep_for(2671ms);
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
// UNUSED
//int currentTrap = 0;			//current number of traps on the game grid
//int trapLocations[200][2];	//locations of the traps on the game grid
//int r = 4;					//used for counting the number of traps whose locations have been set
//int actualTrapCount;
//bool alternator1 = true;

/*
//E
if (snek1[pt].direction_frame == 'e' && ((snek1[pt].action_keys) && display[snek1[pt].snek_head[0] + 1][snek1[pt].snek_head[1]] == '8' || display[snek1[pt].snek_head[0] + 1][snek1[pt].snek_head[1]] == 'X' || display[snek1[pt].snek_head[0] + 1][snek1[pt].snek_head[1]] == '7' && display[snek1[pt].snek_head[0] + 2][snek1[pt].snek_head[1]] == 'z' || display[snek1[pt].snek_head[0] + 2][snek1[pt].snek_head[1]] == '+' && snek1[pt].snek_head[0] + 2 < 25)) {
	styleCounter++;
	styleCounter++;
}

if (snek1[pt].direction_frame == 'e' && ((display[snek1[pt].snek_head[0] + 1][snek1[pt].snek_head[1] + 1] == '8' || display[snek1[pt].snek_head[0] + 1][snek1[pt].snek_head[1] + 1] == 'X' || display[snek1[pt].snek_head[0] + 1][snek1[pt].snek_head[1] + 1] == '7') && (display[snek1[pt].snek_head[0] + 1][snek1[pt].snek_head[1] - 1] == '8' || display[snek1[pt].snek_head[0] + 1][snek1[pt].snek_head[1] - 1] == 'X' || display[snek1[pt].snek_head[0] + 1][snek1[pt].snek_head[1] - 1] == '7')) && (display[snek1[pt].snek_head[0] + 1][snek1[pt].snek_head[1]] == 'z' || display[snek1[pt].snek_head[0] + 1][snek1[pt].snek_head[1]] == '+')) {
	styleCounter++;
}

//W
if (snek1[pt].direction_frame == 'w' && ((snek1[pt].action_keys) && display[snek1[pt].snek_head[0] - 1][snek1[pt].snek_head[1]] == '8' || display[snek1[pt].snek_head[0] - 1][snek1[pt].snek_head[1]] == 'X' || display[snek1[pt].snek_head[0] - 1][snek1[pt].snek_head[1]] == '7' && display[snek1[pt].snek_head[0] - 2][snek1[pt].snek_head[1]] == 'z' || display[snek1[pt].snek_head[0] - 2][snek1[pt].snek_head[1]] == '+' && snek1[pt].snek_head[0] - 2 >= 0)) {
	styleCounter++;
	styleCounter++;
}

if (snek1[pt].direction_frame == 'w' && ((display[snek1[pt].snek_head[0] - 1][snek1[pt].snek_head[1] + 1] == '8' || display[snek1[pt].snek_head[0] - 1][snek1[pt].snek_head[1] + 1] == 'X' || display[snek1[pt].snek_head[0] - 1][snek1[pt].snek_head[1] + 1] == '7') && (display[snek1[pt].snek_head[0] - 1][snek1[pt].snek_head[1] - 1] == '8' || display[snek1[pt].snek_head[0] - 1][snek1[pt].snek_head[1] - 1] == 'X' || display[snek1[pt].snek_head[0] - 1][snek1[pt].snek_head[1] - 1] == '7')) && (display[snek1[pt].snek_head[0] - 1][snek1[pt].snek_head[1]] == 'z' || display[snek1[pt].snek_head[0] - 1][snek1[pt].snek_head[1]] == '+')) {
	styleCounter++;
}

//S
if (snek1[pt].direction_frame == 's' && ((snek1[pt].action_keys) && display[snek1[pt].snek_head[0]][snek1[pt].snek_head[1] + 1] == '8' || display[snek1[pt].snek_head[0]][snek1[pt].snek_head[1] + 1] == 'X' || display[snek1[pt].snek_head[0]][snek1[pt].snek_head[1] + 1] == '7' && display[snek1[pt].snek_head[0]][snek1[pt].snek_head[1] + 2] == 'z' || display[snek1[pt].snek_head[0]][snek1[pt].snek_head[1] + 2] == '+' && snek1[pt].snek_head[1] + 2 < 25)) {
	styleCounter++;
	styleCounter++;
}

if (snek1[pt].direction_frame == 's' && ((display[snek1[pt].snek_head[0] + 1][snek1[pt].snek_head[1] + 1] == '8' || display[snek1[pt].snek_head[0] + 1][snek1[pt].snek_head[1] + 1] == 'X' || display[snek1[pt].snek_head[0] + 1][snek1[pt].snek_head[1] + 1] == '7') && (display[snek1[pt].snek_head[0] - 1][snek1[pt].snek_head[1] + 1] == '8' || display[snek1[pt].snek_head[0] - 1][snek1[pt].snek_head[1] + 1] == 'X' || display[snek1[pt].snek_head[0] - 1][snek1[pt].snek_head[1] + 1] == '7')) && (display[snek1[pt].snek_head[0]][snek1[pt].snek_head[1] + 1] == 'z' || display[snek1[pt].snek_head[0]][snek1[pt].snek_head[1] + 1] == '+')) {
	styleCounter++;
}

//N
if (snek1[pt].direction_frame == 'n' && ((snek1[pt].action_keys) && display[snek1[pt].snek_head[0]][snek1[pt].snek_head[1] - 1] == '8' || display[snek1[pt].snek_head[0]][snek1[pt].snek_head[1] - 1] == 'X' || display[snek1[pt].snek_head[0]][snek1[pt].snek_head[1] - 1] == '7' && display[snek1[pt].snek_head[0]][snek1[pt].snek_head[1] - 2] == 'z' || display[snek1[pt].snek_head[0]][snek1[pt].snek_head[1] - 2] == '+' && snek1[pt].snek_head[1] - 2 >= 0)) {
	styleCounter++;
	styleCounter++;
}

if (snek1[pt].direction_frame == 'n' && ((display[snek1[pt].snek_head[0] + 1][snek1[pt].snek_head[1] - 1] == '8' || display[snek1[pt].snek_head[0] + 1][snek1[pt].snek_head[1] - 1] == 'X' || display[snek1[pt].snek_head[0] + 1][snek1[pt].snek_head[1] - 1] == '7') && (display[snek1[pt].snek_head[0] - 1][snek1[pt].snek_head[1] - 1] == '8' || display[snek1[pt].snek_head[0] - 1][snek1[pt].snek_head[1] - 1] == 'X' || display[snek1[pt].snek_head[0] - 1][snek1[pt].snek_head[1] - 1] == '7')) && (display[snek1[pt].snek_head[0]][snek1[pt].snek_head[1] - 1] == 'z' || display[snek1[pt].snek_head[0]][snek1[pt].snek_head[1] - 1] == '+')) {
	styleCounter++;
}

*/

				/*else if (snek1[pt].action_keys && snek1[pt].snek_length > 10) {
					snakeLungeInstance->setPitch(proximityToFruit);		//(FMOD)
					snakeLungeInstance->start();
					snakeMoveInstance->setParameterByName("Reverb Wet", 1.0f);
					if (!snek1[pt].holdAction) {
						snekMoveTimelinePosition += 200;
						snek1[pt].holdAction = true;
					}
				}*/

				/*else {

					snek1[pt].holdAction = false;

					snakeMoveInstance->setPitch(proximityToFruit);		//(FMOD)
					snakeMoveInstance->setTimelinePosition(snekMoveTimelinePosition);
					snakeMoveInstance->setParameterByName("Reverb Wet", snakeMoveReverbLevel);

					if (highestCurrentLength != 0 && (isScoreUnder11 || snakeMoveInstance->getPlaybackState(NULL) == FMOD_STUDIO_PLAYBACK_SUSTAINING || snakeMoveInstance->getPlaybackState(NULL) == FMOD_STUDIO_PLAYBACK_STOPPED)) {
						snakeMoveInstance->start();	//(FMOD)
					}

					/*
					else if (highestCurrentLength == 0 && currentFrame % 2 == 1)	{
						criticalInstance->setPitch(proximityToFruit);
						criticalInstance->start();
					}
					else {
						criticalInstance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);
					}


					snekMoveTimelinePosition += 200;

					if (snekMoveTimelinePosition >= snekMoveTimelinePositionMax) {
						snekMoveTimelinePosition = 0;
					}
				}*/

			/*
			//DETERMINE TRAP LOCATIONS/////////////

			if (currentTrap < snek1[0].snek_length) {
				currentTrap++;

				if (currentTrap == 3) {

					for (int e = 0; e == 0;) {
						trapLocations[0][0] = rand() % 25;
						trapLocations[0][1] = rand() % 25;

						if ((trapLocations[0][0] != snek1[0].snek_head[0] && trapLocations[0][1] != snek1[0].snek_head[1]) && display[trapLocations[0][0]][trapLocations[0][1]] != '7' && display[trapLocations[0][0]][trapLocations[0][1]] != '+') {

							actualTrapCount++;

							e = 1;
						}
					}
				}

				if (currentTrap == 7) {

					for (int e = 0; e == 0;) {
						trapLocations[1][0] = rand() % 25;
						trapLocations[1][1] = rand() % 25;

						if ((trapLocations[1][0] != snek1[0].snek_head[0] && trapLocations[1][1] != snek1[0].snek_head[1]) && display[trapLocations[1][0]][trapLocations[1][1]] != '7' && display[trapLocations[1][0]][trapLocations[1][1]] != '+') {

							actualTrapCount++;

							e = 1;
						}
					}
				}

				if (currentTrap == 11) {

					for (int e = 0; e == 0;) {
						trapLocations[2][0] = rand() % 25;
						trapLocations[2][1] = rand() % 25;

						if ((trapLocations[2][0] != snek1[0].snek_head[0] && trapLocations[2][1] != snek1[0].snek_head[1]) && display[trapLocations[2][0]][trapLocations[2][1]] != '7' && display[trapLocations[2][0]][trapLocations[2][1]] != '+') {

							actualTrapCount++;

							e = 1;
						}
					}

					for (int e = 0; e == 0;) {
						trapLocations[3][0] = rand() % 25;
						trapLocations[3][1] = rand() % 25;

						if ((trapLocations[3][0] != snek1[0].snek_head[0] && trapLocations[3][1] != snek1[0].snek_head[1]) && display[trapLocations[3][0]][trapLocations[3][1]] != '7' && display[trapLocations[3][0]][trapLocations[3][1]] != '+') {

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

							if ((trapLocations[r][0] != snek1[0].snek_head[0] && trapLocations[r][1] != snek1[0].snek_head[1]) && display[trapLocations[r][0]][trapLocations[r][1]] != '7' && display[trapLocations[r][0]][trapLocations[r][1]] != '+') {

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
			if (actualTrapCount < 0 && display[snek1[0].snek_head[0]][snek1[0].snek_head[1]] == 'X') {
				gameLose = true;
				break;

			}
			*/



/*for (int q = 0; q < 25; q++) {

	cout << q << " = " << display[q] << endl;

}*/

/*for (int q = 0; q < 25; q++) {

	display[q] = 0;

}*/

/*
cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snek1[0].snek_head[0] == z) { cout << "="; } z++; //1 
cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snek1[0].snek_head[0] == z) { cout << "="; } z++; //2 
cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snek1[0].snek_head[0] == z) { cout << "="; } z++; //3 
cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snek1[0].snek_head[0] == z) { cout << "="; } z++; //4 
cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snek1[0].snek_head[0] == z) { cout << "="; } z++; //5 
cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snek1[0].snek_head[0] == z) { cout << "="; } z++; //6 
cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snek1[0].snek_head[0] == z) { cout << "="; } z++; //7 
cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snek1[0].snek_head[0] == z) { cout << "="; } z++; //8 
cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snek1[0].snek_head[0] == z) { cout << "="; } z++; //9 
cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snek1[0].snek_head[0] == z) { cout << "="; } z++; //10
cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snek1[0].snek_head[0] == z) { cout << "="; } z++; //11
cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snek1[0].snek_head[0] == z) { cout << "="; } z++; //12
cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snek1[0].snek_head[0] == z) { cout << "="; } z++; //13
cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snek1[0].snek_head[0] == z) { cout << "="; } z++; //14
cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snek1[0].snek_head[0] == z) { cout << "="; } z++; //15
cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snek1[0].snek_head[0] == z) { cout << "="; } z++; //16
cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snek1[0].snek_head[0] == z) { cout << "="; } z++; //17
cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snek1[0].snek_head[0] == z) { cout << "="; } z++; //18
cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snek1[0].snek_head[0] == z) { cout << "="; } z++; //19
cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snek1[0].snek_head[0] == z) { cout << "="; } z++; //20
cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snek1[0].snek_head[0] == z) { cout << "="; } z++; //21
cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snek1[0].snek_head[0] == z) { cout << "="; } z++; //22
cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snek1[0].snek_head[0] == z) { cout << "="; } z++; //23
cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snek1[0].snek_head[0] == z) { cout << "="; } z++; //24
cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snek1[0].snek_head[0] == z) { cout << "="; } z++; //25

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
if (snek1[0].snek_head[0] >= 25) {
	direction = 'w';

}

else if (snek1[0].snek_head[0] <= 1) {
	direction = 'e';

}
*/

/*if (snek1[0].snek_head[0] - 1 < 0 && direction == 'w' || snek1[0].snek_head[0] + 1 > 25 && direction == 'e' || snek1[0].snek_head[1] - 1 < 0 && direction == 'n' || snek1[0].snek_head[1] + 1 > 25 && direction == 's') {
				gameLose = true;

			}*/