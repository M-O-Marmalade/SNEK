#include "ASCIIGraphics.h"

#include "utfcpp/source/utf8.h"

Soil::ASCIIGraphics::ASCIIGraphics(int width, int height) : width{ width }, height{ height } {
	this->textBuffer = std::vector<std::u32string>(height, std::u32string(width, U' '));
	this->attributeBuffer = std::vector<WORD>(width * height, FOREGROUND_RED);
}

void Soil::ASCIIGraphics::clearAll() {
	for (auto& vec : this->textBuffer) {
		for (auto& character : vec) {
			character = U' ';
		}
	}
	for (auto& attribute : this->attributeBuffer) {
		attribute = 0;
	}
}

void Soil::ASCIIGraphics::drawTextSprite(int x, int y, ASCIISprite sprite) {
	int i = 0, x1 = x, y1 = y;
	while (i < sprite.text.size() && x1 >= 0 && y1 >= 0 && x1 < this->width && y1 < this->height) {
		if (sprite.text[i] == U'\n') {
			x1 = x;
			y1++;
		}
		else {
			this->textBuffer[y1][x1] = sprite.text[i];
			this->attributeBuffer[x1 + y1 * this->width] = sprite.color;
			x1++;
		}
		i++;
	}
}

void Soil::ASCIIGraphics::drawTextSprite(Coords2D coordinates, ASCIISprite sprite) {
	drawTextSprite(coordinates.x, coordinates.y, sprite);
}

void Soil::ASCIIGraphics::drawText(int x, int y, char32_t charToWrite) {
	this->textBuffer[y][x] = charToWrite;
}

void Soil::ASCIIGraphics::drawText(int x, int y, char charToWrite) {
	Soil::ASCIIGraphics::drawText(x, y, utf8::utf8to32(std::string(1, charToWrite))[0]);
}

void Soil::ASCIIGraphics::drawText(int x, int y, std::string stringToWrite) {
	std::u32string stringToWrite32 = utf8::utf8to32(stringToWrite);
	int i = 0, x1 = x, y1 = y;
	while (i < stringToWrite32.size() && x1 >= 0 && y1 >= 0 && x1 < this->width && y1 < this->height) {
		if (stringToWrite32[i] == U'\n') {
			x1 = x;
			y1++;
		}
		else {
			this->textBuffer[y1][x1] = stringToWrite32[i];
			x1++;
		}
		i++;
	}
}

void Soil::ASCIIGraphics::drawText(Coords2D coordinates, std::string stringToWrite) {
	this->drawText(coordinates.x, coordinates.y, stringToWrite);
}

void Soil::ASCIIGraphics::fillText(int left, int top, int right, int bottom, char charToWrite) {
	Soil::ASCIIGraphics::fillText(left, top, right, bottom, utf8::utf8to32(std::string(1, charToWrite))[0]);
}

void Soil::ASCIIGraphics::fillText(int left, int top, int right, int bottom, char32_t charToWrite) {
	int x = left, y = top;
	for (int y = top; y <= bottom; y++) {
		for (int x = left; x <= right; x++) {
			this->textBuffer[y][x] = charToWrite;
		}
	}
}

void Soil::ASCIIGraphics::fillColor(WORD colorToDraw, int left, int top, int right, int bottom) {
	int x = std::max(left, 0), y = std::max(top, 0);
	while (y <= bottom && y < this->height) {
		this->attributeBuffer[x + y * this->width] = colorToDraw; // color foreground & background
		if (x < right && x < this->width - 1) {
			x++;
		}
		else {
			x = left;
			y++;
		}
	}
}

void Soil::ASCIIGraphics::fillColor(WORD colorToDraw, int x, int y) {
		this->attributeBuffer[x + y * this->width] = colorToDraw; // color foreground & background
}

void Soil::ASCIIGraphics::fillColorBackground(WORD colorToDraw, int left, int top, int right, int bottom) {
	WORD backgroundColorMask = BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_RED | BACKGROUND_INTENSITY;
	int x = std::max(left, 0), y = std::max(top, 0);
	while (y <= bottom && y < this->height) {
		this->attributeBuffer[x + y * this->width] |= colorToDraw & backgroundColorMask;
		if (x < right && x < this->width - 1) {
			x++;
		}
		else {
			x = left;
			y++;
		}
	}
}