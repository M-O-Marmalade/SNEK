#include "SnakeGame.h"

void SnakeGame::readScoreFile() {
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

		getline(scoreFileRead, highScoreName);

		scoreFileRead.close();

		wasPreviousHighScoreFound = true;
	}
	else {
		highScore = 0;
		highScoreName = " ";
	}
	oldHighScore = highScore;
}

SnakeGame::SnakeGame(int playerCount, int gridWidth, int gridHeight, ASCIIGraphics* asciiGraphics, ASCIIOutputCMD* asciiOutput, AudioSystem* audioSystem) : playerCount{ playerCount }, asciiGraphics { asciiGraphics }, asciiOutput{ asciiOutput }, snekAudioSystem{ audioSystem } {
	
	// initialize/resize `gameGrid`
	this->gameGrid = std::vector<std::vector<char>>(gridWidth,std::vector<char>(gridHeight, 'z'));

	// initialize `snakes` depending on amount of players
	this->snakes = std::vector<Snake>();
	this->snakes.push_back(Snake(playerCount == 1 ? 13 : 17, 12, 's'));
	this->snakes.push_back(Snake(7, 12, 's'));

	this->readScoreFile();
	
	fps = std::chrono::duration<long double, std::nano>(15000000000 / bpmValues[0]);			//reset the framerate

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
			gameGrid[x][y] = 'z';
		}
	}

	//Reset the Screen String Buffer//
	asciiGraphics->fillText(0, 0, asciiGraphics->width - 1, asciiGraphics->height - 1, ' ');

	//Reset FMOD-Related Audio/Sound Variables//
	snekAudioSystem->fmodEventInstances["Instruments+FX/SnakeMove"]->setParameterByName("SnakeMoveVolume", 1.0f);
	snekAudioSystem->fmodEventInstances["Instruments+FX/Snare1"]->setParameterByName("SnareReverb", 0.0f);
	snekAudioSystem->fmodEventInstances["Instruments+FX/arp"]->setParameterByName("ArpVolume", 0.0f);
	snekAudioSystem->fmodEventInstances["Instruments+FX/Triangle"]->setParameterByName("TriangleDecay", 1.0f);
	snekAudioSystem->fmodEventInstances["Instruments+FX/SnakeFruit"]->setPitch(1.0f);
	snekAudioSystem->fmodEventInstances["Instruments+FX/SnakeFruit"]->setVolume(0.7f);
	snekAudioSystem->fmodResult = snekAudioSystem->fmodSystem->setParameterByName("ChordsSelection", 0.0f);
	snekAudioSystem->fmodResult = snekAudioSystem->fmodSystem->setParameterByName("ChordsReverb", 0.0f);
	//result = system->setVolume(1.0f);
}

void SnakeGame::play() {


	frameTime = std::chrono::steady_clock::now();	//record start time of first frame of the game loop

	snekAudioSystem->startEventInstance(bpmNames[0]);

	//				     //
	// [GAME LOOP START] //
	//				   //
	while (gameLose == false) {

		if (playerCount == 2 && currentFrame == 1) {
			frameTime = std::chrono::steady_clock::now();
		}

		//			     //
		// SET FRAMERATE //
		//			   //
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

		//			  //
		// TICK CLOCK //
		//			//
		while (std::chrono::steady_clock::now() < frameTime + fps) {

			//				     //
			// READ PLAYER INPUT //
			//				   //
			for (int k = 0; k < 4; k++) {	//player 1
				snakes[0].directional_keys[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x25\x26\x27\x28"[k]))) != 0;

			}

			for (int k = 0; k < 4; k++) {	//player 2
				snakes[1].directional_keys[k] = (0x8000 & GetAsyncKeyState((unsigned char)("AWDS"[k]))) != 0;

			}

			if (highestCurrentLength > 10) {
				if (playerCount == 1) {
					snakes[0].action_keys = ((0x8000 & GetAsyncKeyState((unsigned char)("Z"[0]))) != 0);
				}
				else {
					snakes[0].action_keys = ((0x8000 & GetAsyncKeyState((unsigned char)("P"[0]))) != 0);
					snakes[1].action_keys = ((0x8000 & GetAsyncKeyState((unsigned char)("V"[0]))) != 0);
				}

			}
			else {
				snakes[0].action_keys = false;
				snakes[1].action_keys = false;
			}


			//										   //
			// CHECK + SET DIRECTION [TICK RESOLUTION] //
			//										 //

			for (int pt = 0; pt < playerCount; pt++) {

				if (snakes[pt].directional_keys[0] && snakes[pt].holdW == false && snakes[pt].direction_frame != 'e') {
					snakes[pt].direction_tick = 'w';
				}

				else if (snakes[pt].directional_keys[1] && snakes[pt].holdN == false && snakes[pt].direction_frame != 's') {
					snakes[pt].direction_tick = 'n';
				}

				else if (snakes[pt].directional_keys[2] && snakes[pt].holdE == false && snakes[pt].direction_frame != 'w') {
					snakes[pt].direction_tick = 'e';
				}

				else if (snakes[pt].directional_keys[3] && snakes[pt].holdS == false && snakes[pt].direction_frame != 'n') {
					snakes[pt].direction_tick = 's';
				}
			}
		}

		currentFrame++;
		frameTime += std::chrono::duration_cast<std::chrono::nanoseconds>(fps);

		//				   //
		// REFRESH DISPLAY //
		//				 //
		for (int x = 0; x < 25; x++) {

			for (int y = 0; y < 25; y++) {

				gameGrid[x][y] = 'z';
			}
		}

		//					  //
		// MOVE BODY SEGMENTS //
		//					//
		for (int pt = 0; pt < playerCount; pt++) {

			for (int r = snakes[pt].snek_length - 1; r >= 0; r--) {

				if (r > 0) {
					snakes[pt].snek_body[r][0] = snakes[pt].snek_body[r - 1][0];	//move all segments except the segment right before the head
					snakes[pt].snek_body[r][1] = snakes[pt].snek_body[r - 1][1];

				}

				else {
					snakes[pt].snek_body[r][0] = snakes[pt].head[0];		//move the segment right before the head
					snakes[pt].snek_body[r][1] = snakes[pt].head[1];

				}
			}

		}

		//										    //
		// CHECK + SET DIRECTION [FRAME RESOLUTION] //
		//										  //
		for (int pt = 0; pt < playerCount; pt++) {

			if (snakes[pt].direction_tick == 'w' && snakes[pt].holdW == false && snakes[pt].direction_frame != 'e') {
				snakes[pt].direction_frame = 'w';

				snakes[pt].holdW = true;
				snakes[pt].holdE = false;
				snakes[pt].holdS = false;
				snakes[pt].holdN = false;
			}

			else if (snakes[pt].direction_tick == 'n' && snakes[pt].holdN == false && snakes[pt].direction_frame != 's') {
				snakes[pt].direction_frame = 'n';

				snakes[pt].holdN = true;
				snakes[pt].holdE = false;
				snakes[pt].holdS = false;
				snakes[pt].holdW = false;
			}

			else if (snakes[pt].direction_tick == 'e' && snakes[pt].holdE == false && snakes[pt].direction_frame != 'w') {
				snakes[pt].direction_frame = 'e';

				snakes[pt].holdE = true;
				snakes[pt].holdW = false;
				snakes[pt].holdS = false;
				snakes[pt].holdN = false;
			}

			else if (snakes[pt].direction_tick == 's' && snakes[pt].holdS == false && snakes[pt].direction_frame != 'n') {
				snakes[pt].direction_frame = 's';

				snakes[pt].holdS = true;
				snakes[pt].holdE = false;
				snakes[pt].holdW = false;
				snakes[pt].holdN = false;
			}
		}

		//								      //
		// PLACE SNEK BODY INTO DISPLAY ARRAY //
		//									//
		for (int pt = 0; pt < playerCount; pt++) {

			for (int r = snakes[pt].snek_length - 1; r >= 0; r--) {
				gameGrid[snakes[pt].snek_body[r][0]][snakes[pt].snek_body[r][1]] = '7';
			}
		}

		//								  //
		// STORE EACH SNEK'S SURROUNDINGS //
		//								//
		//for (int i = 0; i < playerCount; i++) {

		//	//North
		//	if (snakes[i].head[1] > 0 && gameGrid[snakes[i].head[0]][snakes[i].head[1] - 1] == 'z' || gameGrid[snakes[i].head[0]][snakes[i].head[1] - 1] == '+') {
		//		snakes[i].surroundingObstacles[0] = false;
		//	}
		//	else {
		//		snakes[i].surroundingObstacles[0] = true;
		//	}

		//	//North-East
		//	if (snakes[i].head[0] < 24 && snakes[i].head[1] > 0 && gameGrid[snakes[i].head[0] + 1][snakes[i].head[1] - 1] == 'z' || gameGrid[snakes[i].head[0] + 1][snakes[i].head[1] - 1] == '+') {
		//		snakes[i].surroundingObstacles[1] = false;
		//	}
		//	else {
		//		snakes[i].surroundingObstacles[1] = true;
		//	}

		//	//East
		//	if (snakes[i].head[0] < 24 && gameGrid[snakes[i].head[0] + 1][snakes[i].head[1]] == 'z' || gameGrid[snakes[i].head[0] + 1][snakes[i].head[1]] == '+') {
		//		snakes[i].surroundingObstacles[2] = false;
		//	}
		//	else {
		//		snakes[i].surroundingObstacles[2] = true;
		//	}

		//	//South-East
		//	if (snakes[i].head[0] < 24 && snakes[i].head[1] < 24 && gameGrid[snakes[i].head[0] + 1][snakes[i].head[1] + 1] == 'z' || gameGrid[snakes[i].head[0] + 1][snakes[i].head[1] + 1] == '+') {
		//		snakes[i].surroundingObstacles[3] = false;
		//	}
		//	else {
		//		snakes[i].surroundingObstacles[3] = true;
		//	}

		//	//South
		//	if (snakes[i].head[1] < 24 && gameGrid[snakes[i].head[0]][snakes[i].head[1] + 1] == 'z' || gameGrid[snakes[i].head[0]][snakes[i].head[1] + 1] == '+') {
		//		snakes[i].surroundingObstacles[4] = false;
		//	}
		//	else {
		//		snakes[i].surroundingObstacles[4] = true;
		//	}

		//	//South-West
		//	if (snakes[i].head[0] > 0 && snakes[i].head[1] < 24 && gameGrid[snakes[i].head[0] - 1][snakes[i].head[1] + 1] == 'z' || gameGrid[snakes[i].head[0] - 1][snakes[i].head[1] + 1] == '+') {
		//		snakes[i].surroundingObstacles[5] = false;
		//	}
		//	else {
		//		snakes[i].surroundingObstacles[5] = true;
		//	}

		//	//West
		//	if (snakes[i].head[0] > 0 && gameGrid[snakes[i].head[0] - 1][snakes[i].head[1]] == 'z' || gameGrid[snakes[i].head[0] - 1][snakes[i].head[1]] == '+') {
		//		snakes[i].surroundingObstacles[6] = false;
		//	}
		//	else {
		//		snakes[i].surroundingObstacles[6] = true;
		//	}

		//	//North-West
		//	if (snakes[i].head[0] > 0 && snakes[i].head[1] > 0 && gameGrid[snakes[i].head[0] - 1][snakes[i].head[1] - 1] == 'z' || gameGrid[snakes[i].head[0] - 1][snakes[i].head[1] - 1] == '+') {
		//		snakes[i].surroundingObstacles[7] = false;
		//	}
		//	else {
		//		snakes[i].surroundingObstacles[7] = true;
		//	}
		//}

		////				    //
		//// ADD STYLE POINTS //
		////				  //
		//for (int pt = 0; pt < playerCount; pt++) {

		//	//EAST//
		//	if (snakes[pt].direction_frame == 'e') {

		//		//if player jumps over an obstacle to the East, add two style points
		//		if (snakes[pt].action_keys && snakes[pt].surroundingObstacles[2] && snakes[pt].head[0] + 2 < 25) {
		//			if (gameGrid[snakes[pt].head[0] + 2][snakes[pt].head[1]] == 'z' || gameGrid[snakes[pt].head[0] + 2][snakes[pt].head[1]] == '+') {
		//				styleCounter += 2;
		//			}
		//		}

		//		//if player goes in-between two obstacles to the North-East and South-East, add one style point
		//		else if (snakes[pt].surroundingObstacles[1] && snakes[pt].surroundingObstacles[3] && !snakes[pt].surroundingObstacles[2] && snakes[pt].head[0] + 1 < 25) {
		//			styleCounter++;
		//		}
		//	}

		//	//WEST//
		//	if (snakes[pt].direction_frame == 'w') {

		//		//if player jumps over an obstacle to the West, add two style points
		//		if (snakes[pt].action_keys && snakes[pt].surroundingObstacles[6] && snakes[pt].head[0] - 2 > 0) {
		//			if (gameGrid[snakes[pt].head[0] - 2][snakes[pt].head[1]] == 'z' || gameGrid[snakes[pt].head[0] - 2][snakes[pt].head[1]] == '+') {
		//				styleCounter += 2;
		//			}
		//		}

		//		//if player goes in-between two obstacles to the North-West and South-West, add one style point
		//		else if (snakes[pt].surroundingObstacles[7] && snakes[pt].surroundingObstacles[5] && !snakes[pt].surroundingObstacles[6] && snakes[pt].head[0] - 1 > 0) {
		//			styleCounter++;
		//		}
		//	}

		//	//NORTH//
		//	if (snakes[pt].direction_frame == 'n') {

		//		//if player jumps over an obstacle to the North, add two style points
		//		if (snakes[pt].action_keys && snakes[pt].surroundingObstacles[0] && snakes[pt].head[1] - 2 > 0) {
		//			if (gameGrid[snakes[pt].head[0]][snakes[pt].head[1] - 2] == 'z' || gameGrid[snakes[pt].head[0]][snakes[pt].head[1] - 2] == '+') {
		//				styleCounter += 2;
		//			}
		//		}

		//		//if player goes in-between two obstacles to the North-West and North-East, add one style point
		//		else if (snakes[pt].surroundingObstacles[7] && snakes[pt].surroundingObstacles[1] && !snakes[pt].surroundingObstacles[0] && snakes[pt].head[1] - 1 > 0) {
		//			styleCounter++;
		//		}
		//	}

		//	//SOUTH//
		//	if (snakes[pt].direction_frame == 's') {

		//		//if player jumps over an obstacle to the South, add two style points
		//		if (snakes[pt].action_keys && snakes[pt].surroundingObstacles[4] && snakes[pt].head[1] + 2 < 25) {
		//			if (gameGrid[snakes[pt].head[0]][snakes[pt].head[1] + 2] == 'z' || gameGrid[snakes[pt].head[0]][snakes[pt].head[1] + 2] == '+') {
		//				styleCounter += 2;
		//			}
		//		}

		//		//if player goes in-between two obstacles to the South-West and South-East, add one style point
		//		else if (snakes[pt].surroundingObstacles[5] && snakes[pt].surroundingObstacles[3] && !snakes[pt].surroundingObstacles[4] && snakes[pt].head[1] + 1 < 25) {
		//			styleCounter++;
		//		}
		//	}
		//}

		////						   //
		//// UPDATE STYLE HIGH SCORE //
		////						 //
		//if (styleCounter > styleHighScore && !gameLose) {
		//	styleHighScore++;
		//}

		//			   //
		// MOVE PLAYER //
		//			 //
		for (int pt = 0; pt < playerCount; pt++) {

			if (snakes[pt].direction_frame == 'e') {
				snakes[pt].head[0]++;

				if (snakes[pt].action_keys) {
					snakes[pt].head[0]++;
				}
			}

			else if (snakes[pt].direction_frame == 'w') {
				snakes[pt].head[0]--;

				if (snakes[pt].action_keys) {
					snakes[pt].head[0]--;
				}
			}

			else if (snakes[pt].direction_frame == 's') {
				snakes[pt].head[1]++;

				if (snakes[pt].action_keys) {
					snakes[pt].head[1]++;
				}
			}

			else if (snakes[pt].direction_frame == 'n') {
				snakes[pt].head[1]--;

				if (snakes[pt].action_keys) {
					snakes[pt].head[1]--;
				}
			}
		}

		//								     //
		// DETECT IF PLAYER HAS HIT MAP EDGE //
		//								   //
		for (int pt = 0; pt < playerCount; pt++) {
			if (snakes[pt].head[0] < 0 || snakes[pt].head[0] > 24 || snakes[pt].head[1] < 0 || snakes[pt].head[1] > 24) {
				snakes[pt].justDied = true;
				gameLose = true;

				if (snakes[pt].head[0] > 24) {
					snakes[pt].head[0] = 24;
				}
				else if (snakes[pt].head[0] < 0) {
					snakes[pt].head[0] = 0;
				}
				else if (snakes[pt].head[1] > 24) {
					snakes[pt].head[1] = 24;
				}
				else if (snakes[pt].head[1] < 0) {
					snakes[pt].head[1] = 0;
				}
			}
		}


		//									   //
		// DETECT IF PLAYER HAS HIT THEMSELVES //
		//									 //
		for (int pt = 0; pt < playerCount; pt++) {
			if (gameGrid[snakes[pt].head[0]][snakes[pt].head[1]] == '7' || gameGrid[snakes[pt].head[0]][snakes[pt].head[1]] == 'X' || gameGrid[snakes[pt].head[0]][snakes[pt].head[1]] == '8') {
				snakes[pt].justDied = true;
				gameLose = true;
			}
		}

		//								    //
		// CALCULATE PROXIMITY TO THE FRUIT //
		//								  //
		for (int pt = 0; pt < playerCount; pt++) {	//calculate each player's individual proximity
			snakes[pt].iProximityToFruit = 1.0f - ((abs(snakes[pt].head[0] - currentFruit[0]) + abs(snakes[pt].head[1] - currentFruit[1])) / 48.0f);	//1/48 max distance
		}
		if (playerCount > 1 && snakes[1].iProximityToFruit > snakes[0].iProximityToFruit) {	//use whichever one is closer
			proximityToFruit = snakes[1].iProximityToFruit;
		}
		else {
			proximityToFruit = snakes[0].iProximityToFruit;	//if there is only one player, then 
		}


		//								  //
		// DETECT IF PLAYER HAS HIT FRUIT //
		//								//
		gotNewFruit = false;

		for (int pt = 0; pt < playerCount; pt++) {

			if (snakes[pt].head[0] == currentFruit[0] && snakes[pt].head[1] == currentFruit[1]) {

				snakes[pt].snek_length++;
				snakes[pt].justGotNewFruit = true;
				gotNewFruit = true;
				snakes[pt].snekSwallowTimer = 0;

				for (int e = 0; e < 300; e++) {
					currentFruit[0] = rand() % 25;		//create a potential fruit spot
					currentFruit[1] = rand() % 25;

					//check if that spot is currently filled//
					if ((currentFruit[0] != snakes[pt].head[0] && currentFruit[1] != snakes[pt].head[1]) && gameGrid[currentFruit[0]][currentFruit[1]] != '7' && gameGrid[currentFruit[0]][currentFruit[1]] != 'X' && gameGrid[currentFruit[0]][currentFruit[1]] != 'p') {

						//a temp proximity to use in the for loop for the new fruit//
						int proximityToFruitTemp = (abs(snakes[pt].head[0] - currentFruit[0]) + abs(snakes[pt].head[1] - currentFruit[1]));

						//calculate snek's potential spots to be on the beat
						if (i16thNote == 1) {
							snakes[pt].potentialFruitSpot1 = 17;
						}
						else {
							snakes[pt].potentialFruitSpot1 = 17 - i16thNote;
						}

						if (i16thNote >= 5) {
							snakes[pt].potentialFruitSpot2 = 16 - (i16thNote - 5);
						}
						else {
							snakes[pt].potentialFruitSpot2 = 5 - i16thNote;
						}

						if (i16thNote >= 13) {
							snakes[pt].potentialFruitSpot3 = 16 - (i16thNote - 13);
						}
						else {
							snakes[pt].potentialFruitSpot3 = 13 - i16thNote;
						}

						//accept new fruit position if it lands the appropriate distance from the snek who just got the last fruit//
						if (proximityToFruitTemp % 16 == snakes[pt].potentialFruitSpot1 || proximityToFruitTemp % 16 == snakes[pt].potentialFruitSpot2 || proximityToFruitTemp % 16 == snakes[pt].potentialFruitSpot3) {
							break;
						}
					}
				}

				if (snakes[pt].snek_length == 11) {
					isScoreUnder11 = false;
				}

				//					  //
				// SET NEW HIGH SCORE //
				//					//
				if (snakes[pt].snek_length > highestCurrentLength) {		//update current highest length
					highestCurrentLength = snakes[pt].snek_length;

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

		//				  //
		// CREATE PORTALS //
		//				//
		if (highestCurrentLength == 14 && portalCount < 1) {
			portalCount++;

			bool portalsFarEnoughApart = false;
			while (!portalsFarEnoughApart) {

				for (int e = 0; e == 0;) {
					portalCoordinates[0][0] = (rand() % 21) + 2;
					portalCoordinates[0][1] = (rand() % 21) + 2;

					if (portalCoordinates[0][0] != snakes[0].head[0] && portalCoordinates[0][1] != snakes[0].head[1]) {
						if (gameGrid[portalCoordinates[0][0]][portalCoordinates[0][1]] != '7' && gameGrid[portalCoordinates[0][0]][portalCoordinates[0][1]] != 'X' && gameGrid[portalCoordinates[0][0]][portalCoordinates[0][1]] != 'o' && gameGrid[portalCoordinates[0][0]][portalCoordinates[0][1]] != 'p') {
							e = 1;
						}
					}
				}

				for (int e = 0; e == 0;) {
					portalCoordinates[1][0] = (rand() % 21) + 2;
					portalCoordinates[1][1] = (rand() % 21) + 2;

					if (portalCoordinates[1][0] != snakes[0].head[0] && portalCoordinates[1][1] != snakes[0].head[1]) {
						if (gameGrid[portalCoordinates[1][0]][portalCoordinates[1][1]] != '7' && gameGrid[portalCoordinates[1][0]][portalCoordinates[1][1]] != 'X' && gameGrid[portalCoordinates[1][0]][portalCoordinates[1][1]] != 'o' && gameGrid[portalCoordinates[1][0]][portalCoordinates[1][1]] != 'p') {
							e = 1;
						}
					}
				}

				if (abs(portalCoordinates[0][0] - portalCoordinates[1][0]) + abs(portalCoordinates[0][1] - portalCoordinates[1][1]) > 14) {
					portalsFarEnoughApart = true;
				}
			}

		}

		//							   //
		// PASS PLAYER THROUGH PORTALS //
		//							 //
		for (int pt = 0; pt < playerCount; pt++) {
			for (int hh = 0; hh < portalCount * 2; hh++) {
				if (snakes[pt].head[0] == portalCoordinates[hh][0] && snakes[pt].head[1] == portalCoordinates[hh][1]) {
					snakes[pt].head[0] = portalCoordinates[((hh + 1) % 2)][0];
					snakes[pt].head[1] = portalCoordinates[((hh + 1) % 2)][1];
					break;
				}
			}
		}

		//								   //
		// PLACE PLAYER INTO DISPLAY ARRAY //
		//								 //
		for (int pt = 0; pt < playerCount; pt++) {
			gameGrid[snakes[pt].head[0]][snakes[pt].head[1]] = '8';
		}

		//								  //
		// PLACE FRUIT INTO DISPLAY ARRAY //
		//								//
		gameGrid[currentFruit[0]][currentFruit[1]] = '+';


		//				    //
		// AUDIO PROCESSING //
		//				  //		

		if (gameLose) {					//play the death sound if the game is lost
			snekAudioSystem->stopEventInstance(bpmNames[currentChordBPM], true);
			snekAudioSystem->startEventInstance("Instruments+FX/Death");
		}

		if (gotNewFruit) {				//if someone gets a new fruit..
			for (int pt = 0; pt < playerCount; pt++) {		//..check each player..
				if (snakes[pt].justGotNewFruit) {			//..to see if they were the one who got the new fruit..					
					if (gotNewHighScoreSoundPlay) {
						snekAudioSystem->startEventInstance("Instruments+FX/newHighScore");
						gotNewHighScoreSoundPlay = false;
					}
					else if (snakes[pt].snek_length == 11) {		//..if they did get a fruit, see if they just got their 11th fruit..
						snekAudioSystem->startEventInstance("Instruments+FX/SnakeFruit11");		//..if they did, then play the 11th fruit sound..
					}
					else if (gotNewFruit && (i16thNote == 3 || i16thNote == 7 || i16thNote == 11 || i16thNote == 15)) {
						snekAudioSystem->startEventInstance("Instruments+FX/Triangle");			//if they got a fruit on an offbeat, play triangle sound
					}
					else if (i16thNote == 5 || i16thNote == 13) {
						snekAudioSystem->startEventInstance("Instruments+FX/Snare1");			//if they got a fruit on an offbeat, play triangle sound
					}
					else if (i16thNote == 1) {
						snekAudioSystem->startEventInstance("Instruments+FX/KickTight");
						snekAudioSystem->startEventInstance("Instruments+FX/Triangle");
						snekAudioSystem->startEventInstance("Instruments+FX/SnakeFruit");
					}
					else {	//..otherwise, play the default fruit eating sound
						snekAudioSystem->startEventInstance("Instruments+FX/SnakeFruit");
					}
				}
			}

			switch (highestCurrentLength) {					//update reverb level and max timeline position from the current highest length
			case 1:
				snekAudioSystem->stopEventInstance(bpmNames[0], true);
				chordsStartToggle = true;
				currentChordBPM = 1;
				if (switchChordsCounter == 0) {
					switchChordsCounter = 1;
				}
				break;

			case 7:
				snekAudioSystem->fmodEventInstances["Instruments+FX/arp"]->setParameterByName("ArpVolume", 0.09f);
				snekAudioSystem->fmodResult = snekAudioSystem->fmodSystem->setParameterByName("ChordsReverb", 0.7f);
				currentChordBPM = 2;
				if (switchChordsCounter == 1) {
					switchChords = true;
					switchChordsCounter = 7;
				}
				break;

			case 11:
				snekMoveTimelinePositionMax += 200;
				snakeMoveReverbLevel = 0.125f;
				snekAudioSystem->fmodEventInstances["Instruments+FX/Snare1"]->setParameterByName("SnareReverb", 0.2f);
				snekAudioSystem->fmodEventInstances["Instruments+FX/arp"]->setParameterByName("ArpVolume", 0.14f);
				snekAudioSystem->fmodEventInstances["Instruments+FX/SnakeMove"]->setParameterByName("SnakeMoveVolume", 1.0f);
				snekAudioSystem->fmodEventInstances["Instruments+FX/Triangle"]->setParameterByName("TriangleDecay", 0.9f);
				snekAudioSystem->fmodResult = snekAudioSystem->fmodSystem->setParameterByName("ChordsReverb", 1.0f);
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
				snekAudioSystem->fmodEventInstances["Instruments+FX/Snare1"]->setParameterByName("SnareReverb", 0.4f);
				snekAudioSystem->fmodEventInstances["Instruments+FX/arp"]->setParameterByName("ArpVolume", 0.17f);
				snekAudioSystem->fmodEventInstances["Instruments+FX/SnakeMove"]->setParameterByName("SnakeMoveVolume", 0.9f);
				snekAudioSystem->fmodEventInstances["Instruments+FX/Triangle"]->setParameterByName("TriangleDecay", 0.8f);
				currentChordBPM = 4;
				if (switchChordsCounter == 11) {
					switchChords = true;
					switchChordsCounter = 20;
				}
				break;

			case 30:
				snekMoveTimelinePositionMax += 200;
				snakeMoveReverbLevel = 0.375f;
				snekAudioSystem->fmodEventInstances["Instruments+FX/Snare1"]->setParameterByName("SnareReverb", 0.5f);
				snekAudioSystem->fmodEventInstances["Instruments+FX/arp"]->setParameterByName("ArpVolume", 0.2f);
				snekAudioSystem->fmodEventInstances["Instruments+FX/SnakeMove"]->setParameterByName("SnakeMoveVolume", 0.88f);
				snekAudioSystem->fmodEventInstances["Instruments+FX/Triangle"]->setParameterByName("TriangleDecay", 0.5f);
				snekAudioSystem->fmodResult = snekAudioSystem->fmodSystem->setParameterByName("ChordsReverb", 1.0f);
				currentChordBPM = 5;
				if (switchChordsCounter == 20) {
					switchChords = true;
					switchChordsCounter = 30;
				}
				break;

			case 40:
				snekMoveTimelinePositionMax += 200;
				snakeMoveReverbLevel = 0.5f;
				snekAudioSystem->fmodEventInstances["Instruments+FX/Snare1"]->setParameterByName("SnareReverb", 0.64f);
				snekAudioSystem->fmodEventInstances["Instruments+FX/arp"]->setParameterByName("ArpVolume", 0.3f);
				snekAudioSystem->fmodEventInstances["Instruments+FX/SnakeMove"]->setParameterByName("SnakeMoveVolume", 0.4f);
				snekAudioSystem->fmodEventInstances["Instruments+FX/Triangle"]->setParameterByName("TriangleDecay", 0.25f);
				snekAudioSystem->fmodResult = snekAudioSystem->fmodSystem->setParameterByName("ChordsReverb", 0.7f);
				currentChordBPM = 6;
				if (switchChordsCounter == 30) {
					switchChords = true;
					switchChordsCounter = 40;
				}
				break;

			case 50:
				snekMoveTimelinePositionMax += 200;
				snakeMoveReverbLevel = 0.625f;
				snekAudioSystem->fmodEventInstances["Instruments+FX/Snare1"]->setParameterByName("SnareReverb", 0.72f);
				snekAudioSystem->fmodEventInstances["Instruments+FX/arp"]->setParameterByName("ArpVolume", 0.45f);
				snekAudioSystem->fmodEventInstances["Instruments+FX/SnakeMove"]->setParameterByName("SnakeMoveVolume", 0.2f);
				snekAudioSystem->fmodEventInstances["Instruments+FX/Triangle"]->setParameterByName("TriangleDecay", 0.15f);
				snekAudioSystem->fmodResult = snekAudioSystem->fmodSystem->setParameterByName("ChordsReverb", 0.3f);
				currentChordBPM = 7;
				if (switchChordsCounter == 40) {
					switchChords = true;
					switchChordsCounter = 50;
				}
				break;

			case 60:
				snekMoveTimelinePositionMax += 200;
				snakeMoveReverbLevel = 0.750f;
				snekAudioSystem->fmodEventInstances["Instruments+FX/Snare1"]->setParameterByName("SnareReverb", 0.8f);
				snekAudioSystem->fmodEventInstances["Instruments+FX/arp"]->setParameterByName("ArpVolume", 0.45f);
				snekAudioSystem->fmodEventInstances["Instruments+FX/SnakeMove"]->setParameterByName("SnakeMoveVolume", 0.4f);
				snekAudioSystem->fmodEventInstances["Instruments+FX/Triangle"]->setParameterByName("TriangleDecay", 0.1f);
				snekAudioSystem->fmodResult = snekAudioSystem->fmodSystem->setParameterByName("ChordsReverb", 0.0f);
				currentChordBPM = 8;
				if (switchChordsCounter == 50) {
					switchChords = true;
					switchChordsCounter = 60;
				}
				break;

			case 70:
				snekMoveTimelinePositionMax += 200;
				snakeMoveReverbLevel = 0.875f;
				snekAudioSystem->fmodEventInstances["Instruments+FX/Snare1"]->setParameterByName("SnareReverb", 0.4f);
				snekAudioSystem->fmodEventInstances["Instruments+FX/arp"]->setParameterByName("ArpVolume", 0.35f);
				snekAudioSystem->fmodEventInstances["Instruments+FX/SnakeMove"]->setParameterByName("SnakeMoveVolume", 0.6f);
				snekAudioSystem->fmodEventInstances["Instruments+FX/Triangle"]->setParameterByName("TriangleDecay", 0.05f);
				currentChordBPM = 9;
				if (switchChordsCounter == 60) {
					switchChords = true;
					switchChordsCounter = 70;
				}
				break;

			case 80:
				snekMoveTimelinePositionMax += 200;
				snakeMoveReverbLevel = 1.0f;
				snekAudioSystem->fmodEventInstances["Instruments+FX/Snare1"]->setParameterByName("SnareReverb", 0.2f);
				snekAudioSystem->fmodEventInstances["Instruments+FX/arp"]->setParameterByName("ArpVolume", 0.20f);
				snekAudioSystem->fmodEventInstances["Instruments+FX/Triangle"]->setParameterByName("TriangleDecay", 0.05f);
				currentChordBPM = 10;
				if (switchChordsCounter == 70) {
					switchChords = true;
					switchChordsCounter = 80;
				}
				break;

			case 90:
				currentChordBPM = 11;
				snekAudioSystem->fmodEventInstances["Instruments+FX/arp"]->setParameterByName("ArpVolume", 0.0f);
				snekAudioSystem->fmodEventInstances["Instruments+FX/Snare1"]->setParameterByName("SnareReverb", 0.0f);
				if (switchChordsCounter == 80) {
					switchChords = true;
					switchChordsCounter = 90;
				}
				break;
			}
		}
		//if nobody got any fruits, then check if either snake is using the action keys..
		else if (snakes[0].action_keys || snakes[1].action_keys) {
			snekAudioSystem->fmodEventInstances["Instruments+FX/SnakeLunge"]->setPitch(proximityToFruit);
			snekAudioSystem->startEventInstance("Instruments+FX/SnakeLunge");
			snekAudioSystem->fmodEventInstances["Instruments+FX/SnakeMove"]->setParameterByName("Reverb Wet", 1.0f);		//set the reverb level high for the move sound
			if (!actionKeyHeld) {
				snekMoveTimelinePosition = (200 + snekMoveTimelinePositionMax);
				actionKeyHeld = true;
			}
		}

		//if nobody got a fruit, and nobody is holding any action keys, then..
		else {

			if (highestCurrentLength == 0) {
				snekAudioSystem->fmodEventInstances[bpmNames[0]]->setParameterByName("HeartRateDryLevel", proximityToFruit);
			}

			actionKeyHeld = false;		//nobody is holding any action keys anymore

			snekAudioSystem->fmodEventInstances["Instruments+FX/SnakeMove"]->setPitch(proximityToFruit);
			snekAudioSystem->fmodEventInstances["Instruments+FX/SnakeMove"]->setTimelinePosition(snekMoveTimelinePosition);
			snekAudioSystem->fmodEventInstances["Instruments+FX/SnakeMove"]->setParameterByName("Reverb Wet", snakeMoveReverbLevel);
			snekAudioSystem->startEventInstance("Instruments+FX/SnakeMove");


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
				snekAudioSystem->startEventInstance(bpmNames[1]);
				i16thNote = 1;
				currentChord = 1;
				hasFirstSwitchHappened = true;
			}

			if (switchChordsCounter > 6 && currentChordBPM > 0) {

				if (switchChords == true) {
					int oldPlaybackPosition;
					int newPlaybackPosition;
					snekAudioSystem->fmodEventInstances[bpmNames[currentChordBPM - 1]]->getTimelinePosition(&oldPlaybackPosition);
					snekAudioSystem->stopEventInstance(bpmNames[currentChordBPM - 1]);

					newPlaybackPosition = (oldPlaybackPosition / (60000.0f / bpmValues[currentChordBPM - 1])) * (60000.0f / bpmValues[currentChordBPM]);

					snekAudioSystem->startEventInstance(bpmNames[currentChordBPM]);
					snekAudioSystem->fmodEventInstances[bpmNames[currentChordBPM]]->setTimelinePosition(newPlaybackPosition);
				}

				switchChords = false;
			}

			if (currentChord == 1 && i16thNote == 1) {
				snekAudioSystem->startEventInstance(bpmNames[currentChordBPM]);
			}

			//ARP//
			switch (currentChord) {
			case 1:
				switch (i16thNote) {
				case 1:
					snekAudioSystem->startEventInstance("Instruments+FX/arp");
					break;
				case 3:
					snekAudioSystem->fmodEventInstances["Instruments+FX/arp"]->setTimelinePosition(170);
					break;
				case 5:
					snekAudioSystem->fmodEventInstances["Instruments+FX/arp"]->setTimelinePosition(338);
					break;
				case 7:
					snekAudioSystem->fmodEventInstances["Instruments+FX/arp"]->setTimelinePosition(507);
					break;
				case 9:
					snekAudioSystem->fmodEventInstances["Instruments+FX/arp"]->setTimelinePosition(675);
					break;
				case 11:
					snekAudioSystem->fmodEventInstances["Instruments+FX/arp"]->setTimelinePosition(844);
					break;
				case 13:
					snekAudioSystem->fmodEventInstances["Instruments+FX/arp"]->setTimelinePosition(1012);
					break;
				case 15:
					snekAudioSystem->fmodEventInstances["Instruments+FX/arp"]->setTimelinePosition(1181);
					break;
				}
				break;

			case 2:
				switch (i16thNote) {
				case 1:
					snekAudioSystem->fmodEventInstances["Instruments+FX/arp"]->setTimelinePosition(2023);
					break;
				case 3:
					snekAudioSystem->fmodEventInstances["Instruments+FX/arp"]->setTimelinePosition(2192);
					break;
				case 5:
					snekAudioSystem->fmodEventInstances["Instruments+FX/arp"]->setTimelinePosition(2361);
					break;
				case 7:
					snekAudioSystem->fmodEventInstances["Instruments+FX/arp"]->setTimelinePosition(2529);
					break;
				case 9:
					snekAudioSystem->fmodEventInstances["Instruments+FX/arp"]->setTimelinePosition(2698);
					break;
				case 11:
					snekAudioSystem->fmodEventInstances["Instruments+FX/arp"]->setTimelinePosition(2866);
					break;
				case 13:
					snekAudioSystem->fmodEventInstances["Instruments+FX/arp"]->setTimelinePosition(3035);
					break;
				case 15:
					snekAudioSystem->fmodEventInstances["Instruments+FX/arp"]->setTimelinePosition(3203);
					break;
				}
				break;

			case 3:
				switch (i16thNote) {
				case 1:
					snekAudioSystem->fmodEventInstances["Instruments+FX/arp"]->setTimelinePosition(4046);
					break;
				case 3:
					snekAudioSystem->fmodEventInstances["Instruments+FX/arp"]->setTimelinePosition(4214);
					break;
				case 5:
					snekAudioSystem->fmodEventInstances["Instruments+FX/arp"]->setTimelinePosition(4383);
					break;
				case 7:
					snekAudioSystem->fmodEventInstances["Instruments+FX/arp"]->setTimelinePosition(4552);
					break;
				case 9:
					snekAudioSystem->fmodEventInstances["Instruments+FX/arp"]->setTimelinePosition(4720);
					break;
				case 11:
					snekAudioSystem->fmodEventInstances["Instruments+FX/arp"]->setTimelinePosition(4889);
					break;
				case 13:
					snekAudioSystem->fmodEventInstances["Instruments+FX/arp"]->setTimelinePosition(5057);
					break;
				case 15:
					snekAudioSystem->fmodEventInstances["Instruments+FX/arp"]->setTimelinePosition(5226);
					break;
				}
				break;

			case 4:
				switch (i16thNote) {
				case 1:
					snekAudioSystem->fmodEventInstances["Instruments+FX/arp"]->setTimelinePosition(6068);
					break;
				case 3:
					snekAudioSystem->fmodEventInstances["Instruments+FX/arp"]->setTimelinePosition(6237);
					break;
				case 5:
					snekAudioSystem->fmodEventInstances["Instruments+FX/arp"]->setTimelinePosition(6405);
					break;
				case 7:
					snekAudioSystem->fmodEventInstances["Instruments+FX/arp"]->setTimelinePosition(6574);
					break;
				case 9:
					snekAudioSystem->fmodEventInstances["Instruments+FX/arp"]->setTimelinePosition(6743);
					break;
				case 11:
					snekAudioSystem->fmodEventInstances["Instruments+FX/arp"]->setTimelinePosition(6911);
					break;
				case 13:
					snekAudioSystem->fmodEventInstances["Instruments+FX/arp"]->setTimelinePosition(7080);
					break;
				case 15:
					snekAudioSystem->fmodEventInstances["Instruments+FX/arp"]->setTimelinePosition(7248);
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

		snekAudioSystem->fmodUpdate(); //update FMOD system	

		//			 //
		// DRAW SCREEN //	
		//			   //
		for (int t = 0; t < 25; t++) {
			for (int q = 0; q < 25; q++) {

				if (gameGrid[q][t] == 'z') {
					asciiGraphics->drawText(q, t, " ");

				}

				else if (gameGrid[q][t] == '7') {
					asciiGraphics->drawText(q, t, "8");
				}

				else if (gameGrid[q][t] == '8' && snakes[0].direction_frame == 'n') {
					asciiGraphics->drawText(q, t, "^");
				}

				else if (gameGrid[q][t] == '8' && snakes[0].direction_frame == 's') {
					asciiGraphics->drawText(q, t, "v");
				}

				else if (gameGrid[q][t] == '8' && snakes[0].direction_frame == 'w') {
					asciiGraphics->drawText(q, t, "<");
				}

				else if (gameGrid[q][t] == '8' && snakes[0].direction_frame == 'e') {
					asciiGraphics->drawText(q, t, ">");
				}

				else if (gameGrid[q][t] == 'p') {
					asciiGraphics->drawText(q, t, "O");
				}

				else {
					asciiGraphics->drawText(q, t, "+");
				}
			}
		}

		asciiGraphics->fillColor(colorPalette.hud, 25, 0, 25, asciiGraphics->height - 1);
		asciiGraphics->fillText(25, 0, 25, asciiGraphics->height - 1, '|');

		asciiGraphics->drawTextSprite(32, 0, ASCIISprite(
			"       __    _    _              _  __   ____\n"
			"      / /   | \\  | |     /\\     | |/ /  |  __|\n"
			"      \\ \\   |  \\ | |    /  \\    | | /   | |__\n"
			"       \\ \\  | | \\| |   / /\\ \\   |   \\   |  __|\n"
			"       / /  | |\\ \\ |  /  __  \\  | |\\ \\  | |__\n"
			"      /_/   |_| \\__| /__/  \\__\\ |_| \\_\\ |____|",
			colorPalette.hud
		));

		if (playerCount == 1) {
			asciiGraphics->drawText(33, 7, "SCORE: " + std::to_string(highestCurrentLength));
		}
		else {
			asciiGraphics->drawText(30, 7, "P1 SCORE: " + std::to_string(snakes[0].snek_length));
			asciiGraphics->drawText(30, 8, "P2 SCORE: " + std::to_string(snakes[1].snek_length));
		}

		asciiGraphics->drawText(47, 7, "HIGH SCORE: " + std::to_string(highScore));

		asciiGraphics->drawText(33, 9, "STYLE: " + std::to_string(styleCounter));
		asciiGraphics->drawText(47, 9, "HIGH STYLE: " + std::to_string(styleHighScore));

		for (int i = 0; i < playerCount; i++) {
			WORD playerColor = i == 0 ? colorPalette.player_1 : colorPalette.player_2;

			if (snakes[i].justDied) {
				asciiGraphics->drawTextSprite(snakes[i].head[0], snakes[i].head[1], ASCIISprite("X", colorPalette.white));
			}
			else {
				switch (snakes[i].direction_frame) {
				case 'n':
					asciiGraphics->drawTextSprite(snakes[i].head[0], snakes[i].head[1], ASCIISprite("^", playerColor));
					break;
				case 's':
					asciiGraphics->drawTextSprite(snakes[i].head[0], snakes[i].head[1], ASCIISprite("v", playerColor));
					break;
				case 'e':
					asciiGraphics->drawTextSprite(snakes[i].head[0], snakes[i].head[1], ASCIISprite(">", playerColor));
					break;
				case 'w':
					asciiGraphics->drawTextSprite(snakes[i].head[0], snakes[i].head[1], ASCIISprite("<", playerColor));
					break;
				}
			}
		}

		if (portalCount) {
			asciiGraphics->drawTextSprite(portalCoordinates[0][0], portalCoordinates[0][1], ASCIISprite("O", colorPalette.portal));
			asciiGraphics->drawTextSprite(portalCoordinates[1][0], portalCoordinates[1][1], ASCIISprite("O", colorPalette.portal));
		}

		if (wasPreviousHighScoreFound) {
			asciiGraphics->drawTextSprite(63, 6, ASCIISprite(
				" -------------\n"
				"|             |\n"
				" -------------",
				colorPalette.hud));
			asciiGraphics->drawText((11 - highScoreName.length()) / 2 + 65, 7, highScoreName);
		}

		if (playerCount == 2 && currentFrame == 1) {

			asciiGraphics->drawTextSprite(14, 8, ASCIISprite(
				"Player 1\n"
				"   |    \n"
				"   |    \n"
				"   V    ",
				colorPalette.player_1
			));

			asciiGraphics->drawTextSprite(4, 8, ASCIISprite(
				"Player 2\n"
				"   |    \n"
				"   |    \n"
				"   V    ",
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
		asciiGraphics->fillColor(colorPalette.fruit, currentFruit[0], currentFruit[1], currentFruit[0], currentFruit[1]);

		//					  //
		// COLOR PLAYER 1 GREEN //
		//						//			

		if (!snakes[0].justDied) {
			asciiGraphics->attributeBuffer[snakes[0].head[0] + (snakes[0].head[1] * 80)] = colorPalette.player_1;

			if (!snakes[0].justGotNewFruit) {
				for (int l = 0; l < snakes[0].snek_length; l++) {
					asciiGraphics->attributeBuffer[snakes[0].snek_body[l][0] + (snakes[0].snek_body[l][1] * 80)] = colorPalette.player_1;
				}
				if (snakes[0].snekSwallowTimer <= snakes[0].snek_length) {
					asciiGraphics->attributeBuffer[snakes[0].snek_body[snakes[0].snekSwallowTimer - 1][0] + (snakes[0].snek_body[snakes[0].snekSwallowTimer - 1][1] * 80)] = colorPalette.fruit_swallowed;
					snakes[0].snekSwallowTimer++;
				}
			}
			else {
				snakes[0].justGotNewFruit = false;
				for (int l = 0; l < snakes[0].snek_length - 1; l++) {
					asciiGraphics->attributeBuffer[snakes[0].snek_body[l][0] + (snakes[0].snek_body[l][1] * 80)] = colorPalette.player_1;
				}
				if (snakes[0].snekSwallowTimer == 0) {
					asciiGraphics->attributeBuffer[snakes[0].head[0] + (snakes[0].head[1] * 80)] = colorPalette.fruit_swallowed;
					snakes[0].snekSwallowTimer++;
				}
			}
		}
		else {
			asciiGraphics->attributeBuffer[snakes[0].head[0] + (snakes[0].head[1] * 80)] = colorPalette.white;
			for (int l = 0; l < snakes[0].snek_length - 1; l++) {
				asciiGraphics->attributeBuffer[snakes[0].snek_body[l][0] + (snakes[0].snek_body[l][1] * 80)] = colorPalette.white;
			}
		}

		//					//
		// COLOR PLAYER 2 RED //
		//					  //
		if (playerCount == 2) {
			if (!snakes[1].justDied) {
				asciiGraphics->attributeBuffer[snakes[1].head[0] + (snakes[1].head[1] * 80)] = colorPalette.player_2;

				if (!snakes[1].justGotNewFruit) {
					for (int l = 0; l < snakes[1].snek_length; l++) {
						asciiGraphics->attributeBuffer[snakes[1].snek_body[l][0] + (snakes[1].snek_body[l][1] * 80)] = colorPalette.player_2;
					}
					if (snakes[1].snekSwallowTimer <= snakes[1].snek_length) {
						asciiGraphics->attributeBuffer[snakes[1].snek_body[snakes[1].snekSwallowTimer - 1][0] + (snakes[1].snek_body[snakes[1].snekSwallowTimer - 1][1] * 80)] = colorPalette.fruit_swallowed;
						snakes[1].snekSwallowTimer++;
					}
				}
				else {
					snakes[1].justGotNewFruit = false;
					for (int l = 0; l < snakes[1].snek_length - 1; l++) {
						asciiGraphics->attributeBuffer[snakes[1].snek_body[l][0] + (snakes[1].snek_body[l][1] * 80)] = colorPalette.player_2;
					}
					if (snakes[1].snekSwallowTimer == 0) {
						asciiGraphics->attributeBuffer[snakes[1].head[0] + (snakes[1].head[1] * 80)] = colorPalette.fruit_swallowed;
						snakes[1].snekSwallowTimer++;
					}
				}
			}
			else {
				asciiGraphics->attributeBuffer[snakes[1].head[0] + (snakes[1].head[1] * 80)] = colorPalette.white;
				for (int l = 0; l < snakes[1].snek_length - 1; l++) {
					asciiGraphics->attributeBuffer[snakes[1].snek_body[l][0] + (snakes[1].snek_body[l][1] * 80)] = colorPalette.white;
				}
			}
		}

		asciiOutput->pushOutput(*asciiGraphics);

		// delay for first frame if in 2 player mode //
		if (playerCount == 2 && currentFrame == 1)
			std::this_thread::sleep_for(std::chrono::milliseconds(3000));
	}

	//				  //
	// GAME OVER SCREEN //
	//					//

	snekAudioSystem->stopEventInstance(bpmNames[currentChordBPM]);
	snekAudioSystem->fmodUpdate(); //update FMOD system	

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
		snekAudioSystem->fmodEventInstances["Instruments+FX/SnakeFruit"]->setVolume(1.0f);


		asciiGraphics->drawText(32, 9, "NEW HIGH SCORE! ENTER YOUR NAME:");
		asciiGraphics->fillText(65, 7, 76, 7, ' ');

		//..display the keyboard..
		for (int y = 0; y < 32; y++) {
			if (y < 8) {
				asciiGraphics->textBuffer[40 + (13 * 80) + (y * 2)] = keyboard[y];
			}
			else if (y < 16) {
				asciiGraphics->textBuffer[24 + (15 * 80) + (y * 2)] = keyboard[y];
			}
			else if (y < 24) {
				asciiGraphics->textBuffer[8 + (17 * 80) + (y * 2)] = keyboard[y];
			}
			else {
				asciiGraphics->textBuffer[-8 + (19 * 80) + (y * 2)] = keyboard[y];
			}
		}
		asciiGraphics->drawText(40, 21, "BACK");
		asciiGraphics->drawText(52, 21, "END");

		//..and let them enter their name..
		while (nameEntry) {

			//CHECK INPUT//
			for (int k = 0; k < 4; k++) {
				snakes[0].directional_keys[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x25\x26\x27\x28"[k]))) != 0;
			}
			snakes[0].action_keys = ((0x8000 & GetAsyncKeyState((unsigned char)("Z"[0]))) != 0);

			//APPLY INPUT//

			//..if they press the left arrow key..
			if (!holdNameEntryLeft && snakes[0].directional_keys[0]) {
				if (currentSelChar - 1 > -1) {
					currentSelChar--;
					snekAudioSystem->fmodEventInstances["Instruments+FX/SnakeFruit"]->setPitch(2.0f);
					snekAudioSystem->startEventInstance("Instruments+FX/SnakeFruit");
				}
				holdNameEntryLeft = true;
			}
			//..if they release the left arrow key..
			else if (!snakes[0].directional_keys[0]) {
				holdNameEntryLeft = false;
			}

			//..if they press the right arrow key..
			if (!holdNameEntryRight && snakes[0].directional_keys[2]) {
				if (currentSelChar + 1 < 34) {
					currentSelChar++;
					snekAudioSystem->fmodEventInstances["Instruments+FX/SnakeFruit"]->setPitch(2.0f);
					snekAudioSystem->startEventInstance("Instruments+FX/SnakeFruit");
				}
				holdNameEntryRight = true;
			}
			//..if they release the right arrow key..
			else if (!snakes[0].directional_keys[2]) {
				holdNameEntryRight = false;
			}

			//..if they press the up arrow key..
			if (!holdNameEntryUp && snakes[0].directional_keys[1]) {
				if (currentSelChar - 8 > -1) {
					currentSelChar -= 8;
					snekAudioSystem->fmodEventInstances["Instruments+FX/SnakeFruit"]->setPitch(2.0f);
					snekAudioSystem->startEventInstance("Instruments+FX/SnakeFruit");
				}
				holdNameEntryUp = true;
			}
			//..if they release the up arrow key..
			else if (!snakes[0].directional_keys[1]) {
				holdNameEntryUp = false;
			}

			//..if they press the down arrow key..
			if (!holdNameEntryDown && snakes[0].directional_keys[3]) {
				if (currentSelChar < 32) {
					if (currentSelChar <= 22) {
						currentSelChar += 8;
					}
					else {
						currentSelChar = currentSelChar <= 27 ? 32 : 33;
					}
					snekAudioSystem->fmodEventInstances["Instruments+FX/SnakeFruit"]->setPitch(2.0f);
					snekAudioSystem->startEventInstance("Instruments+FX/SnakeFruit");
				}
				holdNameEntryDown = true;
			}
			//..if they release the down arrow key..
			else if (!snakes[0].directional_keys[3]) {
				holdNameEntryDown = false;
			}

			//..if they press the Z key..
			if (!holdNameEntryZ && snakes[0].action_keys) {
				if (currentSelChar < 31 && highScoreName.length() < 11) {
					highScoreName.append(1, keyboard[currentSelChar]);
					snekAudioSystem->fmodEventInstances["Instruments+FX/SnakeFruit"]->setPitch(1.0f);
					snekAudioSystem->startEventInstance("Instruments+FX/SnakeFruit");
				}
				else if (currentSelChar == 31 && highScoreName.length() < 11 && highScoreName.length() != 0) {
					highScoreName.append(1, keyboard[currentSelChar]);
					snekAudioSystem->fmodEventInstances["Instruments+FX/SnakeFruit"]->setPitch(1.0f);
					snekAudioSystem->startEventInstance("Instruments+FX/SnakeFruit");
				}
				else if (currentSelChar == 32) {
					if (highScoreName.size() > 0) {
						highScoreName.resize(highScoreName.size() - 1);
						snekAudioSystem->fmodEventInstances["Instruments+FX/SnakeFruit"]->setPitch(0.3f);
						snekAudioSystem->startEventInstance("Instruments+FX/SnakeFruit");
					}
				}
				else if (currentSelChar == 33) {
					nameEntry = false;
					snekAudioSystem->startEventInstance("Instruments+FX/SnakeFruit11");
				}

				holdNameEntryZ = true;
			}
			// ..if they release the Z key..
			else if (!snakes[0].action_keys) {
				holdNameEntryZ = false;
			}


			// erase name display
			asciiGraphics->fillText(65, 7, 76, 7, ' ');


			// display their name as they type it
			asciiGraphics->drawText(63, 6,
				" -------------\n"
				"|             |\n"
				" -------------");


			if (highScoreName.length() == 11) {
				asciiGraphics->textBuffer.replace(65 + (80 * 7), highScoreName.length(), highScoreName);
			}
			else {
				asciiGraphics->textBuffer.replace(((11 - highScoreName.length()) / 2) + 65 + (80 * 7), highScoreName.length() + 1, highScoreName + " ");
			}

			//color the whole right side of the screen green
			asciiGraphics->fillColor(colorPalette.hud, 26, 0, asciiGraphics->width - 1, asciiGraphics->height - 1);

			//invert the color of the currently selected keyboard character
			if (currentSelChar < 8) {
				asciiGraphics->attributeBuffer[40 + (13 * 80) + (currentSelChar * 2)] = colorPalette.keyboard_selected;
			}
			else if (currentSelChar < 16) {
				asciiGraphics->attributeBuffer[24 + (15 * 80) + (currentSelChar * 2)] = colorPalette.keyboard_selected;
			}
			else if (currentSelChar < 24) {
				asciiGraphics->attributeBuffer[8 + (17 * 80) + (currentSelChar * 2)] = colorPalette.keyboard_selected;
			}
			else if (currentSelChar < 32) {
				asciiGraphics->attributeBuffer[-8 + (19 * 80) + (currentSelChar * 2)] = colorPalette.keyboard_selected;
			}
			else if (currentSelChar == 32) {
				asciiGraphics->fillColor(colorPalette.keyboard_selected, 40, 21, 43, 21);
			}
			else {
				asciiGraphics->fillColor(colorPalette.keyboard_selected, 52, 21, 54, 21);
			}

			//DISPLAY THE SCREEN//
			asciiOutput->pushOutput(*asciiGraphics);

			snekAudioSystem->fmodUpdate(); //update FMOD system	
		}

		//						  //
		// WRITE HIGH SCORE TO FILE //
		//							//
		std::ofstream scoreFileWrite;
		scoreFileWrite.open("ScoreFile", std::ios::trunc);
		scoreFileWrite << std::to_string(highScore) << std::endl << highScoreName;
		scoreFileWrite.close();

		wasPreviousHighScoreFound = true;
		asciiGraphics->drawText(32, 9, "           HIGH SCORE SAVED!    ");
	}

	//					 //
	// "TRY AGAIN?" SCREEN //
	//					   //

	asciiGraphics->fillText(40, 13, 79, 20, ' ');


	// color the whole right side of the screen green
	asciiGraphics->fillColor(colorPalette.hud, 26, 0, asciiGraphics->width, asciiGraphics->height);

	Sleep(100);

	if (playerCount == 2) {
		asciiGraphics->drawText(44, 12, "PLAYER " + std::to_string(snakes[0].justDied ? 1 : 2) + " DIED!");
	}
	else {
		asciiGraphics->fillText(44, 21, 62, 21, ' ');
	}

	asciiGraphics->fillText(33, 17, 66, 17, ' ');

	snekAudioSystem->startEventInstance("Menu+Songs/FancyBoss");	// (FMOD)
	snekAudioSystem->fmodUpdate();	//begin FMOD sound generation/song playback

	asciiGraphics->drawText(47, 15, "GAME OVER");
	asciiGraphics->drawText(46, 18, "Players: <" + std::to_string(playerCount) + ">");
	asciiGraphics->drawText(40, 20, ">Press [Z] to play again");
	asciiGraphics->drawText(40, 21, ">Press [X] to quit");

	asciiOutput->pushOutput(*asciiGraphics);

	bool gameOverMessage = true;

	bool zKey = false, xKey = false, holdKey = false;
	bool arrowKeys[4] = {false, false, false, false};

	std::this_thread::sleep_for(std::chrono::milliseconds(527));

	while (gameOverMessage) {

		for (int o = 0; o < 4; o++) {
			arrowKeys[o] = (0x8000 & GetAsyncKeyState((unsigned char)("\x25\x26\x27\x28"[o]))) != 0;
		}

		if (arrowKeys[2] && playerCount < 2 && !holdKey) {
			playerCount++;
			snekAudioSystem->fmodEventInstances["Instruments+FX/SnakeFruit"]->setPitch(1.77f);
			snekAudioSystem->startEventInstance("Instruments+FX/SnakeFruit");
			holdKey = true;
		}

		else if (arrowKeys[0] && playerCount > 1 && !holdKey) {
			playerCount--;
			snekAudioSystem->fmodEventInstances["Instruments+FX/SnakeFruit"]->setPitch(0.64f);
			snekAudioSystem->startEventInstance("Instruments+FX/SnakeFruit");
			holdKey = true;
		}

		if (holdKey && !arrowKeys[0] && !arrowKeys[2]) {
			holdKey = false;
		}

		asciiGraphics->drawText(46, 18, "Players: <" + std::to_string(playerCount) + ">");
		//screenString.replace((18 * 80) + 46, 12, "Players: <" + std::to_string(playerCount) + ">");


		if (zKey = (0x8000 & GetAsyncKeyState((unsigned char)("Z"[0]))) != 0) {
			gameOverMessage = false;
			playAgain = true;

			snekAudioSystem->stopEventInstance("Menu+Songs/FancyBoss");		//(FMOD)
			snekAudioSystem->fmodEventInstances["Instruments+FX/SnakeFruit"]->setPitch(1.0f);
			snekAudioSystem->startEventInstance("Instruments+FX/SnakeFruit");
			
		}

		if (xKey = (0x8000 & GetAsyncKeyState((unsigned char)("X"[0]))) != 0) {
			gameOverMessage = false;
			playAgain = false;

			snekAudioSystem->stopEventInstance("Menu+Songs/FancyBoss", true);		//(FMOD)
			snekAudioSystem->startEventInstance("Menu+Songs/ExitGame");

			snekAudioSystem->fmodUpdate();
			std::this_thread::sleep_for(std::chrono::milliseconds(2671));
		}

		asciiOutput->pushOutput(*asciiGraphics);
		snekAudioSystem->fmodUpdate();
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

}
