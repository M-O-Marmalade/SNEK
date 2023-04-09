#include "ASCIISprite.h"

#include "utfcpp/source/utf8.h"

Soil::ASCIISprite::ASCIISprite(std::string text, WORD color) : color{ color } {
	this->text = utf8::utf8to32(text);
}

Soil::ASCIISprite::ASCIISprite(std::u32string text, WORD color) : color{ color }, text{ text } {
}
