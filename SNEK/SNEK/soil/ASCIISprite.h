#pragma once

#include <string>
#include <Windows.h>

namespace Soil {
	class ASCIISprite {
	private:

	public:
		std::u32string text;
		WORD color;
		ASCIISprite(std::string text, WORD color);
		ASCIISprite(std::u32string text, WORD color);
	};
}