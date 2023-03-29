#include "TextOutputCMD.h"

TextOutputCMD::TextOutputCMD() {

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

TextOutputCMD::~TextOutputCMD() {
	SetConsoleActiveScreenBuffer(this->originalConsoleHandle);
}

void TextOutputCMD::pushOutput(TextGraphics& textGraphics) {
	DWORD dwBytesWritten;
	for (short y = 0; y < textGraphics.height; y++) {
		WriteConsoleOutputAttribute(this->gameConsoleHandle, &textGraphics.attributeBuffer[y * textGraphics.width], textGraphics.width, { 0,y }, &dwBytesWritten);
		WriteConsoleOutputCharacterW(this->gameConsoleHandle, textGraphics.textBuffer.c_str() + y * textGraphics.width, textGraphics.width, { 0,y }, &dwBytesWritten);

		// using VTS to draw the screen
		//wprintf(CSI L"%d;%dH", y, x);	// position the cursor
		//wprintf(CSI L"38;2;%d;%d;%dm",	// set foreground RGB color
		//	    attributes[x + y * nScreenWidth].first[0],
		//	    attributes[x + y * nScreenWidth].first[1],
		//	    attributes[x + y * nScreenWidth].first[2]);
		//wprintf(CSI L"48;2;%d;%d;%dm"); // set background RGB color
		//wprintf(L"%c" ,screenString[x + y * nScreenWidth]); // print the character
	}
}
