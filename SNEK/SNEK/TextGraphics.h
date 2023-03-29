#pragma once
#include <string>
#include <vector>
#include <Windows.h>

#include "TextSprite.h"

class TextGraphics {
public:
	int width;
	int height;
	std::wstring textBuffer;
	std::vector<WORD> attributeBuffer;

	TextGraphics(int width, int height);

	void drawTextSprite(int x, int y, TextSprite sprite);
	void drawText(int x, int y, std::wstring stringToWrite);
	void fillText(int left, int top, int right, int bottom, wchar_t charToWrite);
	void fillColor(WORD colorToDraw, int left, int top, int right, int bottom);
};

