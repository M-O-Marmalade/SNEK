#include "SnakeGame.h"

using namespace std::chrono_literals;

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

SnakeGame::SnakeGame(SnakeGameOptions options, Soil::ASCIIGraphics* asciiGraphics, Soil::ASCIIOutputCMD* asciiOutput, Soil::AudioSystem* audioSystem, Soil::InputManager* inputManager) : options{options}, asciiGraphics{asciiGraphics}, asciiOutput{asciiOutput}, snekAudioSystem{audioSystem}, inputManager{inputManager} {

	this->gameGrid = std::vector<std::vector<char>>(options.gridSize.x,std::vector<char>(options.gridSize.y, ' '));
	this->inputManager->addKeys("\x25\x26\x27\x28WASDZPV");
	this->readScoreFile();
	this->clearGameGrid();
	this->asciiGraphics->clearAll();

	// initialize `snakes` depending on amount of players
	this->snakes = std::vector<Snake>();
	if (options.playerCount == 1) {
		this->snakes.push_back(Snake(options.colors.player_1, { (int)gameGrid.size() / 2, (int)gameGrid[0].size() / 2}, {0,1}, SnakeControlScheme(VK_UP, VK_DOWN, VK_LEFT, VK_RIGHT, 'Z')));
	}
	if (options.playerCount == 2) {
		this->snakes.push_back(Snake(options.colors.player_1, { 2 * ((int)gameGrid.size() / 3), (int)gameGrid[0].size() / 2 }, { 0,1 }, SnakeControlScheme(VK_UP, VK_DOWN, VK_LEFT, VK_RIGHT, 'P')));
		this->snakes.push_back(Snake(options.colors.player_2, { 1 * ((int)gameGrid.size() / 3), (int)gameGrid[0].size() / 2 }, { 0,1 }, SnakeControlScheme('W', 'S', 'A', 'D', 'V')));
	}

	this->placeNewFruit();

	// Load the required FMOD soundbanks
	for (std::string& name : bpmNames) {
		snekAudioSystem->loadEventInstance(name);
	}

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

	// record start time of first frame of the game loop
	frameTime = std::chrono::steady_clock::now();
	bool firstFrameOfTheGame = true;

	// clear the input buffer
	this->inputManager->getBufferState();

	//				     //
	// [GAME LOOP START] //
	//				   //
	while (gameLose == false) {

		  //               //
		 // SET FRAMERATE //
		//               //
		for (int i = 0; i < this->fpsScoreThresholds.size(); i++) {
			if (this->highestCurrentLength < this->fpsScoreThresholds[i]) {
				fps = std::chrono::microseconds ((int)(15000000 / bpmValues[i - 1]));
				break;
			}
		}

		std::this_thread::sleep_until(frameTime + fps);
		frameTime += fps;

		this->inputManager->getBufferState();

		  //                                          //
		 // CHECK + SET DIRECTION [FRAME RESOLUTION] //
		//                                          //
		for (Snake& snake : this->snakes) {
			if (inputManager->isKeyPressed(snake.controls.left) && snake.direction_frame.x == 0) {
				snake.direction_frame = Soil::Coords2D{ -1,0 };
			}

			else if (inputManager->isKeyPressed(snake.controls.up) && snake.direction_frame.y == 0) {
				snake.direction_frame = Soil::Coords2D{ 0,-1 };
			}

			else if (inputManager->isKeyPressed(snake.controls.right) && snake.direction_frame.x == 0) {
				snake.direction_frame = Soil::Coords2D{ 1,0 };
			}

			else if (inputManager->isKeyPressed(snake.controls.down) && snake.direction_frame.y == 0) {
				snake.direction_frame = Soil::Coords2D{ 0,1 };
			}
		}

		/*if (options.playerCount == 2 && firstFrameOfTheGame) {
			frameTime = std::chrono::steady_clock::now();
		}*/


		////			  //
		//// TICK CLOCK //
		////			//
		//while (std::chrono::steady_clock::now() < frameTime + fps) {


		//	  //                                         //
		//	 // CHECK + SET DIRECTION [TICK RESOLUTION] //
		//	//                                         //

		//	for (Snake& snake : this->snakes) {
		//		if (inputManager->isKeyPressed(snake.controls.left) && !snake.holdW && snake.direction_frame.x != 1) {
		//			snake.direction_tick = Soil::Coords2D{-1,0};
		//		}
		//		else if (inputManager->isKeyPressed(snake.controls.up) && !snake.holdN && snake.direction_frame.y != 1) {
		//			snake.direction_tick = Soil::Coords2D{0,-1};
		//		}
		//		else if (inputManager->isKeyPressed(snake.controls.right) && !snake.holdE && snake.direction_frame.x != -1) {
		//			snake.direction_tick = Soil::Coords2D{1,0};
		//		}
		//		else if (inputManager->isKeyPressed(snake.controls.down) && !snake.holdS && snake.direction_frame.y != -1) {
		//			snake.direction_tick = Soil::Coords2D{0,1};
		//		}
		//	}
		//}


		//firstFrameOfTheGame = false;
		//frameTime += std::chrono::duration_cast<std::chrono::nanoseconds>(fps);

		  //                 //
		 // REFRESH DISPLAY //
		//                 //
		for (auto& column : gameGrid) {
			for (auto& character : column) {
				character = ' ';
			}
		}

		  //                    //
		 // MOVE BODY SEGMENTS //
		//                    //
		for (Snake& snake : snakes) {
			for (int i = snake.body.size() - 1; i >= 0; i--) {
				if (i > 0) {
					//move all segments except the segment right before the head
					snake.body[i] = snake.body[i - 1];
				}
				else {
					//move the segment right before the head
					snake.body[i] = snake.head;
				}
			}
		}

		//								      //
		// PLACE SNEK BODY INTO DISPLAY ARRAY //
		//									//
		for (Snake& snake : this->snakes) {
			for (int r = snake.body.size() - 1; r >= 0; r--) {
				gameGrid[snake.body[r].x][snake.body[r].y] = '8';
			}
		}

		//								  //
		// STORE EACH SNEK'S SURROUNDINGS //
		//								//
		//for (Snake& snake : this->snakes) {

		//	//North
		//	if (snake.head.y > 0 && gameGrid[snake.head.x][snake.head.y - 1] == ' ' || gameGrid[snake.head.x][snake.head.y - 1] == '+') {
		//		snake.surroundingObstacles[0] = false;
		//	}
		//	else {
		//		snake.surroundingObstacles[0] = true;
		//	}

		//	//North-East
		//	if (snake.head.x < 24 && snake.head.y > 0 && gameGrid[snake.head.x + 1][snake.head.y - 1] == ' ' || gameGrid[snake.head.x + 1][snake.head.y - 1] == '+') {
		//		snake.surroundingObstacles[1] = false;
		//	}
		//	else {
		//		snake.surroundingObstacles[1] = true;
		//	}

		//	//East
		//	if (snake.head.x < 24 && gameGrid[snake.head.x + 1][snake.head.y] == ' ' || gameGrid[snake.head.x + 1][snake.head.y] == '+') {
		//		snake.surroundingObstacles[2] = false;
		//	}
		//	else {
		//		snake.surroundingObstacles[2] = true;
		//	}

		//	//South-East
		//	if (snake.head.x < 24 && snake.head.y < 24 && gameGrid[snake.head.x + 1][snake.head.y + 1] == ' ' || gameGrid[snake.head.x + 1][snake.head.y + 1] == '+') {
		//		snake.surroundingObstacles[3] = false;
		//	}
		//	else {
		//		snake.surroundingObstacles[3] = true;
		//	}

		//	//South
		//	if (snake.head.y < 24 && gameGrid[snake.head.x][snake.head.y + 1] == ' ' || gameGrid[snake.head.x][snake.head.y + 1] == '+') {
		//		snake.surroundingObstacles[4] = false;
		//	}
		//	else {
		//		snake.surroundingObstacles[4] = true;
		//	}

		//	//South-West
		//	if (snake.head.x > 0 && snake.head.y < 24 && gameGrid[snake.head.x - 1][snake.head.y + 1] == ' ' || gameGrid[snake.head.x - 1][snake.head.y + 1] == '+') {
		//		snake.surroundingObstacles[5] = false;
		//	}
		//	else {
		//		snake.surroundingObstacles[5] = true;
		//	}

		//	//West
		//	if (snake.head.x > 0 && gameGrid[snake.head.x - 1][snake.head.y] == ' ' || gameGrid[snake.head.x - 1][snake.head.y] == '+') {
		//		snake.surroundingObstacles[6] = false;
		//	}
		//	else {
		//		snake.surroundingObstacles[6] = true;
		//	}

		//	//North-West
		//	if (snake.head.x > 0 && snake.head.y > 0 && gameGrid[snake.head.x - 1][snake.head.y - 1] == ' ' || gameGrid[snake.head.x - 1][snake.head.y - 1] == '+') {
		//		snake.surroundingObstacles[7] = false;
		//	}
		//	else {
		//		snake.surroundingObstacles[7] = true;
		//	}
		//}

		////				    //
		//// ADD STYLE POINTS //
		////				  //
		//for (Snake& snake : this->snakes) {

		//	//EAST//
		//	if (snake.direction_frame == 'e') {

		//		//if player jumps over an obstacle to the East, add two style points
		//		if (snake.action_keys && snake.surroundingObstacles[2] && snake.head.x + 2 < 25) {
		//			if (gameGrid[snake.head.x + 2][snake.head.y] == ' ' || gameGrid[snake.head.x + 2][snake.head.y] == '+') {
		//				styleCounter += 2;
		//			}
		//		}

		//		//if player goes in-between two obstacles to the North-East and South-East, add one style point
		//		else if (snake.surroundingObstacles[1] && snake.surroundingObstacles[3] && !snake.surroundingObstacles[2] && snake.head.x + 1 < 25) {
		//			styleCounter++;
		//		}
		//	}

		//	//WEST//
		//	if (snake.direction_frame == 'w') {

		//		//if player jumps over an obstacle to the West, add two style points
		//		if (snake.action_keys && snake.surroundingObstacles[6] && snake.head.x - 2 > 0) {
		//			if (gameGrid[snake.head.x - 2][snake.head.y] == ' ' || gameGrid[snake.head.x - 2][snake.head.y] == '+') {
		//				styleCounter += 2;
		//			}
		//		}

		//		//if player goes in-between two obstacles to the North-West and South-West, add one style point
		//		else if (snake.surroundingObstacles[7] && snake.surroundingObstacles[5] && !snake.surroundingObstacles[6] && snake.head.x - 1 > 0) {
		//			styleCounter++;
		//		}
		//	}

		//	//NORTH//
		//	if (snake.direction_frame == 'n') {

		//		//if player jumps over an obstacle to the North, add two style points
		//		if (snake.action_keys && snake.surroundingObstacles[0] && snake.head.y - 2 > 0) {
		//			if (gameGrid[snake.head.x][snake.head.y - 2] == ' ' || gameGrid[snake.head.x][snake.head.y - 2] == '+') {
		//				styleCounter += 2;
		//			}
		//		}

		//		//if player goes in-between two obstacles to the North-West and North-East, add one style point
		//		else if (snake.surroundingObstacles[7] && snake.surroundingObstacles[1] && !snake.surroundingObstacles[0] && snake.head.y - 1 > 0) {
		//			styleCounter++;
		//		}
		//	}

		//	//SOUTH//
		//	if (snake.direction_frame == 's') {

		//		//if player jumps over an obstacle to the South, add two style points
		//		if (snake.action_keys && snake.surroundingObstacles[4] && snake.head.y + 2 < 25) {
		//			if (gameGrid[snake.head.x][snake.head.y + 2] == ' ' || gameGrid[snake.head.x][snake.head.y + 2] == '+') {
		//				styleCounter += 2;
		//			}
		//		}

		//		//if player goes in-between two obstacles to the South-West and South-East, add one style point
		//		else if (snake.surroundingObstacles[5] && snake.surroundingObstacles[3] && !snake.surroundingObstacles[4] && snake.head.y + 1 < 25) {
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

		  //             //
		 // MOVE PLAYER //
		//             //
		for (Snake& snake : this->snakes) {
			snake.head += snake.direction_frame;
			if (highestCurrentLength >= 11 && this->inputManager->isKeyPressed(snake.controls.action)) {
				snake.head += snake.direction_frame;
			}
		}

		//								     //
		// DETECT IF PLAYER HAS HIT MAP EDGE //
		//								   //
		for (Snake& snake : this->snakes) {
			if (snake.head.x < 0 || snake.head.x >= gameGrid.size() || snake.head.y < 0 || snake.head.y >= gameGrid[0].size()) {
				snake.justDied = true;
				gameLose = true;

				snake.head.x = std::min((int)gameGrid.size() - 1, std::max(0, snake.head.x));
				snake.head.y = std::min((int)gameGrid[0].size() - 1, std::max(0, snake.head.y));
			}
		}


		  //                                     //
		 // DETECT IF PLAYER HAS HIT THEMSELVES //
		//                                     //
		for (Snake& snake : this->snakes) {
			if (gameGrid[snake.head.x][snake.head.y] == '8' || gameGrid[snake.head.x][snake.head.y] == 'X' || gameGrid[snake.head.x][snake.head.y] == 'h') {
				snake.justDied = true;
				gameLose = true;
			}
		}

		  //                                  //
		 // CALCULATE PROXIMITY TO THE FRUIT //
		//                                  //

		// calculate each player's individual proximity
		std::vector<float> fruitProximities;
		for (Snake& snake : this->snakes) {
			float thisSnakesProximityToFruit = 1.0f - ((std::abs(snake.head.x - currentFruit.x) + std::abs(snake.head.y - currentFruit.y)) / 48.0f);	//1/48 max distance
			fruitProximities.push_back(thisSnakesProximityToFruit);
		}

		// use the closest proximity
		float closestProximityToFruit = 48.0f;
		for (float& proximity : fruitProximities) {
			closestProximityToFruit = std::min(proximity, closestProximityToFruit);
		}


		processFruitPickups();

		//				  //
		// CREATE PORTALS //
		//				//
		if (highestCurrentLength == 14 && portalCount < 1) {
			portalCount++;

			bool portalsFarEnoughApart = false;
			while (!portalsFarEnoughApart) {

				for (int e = 0; e == 0;) {
					portalCoordinates[0][0] = (rand() % (gameGrid.size() - 2)) + 1;
					portalCoordinates[0][1] = (rand() % (gameGrid[0].size() - 2)) + 1;

					if (portalCoordinates[0][0] != snakes[0].head.x && portalCoordinates[0][1] != snakes[0].head.y) {
						if (gameGrid[portalCoordinates[0][0]][portalCoordinates[0][1]] != '8' && gameGrid[portalCoordinates[0][0]][portalCoordinates[0][1]] != 'X' && gameGrid[portalCoordinates[0][0]][portalCoordinates[0][1]] != 'o' && gameGrid[portalCoordinates[0][0]][portalCoordinates[0][1]] != 'O') {
							e = 1;
						}
					}
				}

				for (int e = 0; e == 0;) {
					portalCoordinates[1][0] = (rand() % (gameGrid.size() - 2)) + 1;
					portalCoordinates[1][1] = (rand() % (gameGrid[0].size() - 2)) + 1;

					if (portalCoordinates[1][0] != snakes[0].head.x && portalCoordinates[1][1] != snakes[0].head.y) {
						if (gameGrid[portalCoordinates[1][0]][portalCoordinates[1][1]] != '8' && gameGrid[portalCoordinates[1][0]][portalCoordinates[1][1]] != 'X' && gameGrid[portalCoordinates[1][0]][portalCoordinates[1][1]] != 'o' && gameGrid[portalCoordinates[1][0]][portalCoordinates[1][1]] != 'O') {
							e = 1;
						}
					}
				}

				if (abs(portalCoordinates[0][0] - portalCoordinates[1][0]) + abs(portalCoordinates[0][1] - portalCoordinates[1][1]) > std::min(gameGrid.size(), gameGrid[0].size()) / 3) {
					portalsFarEnoughApart = true;
				}
			}

		}

		//							   //
		// PASS PLAYER THROUGH PORTALS //
		//							 //
		for (Snake& snake : this->snakes) {
			for (int hh = 0; hh < portalCount * 2; hh++) {
				if (snake.head.x == portalCoordinates[hh][0] && snake.head.y == portalCoordinates[hh][1]) {
					snake.head.x = portalCoordinates[((hh + 1) % 2)][0];
					snake.head.y = portalCoordinates[((hh + 1) % 2)][1];
					break;
				}
			}
		}

		//								   //
		// PLACE PLAYER INTO DISPLAY ARRAY //
		//								 //
		for (Snake& snake : this->snakes) {
			gameGrid[snake.head.x][snake.head.y] = 'h';
		}

		//								  //
		// PLACE FRUIT INTO DISPLAY ARRAY //
		//								//
		gameGrid[currentFruit.x][currentFruit.y] = '+';

		  //               //
		 // PROCESS AUDIO //
		//               //
		processAudioFrame(firstFrameOfTheGame, closestProximityToFruit);


		  //                    //
		 // DRAW THE GAME GRID //
		//                    //

		// calculate the grid origin (top-left corner)
		int smallestDimension = std::min(asciiGraphics->width, asciiGraphics->height);
		int gridOX = (smallestDimension - gameGrid.size()) / 2;
		int gridOY = (smallestDimension - gameGrid[0].size()) / 2;

		// calculate the border origin (top-left corner)
		int borderLeft = gridOX - 1;
		int borderTop = gridOY - 1;
		int borderRight = gameGrid.size() + borderLeft + 1;
		int borderBottom = gameGrid[0].size() + borderTop + 1;

		// blank the game grid
		//asciiGraphics->fillText(marginX, marginY, gameGrid.size() + marginX, marginY, ' ');

		// draw the border
		//asciiGraphics->fillText(borderLeft, borderTop, borderRight, borderTop, '-');	// top
		//asciiGraphics->fillText(borderLeft, borderBottom, borderRight, borderBottom, '-');	// bottom
		//asciiGraphics->fillText(borderLeft, borderTop, borderLeft, borderBottom, '|');	// left
		//asciiGraphics->fillText(borderRight, borderTop, borderRight, borderBottom, '|');	// right
		asciiGraphics->fillColor(BACKGROUND_GREEN, borderLeft, borderTop, borderRight, borderTop);	// top
		asciiGraphics->fillColor(BACKGROUND_GREEN, borderLeft, borderBottom, borderRight, borderBottom);	// bottom
		asciiGraphics->fillColor(BACKGROUND_GREEN, borderLeft, borderTop, borderLeft, borderBottom);	// left
		asciiGraphics->fillColor(BACKGROUND_GREEN, borderRight, borderTop, borderRight, borderBottom);	// right


		for (int t = 0; t < this->gameGrid.size(); t++) {
			for (int q = 0; q < this->gameGrid[0].size(); q++) {
				std::string temp;
				temp.push_back(gameGrid[q][t]);
				asciiGraphics->drawText(gridOX + q, gridOY + t, temp);
			}
		}

		for (Snake& snake : this->snakes) {

			if (snake.justDied) {
				asciiGraphics->drawTextSprite(gridOX + snake.head.x, gridOY + snake.head.y, Soil::ASCIISprite("X", options.colors.white));
			}
			else {
				std::string snakeHeadText;
				if (snake.direction_frame == Soil::Coords2D{ 0,-1 }) {
					snakeHeadText = "^";
				}
				else if (snake.direction_frame == Soil::Coords2D{ 0,1 }) {
					snakeHeadText = "v";
				}
				else if (snake.direction_frame == Soil::Coords2D{ 1,0 }) {
					snakeHeadText = ">";
				}
				else if (snake.direction_frame == Soil::Coords2D{ -1,0 }) {
					snakeHeadText = "<";
				}
				else {
					snakeHeadText = "_";
				}
				asciiGraphics->drawTextSprite(gridOX + snake.head.x, gridOY + snake.head.y, Soil::ASCIISprite(snakeHeadText, snake.color));
			}
		}

		if (portalCount) {
			asciiGraphics->drawTextSprite(gridOX + portalCoordinates[0][0], gridOY + portalCoordinates[0][1], Soil::ASCIISprite("O", options.colors.portal));
			asciiGraphics->drawTextSprite(gridOX + portalCoordinates[1][0], gridOY + portalCoordinates[1][1], Soil::ASCIISprite("O", options.colors.portal));
		}

		if (options.playerCount == 2 && firstFrameOfTheGame) {

			asciiGraphics->drawTextSprite(gridOX + snakes[0].head.x - 3, gridOY + snakes[0].head.y - 4, Soil::ASCIISprite(
				"Player 1\n"
				"   |    \n"
				"   |    \n"
				"   V    ",
				options.colors.player_1
			));

			asciiGraphics->drawTextSprite(gridOX + snakes[1].head.x - 3, gridOY + snakes[1].head.y - 4, Soil::ASCIISprite(
				"Player 2\n"
				"   |    \n"
				"   |    \n"
				"   V    ",
				options.colors.player_2
			));
		}


		asciiGraphics->fillColor(options.colors.fruit, gridOX + currentFruit.x, gridOY + currentFruit.y);


		for (Snake& snake : this->snakes) {
			if (!snake.justDied) {
				asciiGraphics->attributeBuffer[gridOX + snake.head.x + (gridOY + snake.head.y) * 80] = snake.color;

				if (!snake.justGotNewFruit) {
					for (Soil::Coords2D& segment : snake.body) {
						asciiGraphics->fillColor(snake.color, gridOX + segment.x, gridOY + segment.y);
					}
					if (snake.snekSwallowTimer <= snake.body.size()) {
						asciiGraphics->fillColor(options.colors.fruit_swallowed, gridOX + snake.body[snake.snekSwallowTimer - 1].x, gridOY + snake.body[snake.snekSwallowTimer - 1].y);
						snake.snekSwallowTimer++;
					}
				}
				else {
					snake.justGotNewFruit = false;
					for (Soil::Coords2D& segment : snake.body) {
						asciiGraphics->fillColor(snake.color, gridOX + segment.x, gridOY + segment.y);
					}
					if (snake.snekSwallowTimer == 0) {
						asciiGraphics->fillColor(options.colors.fruit_swallowed, gridOX + snake.head.x, gridOY + snake.head.y);
						snake.snekSwallowTimer++;
					}
				}
			}
			else {
				asciiGraphics->fillColor(options.colors.white, gridOX + snake.head.x, gridOY + snake.head.y);
				for (Soil::Coords2D& segment : snake.body) {
					asciiGraphics->fillColor(options.colors.white, gridOX + segment.x, gridOY + segment.y);
				}
			}
		}


		  //              //
		 // DRAW THE HUD //
		//              //

		asciiGraphics->drawTextSprite(32, 0, Soil::ASCIISprite(
			"       __    _    _              _  __   ____\n"
			"      / /   | \\  | |     /\\     | |/ /  |  __|\n"
			"      \\ \\   |  \\ | |    /  \\    | | /   | |__\n"
			"       \\ \\  | | \\| |   / /\\ \\   |   \\   |  __|\n"
			"       / /  | |\\ \\ |  /  __  \\  | |\\ \\  | |__\n"
			"      /_/   |_| \\__| /__/  \\__\\ |_| \\_\\ |____|",
			options.colors.hud
		));

		if (options.playerCount == 1) {
			asciiGraphics->drawTextSprite(33, 7, Soil::ASCIISprite("SCORE: " + std::to_string(highestCurrentLength), this->options.colors.hud));
		}
		else {
			asciiGraphics->drawTextSprite(30, 7, Soil::ASCIISprite("P1 SCORE: " + std::to_string(snakes[0].body.size()), this->options.colors.hud));
			asciiGraphics->drawTextSprite(30, 8, Soil::ASCIISprite("P2 SCORE: " + std::to_string(snakes[1].body.size()), this->options.colors.hud));
		}

		asciiGraphics->drawTextSprite(47, 7, Soil::ASCIISprite("HIGH SCORE: " + std::to_string(highScore), this->options.colors.hud));

		asciiGraphics->drawTextSprite(33, 9, Soil::ASCIISprite("STYLE: " + std::to_string(styleCounter), this->options.colors.hud));
		asciiGraphics->drawTextSprite(47, 9, Soil::ASCIISprite("HIGH STYLE: " + std::to_string(styleHighScore), this->options.colors.hud));

		if (highestCurrentLength >= 11) {
			if (options.playerCount == 1) {
				asciiGraphics->drawTextSprite(43, 14, Soil::ASCIISprite("Press Z to lunge!", options.colors.player_1));
			}
			else if (options.playerCount == 2) {
				asciiGraphics->drawTextSprite(43, 14, Soil::ASCIISprite("Press P to lunge!", options.colors.player_1));
				asciiGraphics->drawTextSprite(43, 16, Soil::ASCIISprite("Press V to lunge!", options.colors.player_2));
			}
		}

		if (wasPreviousHighScoreFound) {
			asciiGraphics->drawTextSprite(63, 6, Soil::ASCIISprite(
				" -------------\n"
				"|             |\n"
				" -------------",
				options.colors.hud));
			asciiGraphics->drawText((11 - highScoreName.length()) / 2 + 65, 7, highScoreName);
		}

		// DRAW DEBUG MENU //
		//drawDebugMenu();


		// PUSH GRAPHICS TO OUTPUT //
		asciiOutput->pushOutput(*asciiGraphics);

		// delay for first frame if in 2 player mode //
		if (options.playerCount == 2 && firstFrameOfTheGame) {
			std::this_thread::sleep_for(3s);
			this->frameTime = std::chrono::steady_clock::now();
		}

		firstFrameOfTheGame = false;
	}

	  //                  //
	 // GAME OVER SCREEN //
	//                  //
	gameOverScreen();

}

void SnakeGame::processFruitPickups()
{
	gotNewFruit = false;
	for (Snake& snake : this->snakes) {
		if (snake.head.x == currentFruit.x && snake.head.y == currentFruit.y) {

			snake.body.push_back({ snake.head });
			snake.justGotNewFruit = true;
			gotNewFruit = true;
			snake.snekSwallowTimer = 0;

			this->placeNewFruit();

			if (snake.body.size() >= 11) {
				isScoreUnder11 = false;
			}

			  //                   //
			 // UPDATE HIGH SCORE //
			//                   //

			// update current highest length
			if (snake.body.size() > highestCurrentLength) {
				highestCurrentLength = snake.body.size();

				// update high score
				if (highestCurrentLength > highScore) {
					highScore = highestCurrentLength;

					if (highScore == oldHighScore + 1) {
						gotNewHighScore = true;
						gotNewHighScoreSoundPlay = true;
					}
				}
			}
		}
	}
}

void SnakeGame::processAudioFrame(bool& firstFrameOfTheGame, float closestProximityToFruit)
{

	// play the death sound if the game is lost
	if (gameLose) {
		snekAudioSystem->stopEventInstance(bpmNames[currentChordBPM], true);
		snekAudioSystem->startEventInstance("Instruments+FX/Death");
	}

	if (firstFrameOfTheGame) {
		snekAudioSystem->startEventInstance(bpmNames[0]);
	}

	if (gotNewFruit) {				//if someone gets a new fruit..
		for (Snake& snake : this->snakes) {		//..check each player..
			if (snake.justGotNewFruit) {			//..to see if they were the one who got the new fruit..					
				if (gotNewHighScoreSoundPlay) {
					snekAudioSystem->startEventInstance("Instruments+FX/newHighScore");
					gotNewHighScoreSoundPlay = false;
				}
				else if (snake.body.size() == 11) {		//..if they did get a fruit, see if they just got their 11th fruit..
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
	else if (highestCurrentLength >= 11 && (inputManager->isKeyPressed(snakes[0].controls.action) || snakes.size() > 1 && inputManager->isKeyPressed(snakes[1].controls.action))) {
		snekAudioSystem->fmodEventInstances["Instruments+FX/SnakeLunge"]->setPitch(closestProximityToFruit);
		snekAudioSystem->startEventInstance("Instruments+FX/SnakeLunge");
		snekAudioSystem->fmodEventInstances["Instruments+FX/SnakeMove"]->setParameterByName("Reverb Wet", 1.0f);		//set the reverb level high for the move sound
		if (!anyActionKeysHeld()) {
			snekMoveTimelinePosition = (200 + snekMoveTimelinePositionMax);
		}
	}

	//if nobody got a fruit, and nobody is holding any action keys, then..
	else {

		if (highestCurrentLength == 0) {
			snekAudioSystem->fmodEventInstances[bpmNames[0]]->setParameterByName("HeartRateDryLevel", closestProximityToFruit);
		}

		snekAudioSystem->fmodEventInstances["Instruments+FX/SnakeMove"]->setPitch(closestProximityToFruit);
		snekAudioSystem->fmodEventInstances["Instruments+FX/SnakeMove"]->setTimelinePosition(snekMoveTimelinePosition);
		snekAudioSystem->fmodEventInstances["Instruments+FX/SnakeMove"]->setParameterByName("Reverb Wet", snakeMoveReverbLevel);
		snekAudioSystem->startEventInstance("Instruments+FX/SnakeMove");


		snekMoveTimelinePosition += 200;
		if (snekMoveTimelinePosition >= snekMoveTimelinePositionMax) {
			snekMoveTimelinePosition = 0;
		}
	}

	//CHORDS//			
	if (highestCurrentLength >= 1) {
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
}

void SnakeGame::gameOverScreen()
{

	snekAudioSystem->stopEventInstance(bpmNames[currentChordBPM]);
	snekAudioSystem->fmodUpdate(); //update FMOD system	

	std::this_thread::sleep_for(700ms);

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
		asciiGraphics->fillText(43, 13, 79, 16, ' ');
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
			this->inputManager->getBufferState();

			//APPLY INPUT//

			//..if they press the left arrow key..
			if (!holdNameEntryLeft && this->inputManager->isKeyPressed(VK_LEFT)) {
				if (currentSelChar - 1 > -1) {
					currentSelChar--;
					snekAudioSystem->fmodEventInstances["Instruments+FX/SnakeFruit"]->setPitch(2.0f);
					snekAudioSystem->startEventInstance("Instruments+FX/SnakeFruit");
				}
				holdNameEntryLeft = true;
			}
			//..if they release the left arrow key..
			else if (!this->inputManager->isKeyPressed(VK_LEFT)) {
				holdNameEntryLeft = false;
			}

			//..if they press the right arrow key..
			if (!holdNameEntryRight && this->inputManager->isKeyPressed(VK_RIGHT)) {
				if (currentSelChar + 1 < 34) {
					currentSelChar++;
					snekAudioSystem->fmodEventInstances["Instruments+FX/SnakeFruit"]->setPitch(2.0f);
					snekAudioSystem->startEventInstance("Instruments+FX/SnakeFruit");
				}
				holdNameEntryRight = true;
			}
			//..if they release the right arrow key..
			else if (!this->inputManager->isKeyPressed(VK_RIGHT)) {
				holdNameEntryRight = false;
			}

			//..if they press the up arrow key..
			if (!holdNameEntryUp && this->inputManager->isKeyPressed(VK_UP)) {
				if (currentSelChar - 8 > -1) {
					currentSelChar -= 8;
					snekAudioSystem->fmodEventInstances["Instruments+FX/SnakeFruit"]->setPitch(2.0f);
					snekAudioSystem->startEventInstance("Instruments+FX/SnakeFruit");
				}
				holdNameEntryUp = true;
			}
			//..if they release the up arrow key..
			else if (!this->inputManager->isKeyPressed(VK_UP)) {
				holdNameEntryUp = false;
			}

			//..if they press the down arrow key..
			if (!holdNameEntryDown && this->inputManager->isKeyPressed(VK_DOWN)) {
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
			else if (!this->inputManager->isKeyPressed(VK_DOWN)) {
				holdNameEntryDown = false;
			}

			//..if they press the Z key..
			if (!holdNameEntryZ && this->inputManager->isKeyPressed('Z')) {
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
			else if (!this->inputManager->isKeyPressed('Z')) {
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
			asciiGraphics->fillColor(options.colors.hud, 26, 0, asciiGraphics->width - 1, asciiGraphics->height - 1);

			//invert the color of the currently selected keyboard character
			if (currentSelChar < 8) {
				asciiGraphics->attributeBuffer[40 + (13 * 80) + (currentSelChar * 2)] = options.colors.keyboard_selected;
			}
			else if (currentSelChar < 16) {
				asciiGraphics->attributeBuffer[24 + (15 * 80) + (currentSelChar * 2)] = options.colors.keyboard_selected;
			}
			else if (currentSelChar < 24) {
				asciiGraphics->attributeBuffer[8 + (17 * 80) + (currentSelChar * 2)] = options.colors.keyboard_selected;
			}
			else if (currentSelChar < 32) {
				asciiGraphics->attributeBuffer[-8 + (19 * 80) + (currentSelChar * 2)] = options.colors.keyboard_selected;
			}
			else if (currentSelChar == 32) {
				asciiGraphics->fillColor(options.colors.keyboard_selected, 40, 21, 43, 21);
			}
			else {
				asciiGraphics->fillColor(options.colors.keyboard_selected, 52, 21, 54, 21);
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

	  //                     //
	 // "TRY AGAIN?" SCREEN //
	//                     //

	asciiGraphics->fillText(40, 13, 79, 20, ' ');


	// color the whole right side of the screen green
	asciiGraphics->fillColor(options.colors.hud, 26, 0, asciiGraphics->width, asciiGraphics->height);

	std::this_thread::sleep_for(100ms);

	if (options.playerCount == 2) {
		asciiGraphics->drawText(44, 12, "PLAYER " + std::to_string(snakes[0].justDied ? 1 : 2) + " DIED!");
	}
	else {
		asciiGraphics->fillText(44, 21, 62, 21, ' ');
	}

	asciiGraphics->fillText(33, 17, 66, 17, ' ');

	snekAudioSystem->startEventInstance("Menu+Songs/FancyBoss");	// (FMOD)
	snekAudioSystem->fmodUpdate();	//begin FMOD sound generation/song playback

	asciiGraphics->drawText(47, 15, "GAME OVER");
	asciiGraphics->drawText(46, 18, "Players: <" + std::to_string(options.playerCount) + ">");
	asciiGraphics->drawText(40, 20, ">Press [Z] to play again");
	asciiGraphics->drawText(40, 21, ">Press [X] to quit");

	asciiOutput->pushOutput(*asciiGraphics);

	bool gameOverMessage = true;

	bool zKey = false, xKey = false, holdKey = false;
	bool arrowKeys[4] = { false, false, false, false };

	std::this_thread::sleep_for(527ms);

	while (gameOverMessage) {

		for (int o = 0; o < 4; o++) {
			arrowKeys[o] = (0x8000 & GetAsyncKeyState((unsigned char)("\x25\x26\x27\x28"[o]))) != 0;
		}

		if (arrowKeys[2] && options.playerCount < 2 && !holdKey) {
			options.playerCount++;
			snekAudioSystem->fmodEventInstances["Instruments+FX/SnakeFruit"]->setPitch(1.77f);
			snekAudioSystem->startEventInstance("Instruments+FX/SnakeFruit");
			holdKey = true;
		}

		else if (arrowKeys[0] && options.playerCount > 1 && !holdKey) {
			options.playerCount--;
			snekAudioSystem->fmodEventInstances["Instruments+FX/SnakeFruit"]->setPitch(0.64f);
			snekAudioSystem->startEventInstance("Instruments+FX/SnakeFruit");
			holdKey = true;
		}

		if (holdKey && !arrowKeys[0] && !arrowKeys[2]) {
			holdKey = false;
		}

		asciiGraphics->drawText(46, 18, "Players: <" + std::to_string(options.playerCount) + ">");
		//screenString.replace((18 * 80) + 46, 12, "Players: <" + std::to_string(options.playerCount) + ">");


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
			std::this_thread::sleep_for(2671ms);
		}

		asciiOutput->pushOutput(*asciiGraphics);
		snekAudioSystem->fmodUpdate();
		std::this_thread::sleep_for(10ms);
	}
}

void SnakeGame::drawDebugMenu()
{
	asciiGraphics->drawText(0, 24,
			"frame{" + std::to_string(snakes[0].direction_frame.x) + "," + std::to_string(snakes[0].direction_frame.y) + "}");

	asciiGraphics->drawText(20, 24,
		"tick{" + std::to_string(snakes[0].direction_tick.x) + "," + std::to_string(snakes[0].direction_tick.y) + "}");

	asciiGraphics->drawText(0, 23,
		"holdN:" + std::to_string(snakes[0].holdN) +
		" holdE" + std::to_string(snakes[0].holdE) +
		" holdS" + std::to_string(snakes[0].holdS) +
		" holdW" + std::to_string(snakes[0].holdW));
}

bool SnakeGame::anyActionKeysHeld()
{
	for (Snake& snake : this->snakes) {
		if (this->inputManager->isKeyPressed(snake.controls.action)) {
			return true;
		}
	}
	return false;
}

void SnakeGame::placeNewFruit()
{
	Snake* snakeThatGotTheFruit = &snakes[0];
	for (Snake& snake : snakes) {
		if (snake.justGotNewFruit) {
			snakeThatGotTheFruit = &snake;
		}
	}

	int tries = 300;
	bool foundGoodSpot = false;
	while (tries > 0 && !foundGoodSpot) {
		// create a potential fruit spot
		currentFruit.x = rand() % gameGrid.size();
		currentFruit.y = rand() % gameGrid[0].size();

		// check if that spot is currently filled
		bool spotIsEmpty = true;
		for (Snake& snake : this->snakes) {
			if (currentFruit.x == snake.head.x && currentFruit.y == snake.head.y) {
				spotIsEmpty = false;
				break;
			}
		}
		if (gameGrid[currentFruit.x][currentFruit.y] == 'h') {
			spotIsEmpty = false;
		}
		if (gameGrid[currentFruit.x][currentFruit.y] == '8') {
			spotIsEmpty = false;
		}
		if (gameGrid[currentFruit.x][currentFruit.y] == 'X') {
			spotIsEmpty = false;
		}

		// if that spot is currently filled, try again
		if (!spotIsEmpty) {
			continue;
		}


		//a temp proximity to use in the for loop for the new fruit//
		int proximityToFruitTemp = (abs(snakeThatGotTheFruit->head.x - currentFruit.x) + abs(snakeThatGotTheFruit->head.y - currentFruit.y));


		//calculate snek's potential spots to be on the beat when it reaches the new fruit
		std::vector<int> potentialFruitSpots(3,0);
		if (i16thNote == 1) {
			potentialFruitSpots[0] = 17;
		}
		else {
			potentialFruitSpots[0] = 17 - i16thNote;
		}

		if (i16thNote >= 5) {
			potentialFruitSpots[1] = 16 - (i16thNote - 5);
		}
		else {
			potentialFruitSpots[1] = 5 - i16thNote;
		}

		if (i16thNote >= 13) {
			potentialFruitSpots[2] = 16 - (i16thNote - 13);
		}
		else {
			potentialFruitSpots[2] = 13 - i16thNote;
		}

		// accept new fruit position if it lands the appropriate distance from the snek who just got the last fruit
		for (int spot : potentialFruitSpots) {
			if (proximityToFruitTemp % 16 == spot) {
				foundGoodSpot = true;
				break;
			}
		}

		tries--;
	}
}

void SnakeGame::clearGameGrid()
{
	for (auto& column : this->gameGrid) {
		for (auto& character : column) {
			character = ' ';
		}
	}
}
