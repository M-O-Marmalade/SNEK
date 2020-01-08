//		 __    _    _              _  __   ____
//		/ /   | \  | |     /\     | |/ /  |  __|
//		\ \   |  \ | |    /  \    | | /   | |__
//		 \ \  | | \| |   / /\ \   |   \   |  __|
//		 / /  | |\ \ |  /  __  \  | |\ \  | |__
//		/_/   |_| \__| /__/  \__\ |_| \_\ |____|
// / / BY M. O. MARMALADE / / / / / / / / / / / / /

#include <Windows.h>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <cstdlib>
#include "fmod.hpp"
#include "fmod_studio.hpp"

using namespace std;

//80w x 25h

		//VARIABLES//

char display[25][25]{'z'};		//the game display (text-pixels)

int snekHead[2];				//the snek's head

int snekBody[625][2];

int snekLength = 0;				//current length of the snek

int snekLengthZ = 1;			//z-key speed/length multiplier

int snekLengthDraw = 0;			//int used to draw the glory of the snekLength to the screen
	
char direction = 'w';			//direction of player movement

char direction1 = 'w';			//direction temporary slot for tick-resolution input

bool collision = false;			//tells if a collision has occured

bool bKey[4];					//stores user input

bool zKey;

bool holdW = false;				//checks if a directional key (arrow key) was held down during previous tick
bool holdE = false;
bool holdS = false;
bool holdN = false;

int currentFruit[2]{ rand() % 25, rand() % 25 };			//location of the current fruit

bool gameLose;					//tells if the game has been lost

int currentFrame = 0;			//keeps track of how many frames have passed

int currentTick = 0;			//keeps track of how many ticks have passed

char playAgain;					//decides whether or not to play again after losing

int highScore = 0;

int styleCounter = 0;

int styleHighScore = 0;

int frameRate = 10;

int currentTrap = 0;

int trapLocations[200][2];

int r = 4;					//used for counting the number of traps whose locations have been set

int actualTrapCount = 0;

bool alternator1 = true;



// 8 is snek head
// 7 is snek body
// o is food
// X is trap
// z is empty space

// (11.5 is halfway)

int nScreenWidth = 80;
int nScreenHeight = 25;
string screenString;

void SleepinnnThang() {
	this_thread::sleep_for(33ms);
}


int main() {

	//FMOD PROGRAMMING

	FMOD_RESULT result;
	FMOD::Studio::System* system = NULL;

	result = FMOD::Studio::System::create(&system); // Create the Studio System object.

	result = system->initialize(256, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, 0);


	FMOD::Studio::Bank* masterBank = NULL;
	system->loadBankFile("media/Master.bank", FMOD_STUDIO_LOAD_BANK_NORMAL, &masterBank);

	FMOD::Studio::Bank* stringsBank = NULL;
	system->loadBankFile("media/Master.strings.bank", FMOD_STUDIO_LOAD_BANK_NORMAL, &stringsBank);



	FMOD::Studio::Bank* musicandFX = NULL;
	result = system->loadBankFile("media/MusicandFX.bank", FMOD_STUDIO_LOAD_BANK_NORMAL, &musicandFX);

	FMOD::Studio::EventDescription* splashJingleDescription = NULL;
	system->getEvent("event:/SplashJingle", &splashJingleDescription);

	FMOD::Studio::EventInstance* splashJingleInstance = NULL;
	splashJingleDescription->createInstance(&splashJingleInstance);

	FMOD::Studio::EventDescription* aNewChipDescription = NULL;
	system->getEvent("event:/ANewChip", &aNewChipDescription);

	FMOD::Studio::EventInstance* aNewChipInstance = NULL;
	aNewChipDescription->createInstance(&aNewChipInstance);

	FMOD::Studio::EventDescription* snakeFruitDescription = NULL;
	system->getEvent("event:/SnakeFruit", &snakeFruitDescription);

	FMOD::Studio::EventInstance* snakeFruitInstance = NULL;
	snakeFruitDescription->createInstance(&snakeFruitInstance);

	FMOD::Studio::EventDescription* snakeFruitDescription11 = NULL;
	system->getEvent("event:/SnakeFruit11", &snakeFruitDescription11);

	FMOD::Studio::EventInstance* snakeFruitInstance11 = NULL;
	snakeFruitDescription11->createInstance(&snakeFruitInstance11);

	FMOD::Studio::EventDescription* snakeMoveDescription = NULL;
	system->getEvent("event:/SnakeMove", &snakeMoveDescription);

	FMOD::Studio::EventInstance* snakeMoveInstance = NULL;
	snakeMoveDescription->createInstance(&snakeMoveInstance);

	FMOD::Studio::EventDescription* startButtonDescription = NULL;
	system->getEvent("event:/StartButton", &startButtonDescription);

	FMOD::Studio::EventInstance* startButtonInstance = NULL;
	startButtonDescription->createInstance(&startButtonInstance);
	
	FMOD::Studio::EventDescription* fancyBossDescription = NULL;
	system->getEvent("event:/FancyBoss", &fancyBossDescription);

	FMOD::Studio::EventInstance* fancyBossInstance = NULL;
	fancyBossDescription->createInstance(&fancyBossInstance);

	FMOD::Studio::EventDescription* snakeLungeDescription = NULL;
	system->getEvent("event:/SnakeLunge", &snakeLungeDescription);

	FMOD::Studio::EventInstance* snakeLungeInstance = NULL;
	snakeLungeDescription->createInstance(&snakeLungeInstance);
	
	FMOD::Studio::EventDescription* deathDescription = NULL;
	system->getEvent("event:/Death", &deathDescription);

	FMOD::Studio::EventInstance* deathInstance = NULL;
	deathDescription->createInstance(&deathInstance);
	
	//FMOD::Studio::EventDescription* proximitySoundDescription = NULL;
	//system->getEvent("event:/ProximitySound", &proximitySoundDescription);

	//FMOD::Studio::EventInstance* proximitySoundInstance = NULL;
	//proximitySoundDescription->createInstance(&proximitySoundInstance);


	
	//REST OF GAME PROGRAMMING

	screenString.resize(nScreenWidth * nScreenHeight);

	//create screen buffer
	char *screen = new char[nScreenWidth * nScreenHeight];
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;
	   	
	CONSOLE_CURSOR_INFO     cursorInfo;
	DWORD fontSize = 0;

	GetConsoleCursorInfo(hConsole, &cursorInfo);
	cursorInfo.bVisible = false; // set the cursor visibility
	SetConsoleCursorInfo(hConsole, &cursorInfo);

	//GetConsoleFontSize(hConsole, fontSize);

	/*
	SMALL_RECT *screenWindowCoordinates = new SMALL_RECT;
	screenWindowCoordinates->Top = 15;
	screenWindowCoordinates->Left = 0;
	screenWindowCoordinates->Bottom = 0;
	screenWindowCoordinates->Right = 79;
	SetConsoleWindowInfo(hConsole, true, screenWindowCoordinates);
	

	CHAR_INFO wAttributes;
	wAttributes.Attributes = FOREGROUND_GREEN;
	*/

	//SetConsoleTextAttribute(hConsole, wAttributes);
	for (int u = 0; u < (nScreenHeight * nScreenWidth); u++) {
		screen[u] = screenString[u];
	}

	//int secondsBro = time(0);
	int charToOverwrite = 992;
	bool animation = true;
	int u = 0;
	
	splashJingleInstance->start();

	system->update(); //begin FMOD sound generation/song playback

	while (animation) {

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
	
	
	/*while (time(0) < secondsBro + 1) {
		
		screenString.replace((12 * 80) + 32, 14, "Citrus Studios");
		
		
		for (int u = 0; u < (nScreenHeight * nScreenWidth); u++) {
			screen[u] = screenString[u];
			
		}
		
		WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);

		this_thread::sleep_for(347ms);
	}

	*/

	animation = true;
	charToOverwrite = 992;
	
	while (animation) {
		
		screen[charToOverwrite] = char(32);
		charToOverwrite++;
		
		WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
		
		this_thread::sleep_for(77ms);
		
		if (charToOverwrite == 1006) {
			animation = false;
			this_thread::sleep_for(222ms);
		}
		
	}
	
	bool startScreen = true;

	bool startToggle = true;

	int frameCount = 1;

	this_thread::sleep_for(777ms);

	aNewChipInstance->start();
	snakeFruitInstance->start();

	system->update(); //begin start screen playback

	while (startScreen) {

		for (int u = 0; u < nScreenHeight * nScreenWidth; u++) {

			screenString[u] = char(32);

		}

		screenString.replace((7 * 80) + 20, 38,   "__    _    _              _  __   ____");
		screenString.replace((8 * 80) + 19, 40,  "/ /   | \\  | |     /\\     | |/ /  |  __|");
		screenString.replace((9 * 80) + 19, 39,  "\\ \\   |  \\ | |    /  \\    | | /   | |__");
		screenString.replace((10 * 80) + 20, 39,  "\\ \\  | | \\| |   / /\\ \\   |   \\   |  __|");
		screenString.replace((11 * 80) + 20, 38,  "/ /  | |\\ \\ |  /  __  \\  | |\\ \\  | |__");
		screenString.replace((12 * 80) + 19, 40, "/_/   |_| \\__| /__/  \\__\\ |_| \\_\\ |____|");

		screenString.replace((22 * 80) + 33, 14, "Citrus Studios");

		if (frameCount == 111 && !startToggle) {
			startToggle = true;
			frameCount = 0;
			
			snakeFruitInstance->start();

			system->update(); //play snakefruitinstance sound for flashing "press start" button
		}

		if (startToggle && frameCount == 111) {
			startToggle = false;
			frameCount = 0;
		}

		if (startToggle) {
			screenString.replace((18 * 80) + 31, 18, "Press [Z] to start");
			
		}

		else { 
			
			screenString.replace((18 * 80) + 31, 18, "                  ");
		}

		for (int u = 0; u < (nScreenHeight * nScreenWidth); u++) {
			screen[u] = screenString[u];

		}

		WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
					
		frameCount++;

		this_thread::sleep_for(7ms);

		if (zKey = (0x8000 & GetAsyncKeyState((unsigned char)("Z"[0]))) != 0) {

			aNewChipInstance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);
			startButtonInstance->start();

			system->update();	//play startbutton sound for pressing 'Z' at start screen

			

			//animation

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

			startScreen = false;

		}
	}

	

	this_thread::sleep_for(1389ms);

	//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
	// 40 x 6
	// 25 - 6 = 19
	//19 / 2 = 9.5
	//9 low, 40 in
	//8 low, 39 in
	do {

		//PRE-GAME PREP///////////////////////////////

		//bool startKey = false;

		srand(time(0));			//seed the RNG using system time

		gameLose = false;		//reset game lose condition

		snekHead[0] = 12;		//reset player position to center
		snekHead[1] = 12;

		snekLength = 0;			//reset snek length

		snekLengthZ = 1;

		snekLengthDraw = 0;

		frameRate = 10;		//reset the framerate

		styleCounter = 0;	//reset the STYLE counter

		currentTrap = 0;

		r = 0;

		actualTrapCount = 0;

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

		int snekMoveTimelinePosition = 0;	//FMOD snakemovesound timeline position

		int snekMoveTimelinePositionMax = 200;

		bool isScoreUnder11 = true;

		float snakeMoveReverbLevel = 0.0f;

		float proximityToFruit;
		//proximitySoundInstance->start();

		bool wasZKeyHeld = false;

		while (gameLose == false) {		//GAME LOOP/////////////////////////////////////////////////////////////////////////

			


			//SET FRAMERATE//

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

			//GAME CLOCK////////////////////////

			for (int q = 0; q < frameRate; q++) {
				if (snekLength == 0) {
					this_thread::sleep_for(27ms);	//25ms per tick

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

				currentTick++;

				//READ PLAYER INPUT//

				for (int k = 0; k < 4; k++) {
					bKey[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x25\x26\x27\x28"[k]))) != 0;

				}

				
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

				//CHECK + SET DIRECTION//

				if (bKey[0] && holdW == false && direction != 'e') {
					direction1 = 'w';

				}

				else if (bKey[1] && holdN == false && direction != 's') {
					direction1 = 'n';

				}

				else if (bKey[2] && holdE == false && direction != 'w') {
					direction1 = 'e';

				}

				else if (bKey[3] && holdS == false && direction != 'n') {
					direction1 = 's';

				}

				

			}

			

			//COUNT FRAMES////////////////////

			currentFrame++;

			//REFRESH DISPLAY/////////////////////////

			for (int x = 0; x < 25; x++) {			//reset the game display

				for (int y = 0; y < 25; y++) {

					display[x][y] = 'z';
				}
			}

			//PASS MOVEMENT ON TO BODY SEGMENTS//

			for (int r = snekLength * snekLengthZ; r > 0; r--) {

				if (r > 1) {
					snekBody[r][0] = snekBody[r - 1][0];	//move all segments except the head and the following segment
					snekBody[r][1] = snekBody[r - 1][1];

				}

				else {
					snekBody[r][0] = snekHead[0];		//move the segment right before the head
					snekBody[r][1] = snekHead[1];

				}
			}

			//CHECK + SET DIRECTION//

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

			//PLACE SNEK BODY INTO DISPLAY ARRAY//

			for (int r = snekLength * snekLengthZ; r > 0; r--) {
				display[snekBody[r][0]][snekBody[r][1]] = '7';

			}

			//ADD STYLE POINTS//

			if (direction == 'e' && ((zKey) && display[snekHead[0] + 1][snekHead[1]] == '8' || display[snekHead[0] + 1][snekHead[1]] == 'X' || display[snekHead[0] + 1][snekHead[1]] == '7' && display[snekHead[0] + 2][snekHead[1]] == 'z' || display[snekHead[0] + 2][snekHead[1]] == '+')) {
				styleCounter++;
				styleCounter++;

			}	//E

			if (direction == 'e' && ((display[snekHead[0] + 1][snekHead[1] + 1] == '8' || display[snekHead[0] + 1][snekHead[1] + 1] == 'X' || display[snekHead[0] + 1][snekHead[1] + 1] == '7') && (display[snekHead[0] + 1][snekHead[1] - 1] == '8' || display[snekHead[0] + 1][snekHead[1] - 1] == 'X' || display[snekHead[0] + 1][snekHead[1] - 1] == '7') && (display[snekHead[0] + 1][snekHead[1]] != '8' && display[snekHead[0] + 1][snekHead[1]] != 'X' && display[snekHead[0] + 1][snekHead[1]] != '7'))) {
				styleCounter++;

			}	//E

			//////////////////////

			if (direction == 'w' && ((zKey) && display[snekHead[0] - 1][snekHead[1]] == '8' || display[snekHead[0] - 1][snekHead[1]] == 'X' || display[snekHead[0] - 1][snekHead[1]] == '7' && display[snekHead[0] - 2][snekHead[1]] == 'z' || display[snekHead[0] - 2][snekHead[1]] == '+')) {
				styleCounter++;
				styleCounter++;

			}	//W

			if (direction == 'w' && ((display[snekHead[0] - 1][snekHead[1] + 1] == '8' || display[snekHead[0] - 1][snekHead[1] + 1] == 'X' || display[snekHead[0] - 1][snekHead[1] + 1] == '7') && (display[snekHead[0] - 1][snekHead[1] - 1] == '8' || display[snekHead[0] - 1][snekHead[1] - 1] == 'X' || display[snekHead[0] - 1][snekHead[1] - 1] == '7') && (display[snekHead[0] - 1][snekHead[1]] != '8' && display[snekHead[0] - 1][snekHead[1]] != 'X' && display[snekHead[0] - 1][snekHead[1]] != '7'))) {
				styleCounter++;

			}	//W

			///////////////////////

			if (direction == 's' && ((zKey) && display[snekHead[0]][snekHead[1] + 1] == '8' || display[snekHead[0]][snekHead[1] + 1] == 'X' || display[snekHead[0]][snekHead[1] + 1] == '7' && display[snekHead[0]][snekHead[1] + 2] == 'z' || display[snekHead[0]][snekHead[1] + 2] == '+')) {
				styleCounter++;
				styleCounter++;

			}	//S

			if (direction == 's' && ((display[snekHead[0] + 1][snekHead[1] + 1] == '8' || display[snekHead[0] + 1][snekHead[1] + 1] == 'X' || display[snekHead[0] + 1][snekHead[1] + 1] == '7') && (display[snekHead[0] - 1][snekHead[1] + 1] == '8' || display[snekHead[0] - 1][snekHead[1] + 1] == 'X' || display[snekHead[0] - 1][snekHead[1] + 1] == '7') && (display[snekHead[0]][snekHead[1] + 1] != '8' && display[snekHead[0]][snekHead[1] + 1] != 'X' && display[snekHead[0]][snekHead[1] + 1] != '7'))) {
				styleCounter++;

			}	//S

			////////////////////////

			if (direction == 'n' && ((zKey) && display[snekHead[0]][snekHead[1] - 1] == '8' || display[snekHead[0]][snekHead[1] - 1] == 'X' || display[snekHead[0]][snekHead[1] - 1] == '7' && display[snekHead[0]][snekHead[1] - 2] == 'z' || display[snekHead[0]][snekHead[1] - 2] == '+')) {
				styleCounter++;
				styleCounter++;

			}	//N

			if (direction == 'n' && ((display[snekHead[0] + 1][snekHead[1] - 1] == '8' || display[snekHead[0] + 1][snekHead[1] - 1] == 'X' || display[snekHead[0] + 1][snekHead[1] - 1] == '7') && (display[snekHead[0] - 1][snekHead[1] - 1] == '8' || display[snekHead[0] - 1][snekHead[1] - 1] == 'X' || display[snekHead[0] - 1][snekHead[1] - 1] == '7') && (display[snekHead[0]][snekHead[1] - 1] != '8' && display[snekHead[0]][snekHead[1] - 1] != 'X' && display[snekHead[0]][snekHead[1] - 1] != '7'))) {
				styleCounter++;

			}	//N

			////////////add high style//

			if (styleCounter > styleHighScore) {
				styleHighScore++;

			}


			//MOVE PLAYER//

			if (direction == 'e') {
				snekHead[0]++;

				if ((zKey = (0x8000 & GetAsyncKeyState((unsigned char)("Z"[0]))) != 0) && snekLength > 10) {
					snekHead[0]++;

				}


			}

			else if (direction == 'w') {
				snekHead[0]--;

				if ((zKey = (0x8000 & GetAsyncKeyState((unsigned char)("Z"[0]))) != 0) && snekLength > 10) {
					snekHead[0]--;

				}


			}

			else if (direction == 's') {
				snekHead[1]++;


				if ((zKey = (0x8000 & GetAsyncKeyState((unsigned char)("Z"[0]))) != 0) && snekLength > 10) {
					snekHead[1]++;

				}


			}

			else if (direction == 'n') {
				snekHead[1]--;

				if ((zKey = (0x8000 & GetAsyncKeyState((unsigned char)("Z"[0]))) != 0) && snekLength > 10) {
					snekHead[1]--;

				}


			}

			//DETECT IF PLAYER HAS HIT MAP EDGE//

			if (snekHead[0] < 0 || snekHead[0] > 24 || snekHead[1] < 0 || snekHead[1] > 24) {
				gameLose = true;

				deathInstance->start();
				system->update();

				break;
						
			}

			//DETECT IF PLAYER HAS HIT THEMSELVES//

			if (display[snekHead[0]][snekHead[1]] == '7' || display[snekHead[0]][snekHead[1]] == 'X') {
				gameLose = true;

				deathInstance->start();
				system->update();

				break;

			}

			//DETECT IF PLAYER HAS HIT FRUIT/////

			if (snekHead[0] == currentFruit[0] && snekHead[1] == currentFruit[1]) {
				snekLength++;
				//snekMoveTimelinePosition += 200;
				
				
				if (snekLength == 11) {
					snakeFruitInstance11->start();	//FMOD
					//snakeFruitInstance11->setParameterByName("Delay Wet", 1.0f);
					//snakeFruitInstance11->setParameterByName("Pitch Shifter Send", 1.0f);
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

				

				if (snekLength > highScore) {		//set new high score
					
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

			else if ((zKey = (0x8000 & GetAsyncKeyState((unsigned char)("Z"[0]))) != 0) && snekLength > 10) {
				snakeLungeInstance->start();	//FMOD				
				snakeMoveInstance->setParameterByName("Reverb Wet", 1.0f);
				if (!wasZKeyHeld) {
					snekMoveTimelinePosition += 200;
					wasZKeyHeld = true;
				}
			}

			else {
						
				wasZKeyHeld = false;

				snakeMoveInstance->setTimelinePosition(snekMoveTimelinePosition);
				snakeMoveInstance->setParameterByName("Reverb Wet", snakeMoveReverbLevel);

				//FMOD::Studio:: snakeMoveInstancePlaybackState;
				//snakeMoveInstance->getPlaybackState(snakeMoveInstancePlaybackState);

				if (isScoreUnder11 || snakeMoveInstance->getPlaybackState(NULL) == FMOD_STUDIO_PLAYBACK_SUSTAINING || snakeMoveInstance->getPlaybackState(NULL) == FMOD_STUDIO_PLAYBACK_STOPPED){
					snakeMoveInstance->start();	//FMOD
					
				}

				snekMoveTimelinePosition += 200;

				if (snekMoveTimelinePosition >= snekMoveTimelinePositionMax) {
					snekMoveTimelinePosition = 0;

				}
			}

			//ADJUST PROXIMITY SOUND/////////////////////

			proximityToFruit = 1.0f - ((abs(snekHead[0] - currentFruit[0]) + abs(snekHead[1] - currentFruit[1])) / 48.0f);	//1/48 max distance
			
			snakeMoveInstance->setPitch(proximityToFruit);
			//proximitySoundInstance->setParameterByName("Proximity Highpass", proximityToFruit);


			//PLACE PLAYER INTO DISPLAY ARRAY////////////

			display[snekHead[0]][snekHead[1]] = '8';

			//PLACE FRUIT INTO DISPLAY ARRAY/////////////

			display[currentFruit[0]][currentFruit[1]] = '+';

			//DETERMINE TRAP LOCATIONS/////////////
/*
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
			//DRAW SCREEN/////////////////////////////


		

			for (int t = 0; t < 25; t++) {
				int q = 0;

				//cout << endl;

				for (q; q < 25; q++) {

					if (display[q][t] == 'z') {
						//cout << " ";
						screenString[q + (80 * t)] = ' ';

					}

					else if (display[q][t] == '7') {
						//cout << "8";
						screenString[q + (80 * t)] = '8';
					}

					else if (display[q][t] == '8' && direction == 'n') {
						//cout << "^";
						screenString[q + (80 * t)] = '^';
					}

					else if (display[q][t] == '8' && direction == 's') {
						//cout << "v";
						screenString[q + (80 * t)] = 'v';
					}

					else if (display[q][t] == '8' && direction == 'w') {
						//cout << "<";
						screenString[q + (80 * t)] = '<';
					}

					else if (display[q][t] == '8' && direction == 'e') {
						//cout << ">";
						screenString[q + (80 * t)] = '>';
					}

					else {
						//cout << display[q][t];
						screenString[q + (80 * t)] = display[q][t];
					}
				}

				//cout << "|";
				screenString[q + (80 * t)] = '|';

				if (t == 0 && q == 25) {
					//cout << "	 __    _    _              _  __   ____";
					screenString.replace(1 + q + (80 * t), 45, "       __    _    _              _  __   ____");
				}

				else if (t == 1 && q == 25) {
					//cout << "	/ /   | \\  | |     /\\     | |/ /  |  __|";
					screenString.replace(1 + q + (80 * t), 46, "      / /   | \\  | |     /\\     | |/ /  |  __|");
				}
				
				else if (t == 2 && q == 25) {
					//cout << "	\\ \\   |  \\ | |    /  \\    | | /   | |__";
					screenString.replace(1 + q + (80 * t), 45, "      \\ \\   |  \\ | |    /  \\    | | /   | |__");
				}

				else if (t == 3 && q == 25) {
					//cout << "	 \\ \\  | | \\| |   / /\\ \\   |   \\   |  __|";
					screenString.replace(1 + q + (80 * t), 46, "       \\ \\  | | \\| |   / /\\ \\   |   \\   |  __|");
				}

				else if (t == 4 && q == 25) {
					//cout << "	 / /  | |\\ \\ |  /  __  \\  | |\\ \\  | |__";
					screenString.replace(1 + q + (80 * t), 45, "       / /  | |\\ \\ |  /  __  \\  | |\\ \\  | |__");
				}

				else if (t == 5 && q == 25) {
					//cout << "       /_/   |_| \\__| /__/  \\__\\ |_| \\_\\ |____|";
					screenString.replace(1 + q + (80 * t), 46, "      /_/   |_| \\__| /__/  \\__\\ |_| \\_\\ |____|");
				}

				else if (t == 7 && q == 25) {
					//cout << "       SCORE: " << snekLength << "     HIGH SCORE: " << highScore;
					
					screenString.replace(1 + q + (80 * t), 14, "       SCORE: ");
					
					string snekLengthString = to_string(snekLength);
										
					screenString.replace(15 + q + (80 * t), snekLengthString.length(), snekLengthString);

					screenString.replace(15 + snekLengthString.length() + q + (80 * t), 17, "     HIGH SCORE: ");

					
					
					string highScoreString = to_string(highScore);

					screenString.replace(32 + snekLengthString.length() + q + (80 * t), highScoreString.length(), highScoreString);

				}

				else if (t == 9 && q == 25 && snekLength > 10) {
					//cout << "       STYLE: " << styleCounter << "     HIGH STYLE: " << styleHighScore;
					screenString.replace(1 + q + (80 * t), 14, "       STYLE: ");

					string styleCounterString = to_string(styleCounter);

					screenString.replace(15 + q + (80 * t), styleCounterString.length(), styleCounterString);

					screenString.replace(16 + styleCounterString.length() + q + (80 * t), 17, "     HIGH STYLE: ");

					string highStyleString = to_string(styleHighScore);

					screenString.replace(33 + styleCounterString.length() + q + (80 * t), highStyleString.length(), highStyleString);

				}

				else if (t == 17 && q == 25) {
					//cout << "	use arrow keys ^ v < > to control";
					screenString.replace(8 + q + (80 * t), 33, "use arrow keys ^ v < > to control");
				}

				/*else if (t == 19 && q == 25) {
					//cout << "	use arrow keys ^ v < > to control";
					screenString.replace(8 + q + (80 * t), 18, "Press [S] to pause");
				}
				*/

				else if (t == 21 && q == 25 && snekLength > 10) {
					//cout << "           use Z key to lunge";
					screenString.replace(14 + q + (80 * t), 18, "use Z key to lunge");
				}

				/*else if (t == 21 && q == 25 && snekLength <= 10) {
					//cout << "           use Z key to lunge";
					screenString.replace(12 + q + (80 * t), 18, "                  ");
				}*/

			}

			for (int u = 0; u < (nScreenHeight * nScreenWidth); u++) {
				screen[u] = screenString[u];

			}

			WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
			system->update(); //play FMOD system
		}

		this_thread::sleep_for(1347ms);
		
		//cout << "GAME OVER\n\nPlay again? (Y/N)\n";

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
					playAgain = 'y';
					
					fancyBossInstance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);
					system->update();
				}



				if (xKey = (0x8000 & GetAsyncKeyState((unsigned char)("X"[0]))) != 0) {
					gameOverMessage = false;
					playAgain = 'x';
				}

				this_thread::sleep_for(10ms);

				//cin >> playAgain;
			}
	}

	while (playAgain == 'y' || playAgain == 'Y');

	return 0;

}








/////////JUNKYARD//////////////



	/*for (int q = 0; q < 25; q++) {

		cout << q << " = " << display[q] << endl;

	}*/

	/*for (int q = 0; q < 25; q++) {

		display[q] = 0;

	}*/

	//display[snekHead[0]] = snekHead[1];



	//for (int q = 0; q < 1;) {

		//int z = 0;

	//*1 */ cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snekHead[0] == z) { cout << "="; } z++;
	//*2 */ cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snekHead[0] == z) { cout << "="; } z++;
	//*3 */ cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snekHead[0] == z) { cout << "="; } z++;
	//*4 */ cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snekHead[0] == z) { cout << "="; } z++;
	//*5 */ cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snekHead[0] == z) { cout << "="; } z++;
	//*6 */ cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snekHead[0] == z) { cout << "="; } z++;
	//*7 */ cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snekHead[0] == z) { cout << "="; } z++;
	//*8 */ cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snekHead[0] == z) { cout << "="; } z++;
	//*9 */ cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snekHead[0] == z) { cout << "="; } z++;
	//*10*/ cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snekHead[0] == z) { cout << "="; } z++;
	//*11*/ cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snekHead[0] == z) { cout << "="; } z++;
	//*12*/ cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snekHead[0] == z) { cout << "="; } z++;
	//*13*/ cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snekHead[0] == z) { cout << "="; } z++;
	//*14*/ cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snekHead[0] == z) { cout << "="; } z++;
	//*15*/ cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snekHead[0] == z) { cout << "="; } z++;
	//*16*/ cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snekHead[0] == z) { cout << "="; } z++;
	//*17*/ cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snekHead[0] == z) { cout << "="; } z++;
	//*18*/ cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snekHead[0] == z) { cout << "="; } z++;
	//*19*/ cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snekHead[0] == z) { cout << "="; } z++;
	//*20*/ cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snekHead[0] == z) { cout << "="; } z++;
	//*21*/ cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snekHead[0] == z) { cout << "="; } z++;
	//*22*/ cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snekHead[0] == z) { cout << "="; } z++;
	//*23*/ cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snekHead[0] == z) { cout << "="; } z++;
	//*24*/ cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snekHead[0] == z) { cout << "="; } z++;
	//*25*/ cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snekHead[0] == z) { cout << "="; } z++;

	//}









/*
if (snekHead[0] >= 25) {
	direction = 'w';

}

else if (snekHead[0] <= 1) {
	direction = 'e';

}
*/



/*

Sleep(frameRate);

*/


/*if (snekHead[0] - 1 < 0 && direction == 'w' || snekHead[0] + 1 > 25 && direction == 'e' || snekHead[1] - 1 < 0 && direction == 'n' || snekHead[1] + 1 > 25 && direction == 's') {
				gameLose = true;

			}*/


			//int frameRate = 250;			//measured in ms