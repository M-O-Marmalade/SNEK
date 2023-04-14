#include "ASCIIGraphics.h"

#include "utfcpp/source/utf8.h"

Soil::ASCIIGraphics::ASCIIGraphics(int width, int height) : width{ width }, height{ height } {
	this->textBuffer = std::vector<std::u32string>(height, std::u32string(width, U' '));
	this->attributeBuffer = std::vector<WORD>(width * height, FOREGROUND_RED);
	this->textChanged = std::vector<std::vector<bool>>(height, std::vector<bool>(width, false));
	this->attributeChanged = std::vector<std::vector<bool>>(height, std::vector<bool>(width, false));
}

void Soil::ASCIIGraphics::clearAll() {
	for (auto& vec : this->textBuffer) {
		for (auto& character : vec) {
			character = U' ';
		}
	}
	resetTextObserver(true);
	for (auto& attribute : this->attributeBuffer) {
		attribute = 0;
	}
	resetAttributeObserver(true);
}

void Soil::ASCIIGraphics::resetTextObserver(bool val) {
	for (int i = 0; i < this->textChanged.size(); i++) {
		for (int j = 0; j < this->textChanged[0].size(); j++) {
			this->textChanged[i][j] = val;
		}
	}
}

void Soil::ASCIIGraphics::resetAttributeObserver(bool val) {
	for (int i = 0; i < this->attributeChanged.size(); i++) {
		for (int j = 0; j < this->attributeChanged[0].size(); j++) {
			this->attributeChanged[i][j] = val;
		}
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
			this->textChanged[y1][x1] = true;
			this->attributeBuffer[x1 + y1 * this->width] = sprite.color;
			this->attributeChanged[y1][x1] = true;
			x1++;
		}
		i++;
	}
}

void Soil::ASCIIGraphics::drawTextSprite(Coords2D coordinates, ASCIISprite sprite) {
	drawTextSprite(coordinates.x, coordinates.y, sprite);
}

void Soil::ASCIIGraphics::drawText(int x, int y, char32_t charToWrite) {
	if (x >= 0 && x < this->width && y >= 0 && y < this->height) {
		this->textBuffer[y][x] = charToWrite;
		this->textChanged[y][x] = true;
	}
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
			this->textChanged[y1][x1] = true;
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
			this->textChanged[y][x] = true;
		}
	}
}

void Soil::ASCIIGraphics::fillColor(int left, int top, int right, int bottom, WORD colorToDraw) {
	int x = std::max(left, 0), y = std::max(top, 0);
	while (y <= bottom && y < this->height) {
		this->attributeBuffer[x + y * this->width] = colorToDraw; // color foreground & background
		this->attributeChanged[y][x] = true;
		if (x < right && x < this->width - 1) {
			x++;
		}
		else {
			x = left;
			y++;
		}
	}
}

void Soil::ASCIIGraphics::fillColor(int x, int y, WORD colorToDraw) {
	if (x >= 0 && x < this->width && y >= 0 && y < this->height) {
		this->attributeBuffer[x + y * this->width] = colorToDraw; // color foreground & background
		this->attributeChanged[y][x] = true;
	}
}

void Soil::ASCIIGraphics::fillColorBackground(int left, int top, int right, int bottom, WORD colorToDraw) {
	WORD backgroundColorMask = BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_RED | BACKGROUND_INTENSITY;
	int x = std::max(left, 0), y = std::max(top, 0);
	while (y <= bottom && y < this->height) {
		this->attributeBuffer[x + y * this->width] |= colorToDraw & backgroundColorMask;
		this->attributeChanged[y][x] = true;
		if (x < right && x < this->width - 1) {
			x++;
		}
		else {
			x = left;
			y++;
		}
	}
}