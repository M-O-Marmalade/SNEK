#include "windows.h"
#include <iostream>
#include <string>

using namespace std;

int frameRate = 250;

int display[25]{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

int snekHead[2]{ 3 , 12 };
	
char direction = 'w';

// = is snek
// + is food
// X is trap

// 11.5 is halfway



int main() {

	/*for (int q = 0; q < 25; q++) {

		cout << q << " = " << display[q] << endl;

	}*/

	for (int q = 0; q < 25; q++) {

		display[q] = 0;

	}

	display[snekHead[0]] = snekHead[1];

	

	for (int q = 0; q < 1;) {

		int z = 0;
		
	/* 1 */ cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snekHead[0] == z) { cout << "="; } z++;
	/* 2 */	cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snekHead[0] == z) { cout << "="; } z++;
	/* 3 */ cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snekHead[0] == z) { cout << "="; } z++;
	/* 4 */ cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snekHead[0] == z) { cout << "="; } z++;
	/* 5 */ cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snekHead[0] == z) { cout << "="; } z++;
	/* 6 */ cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snekHead[0] == z) { cout << "="; } z++;
	/* 7 */ cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snekHead[0] == z) { cout << "="; } z++;
	/* 8 */ cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snekHead[0] == z) { cout << "="; } z++;
	/* 9 */ cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snekHead[0] == z) { cout << "="; } z++;
	/* 10*/ cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snekHead[0] == z) { cout << "="; } z++;
	/* 11*/ cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snekHead[0] == z) { cout << "="; } z++;
	/* 12*/ cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snekHead[0] == z) { cout << "="; } z++;
	/* 13*/ cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snekHead[0] == z) { cout << "="; } z++;
	/* 14*/ cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snekHead[0] == z) { cout << "="; } z++;
	/* 15*/ cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snekHead[0] == z) { cout << "="; } z++;
	/* 16*/ cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snekHead[0] == z) { cout << "="; } z++;
	/* 17*/ cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snekHead[0] == z) { cout << "="; } z++;
	/* 18*/ cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snekHead[0] == z) { cout << "="; } z++;
	/* 19*/ cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snekHead[0] == z) { cout << "="; } z++;
	/* 20*/ cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snekHead[0] == z) { cout << "="; } z++;
	/* 21*/ cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snekHead[0] == z) { cout << "="; } z++;
	/* 22*/ cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snekHead[0] == z) { cout << "="; } z++;
	/* 23*/ cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snekHead[0] == z) { cout << "="; } z++;
	/* 24*/ cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snekHead[0] == z) { cout << "="; } z++;
	/* 25*/ cout << "\n"; for (int i = 0; i < display[z]; i++) { cout << " "; } if (snekHead[0] == z) { cout << "="; } z++;
																							 
	if (direction == 'w') {

		snekHead[1]--;

	}

	else if (direction == 'e') {

		snekHead[1]++;

	}

	if (snekHead[1] >= 25) {

		direction = 'w';


	}

	else if (snekHead[1] <= 1) {

		direction = 'e';

	}

		Sleep(frameRate);

	}

	return 0;

}