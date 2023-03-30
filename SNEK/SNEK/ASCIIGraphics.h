#pragma once
#include <string>
#include <vector>
#include <Windows.h>

#include "ASCIISprite.h"

class ASCIIGraphics {
public:
	int width;
	int height;
	std::string textBuffer;
	std::vector<WORD> attributeBuffer;

	ASCIIGraphics(int width, int height);

	void drawTextSprite(int x, int y, ASCIISprite sprite);
	void drawText(int x, int y, std::string stringToWrite);
	void fillText(int left, int top, int right, int bottom, wchar_t charToWrite);
	void fillColor(WORD colorToDraw, int left, int top, int right, int bottom);
};

