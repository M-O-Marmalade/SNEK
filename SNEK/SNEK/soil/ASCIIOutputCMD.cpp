#include "ASCIIOutputCMD.h"

#include <algorithm>
#include "utfcpp/source/utf8.h"

#define ESC "\x1b"
#define CSI "\x1b["

Soil::ASCIIOutputCMD::ASCIIOutputCMD() {

	// store reference to console buffer that launched the game
	this->originalConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

	// create console buffer for the game to run in
	this->gameConsoleHandle = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(this->gameConsoleHandle);

	// disable the cursor visibility
	CONSOLE_CURSOR_INFO cursorInfo;
	GetConsoleCursorInfo(this->gameConsoleHandle, &cursorInfo);
	cursorInfo.bVisible = false;
	SetConsoleCursorInfo(this->gameConsoleHandle, &cursorInfo);

	// disable mouse input
	DWORD consoleMode;
	GetConsoleMode(this->gameConsoleHandle, &consoleMode);
	consoleMode ^= ENABLE_MOUSE_INPUT;
	SetConsoleMode(this->gameConsoleHandle, consoleMode);

	// set console buffer size
	/*CONSOLE_SCREEN_BUFFER_INFO consoleBufferInfo;
	GetConsoleScreenBufferInfo(hConsole, &consoleBufferInfo);
	consoleBufferInfo.srWindow.Top = 0;
	consoleBufferInfo.srWindow.Left = 0;
	consoleBufferInfo.srWindow.Right = 80;
	consoleBufferInfo.srWindow.Bottom = 25;
	SetConsoleWindowInfo(hConsole, TRUE, &(consoleBufferInfo.srWindow));*/
}

Soil::ASCIIOutputCMD::~ASCIIOutputCMD() {
	SetConsoleActiveScreenBuffer(this->originalConsoleHandle);
}

void Soil::ASCIIOutputCMD::pushOutput(ASCIIGraphics& asciiGraphics) {

	// check what size our window is
	CONSOLE_SCREEN_BUFFER_INFO consoleScreenBufferInfo;
	GetConsoleScreenBufferInfo(this->gameConsoleHandle, &consoleScreenBufferInfo);

	if (
		consoleScreenBufferInfo.dwMaximumWindowSize.X != this->currentWindowSize.X || 
		consoleScreenBufferInfo.dwMaximumWindowSize.Y != this->currentWindowSize.Y
		) {

		// resize our buffer to fit the window size
		SetConsoleScreenBufferSize(this->gameConsoleHandle, consoleScreenBufferInfo.dwMaximumWindowSize);

		// clear the buffer of any leftover characters from the previous size
		DWORD garbage;
		FillConsoleOutputCharacter(
			this->gameConsoleHandle,
			' ',
			consoleScreenBufferInfo.dwMaximumWindowSize.X * consoleScreenBufferInfo.dwMaximumWindowSize.Y, { 0,0 }, &garbage
		);

		// store the size for comparison the next time we check this `if` statement
		this->currentWindowSize = consoleScreenBufferInfo.dwMaximumWindowSize;
	}


	// find the coordinates we need to draw our ASCIIGraphics at to be centered
	short xOrigin = std::max(0, (consoleScreenBufferInfo.dwMaximumWindowSize.X - asciiGraphics.width) / 2);
	short yOrigin = std::max(0, (consoleScreenBufferInfo.dwMaximumWindowSize.Y - asciiGraphics.height) / 2);

	DWORD dwBytesWritten;
	for (short y = 0; y < asciiGraphics.height; y++) {
		if (std::any_of(asciiGraphics.textChanged[y].begin(), asciiGraphics.textChanged[y].end(), [](bool v) { return v; })) {
			std::u16string outputLine = utf8::utf8to16(utf8::utf32to8(asciiGraphics.textBuffer[y]));

			WriteConsoleOutputCharacterW(this->gameConsoleHandle, 
										 (LPCWSTR)outputLine.c_str(),
										 outputLine.length(),
										 { xOrigin, (short)(yOrigin + y) }, 
										 &dwBytesWritten);
		}
		if (std::any_of(asciiGraphics.attributeChanged[y].begin(), asciiGraphics.attributeChanged[y].end(), [](bool v) { return v; })) {
			WriteConsoleOutputAttribute(this->gameConsoleHandle, 
										&asciiGraphics.attributeBuffer[y * asciiGraphics.width], 
										asciiGraphics.width,
										{ xOrigin, (short)(yOrigin + y) },
										&dwBytesWritten);
		}
	}

	asciiGraphics.resetTextObserver(false);
	asciiGraphics.resetAttributeObserver(false);


	// using VTS to draw the screen
	//for (int y = 0; y < asciiGraphics.height; y++) {
	//	for (int x = 0; x < asciiGraphics.width; x++) {
	//		wprintf(CSI L"%d;%dH", y, x);	// position the cursor
	//		wprintf(CSI L"38;2;%d;%d;%dm",	// set foreground RGB color
	//		        asciiGraphics.attributeBuffer[x + y * asciiGraphics.width].first[0],
	//		        asciiGraphics.attributeBuffer[x + y * asciiGraphics.width].first[1],
	//		        asciiGraphics.attributeBuffer[x + y * asciiGraphics.width].first[2]);
	//		//wprintf(CSI L"48;2;%d;%d;%dm"); // set background RGB color
	//		wprintf(L"%c" ,asciiGraphics.textBuffer[y][x]); // print the character
	//	}
	//}
}
