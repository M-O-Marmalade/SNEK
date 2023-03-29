#pragma once

#include <string>
#include <Windows.h>

class TextSprite {
private:

public:
	std::wstring text;
	WORD color;
	TextSprite(std::wstring text, WORD color);
};