#pragma once
#include "TextGraphics.h"

class TextOutput
{
	virtual void pushOutput(TextGraphics& textGraphics) = 0;
};

