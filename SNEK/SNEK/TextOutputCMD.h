#pragma once
#include "TextOutput.h"

#include <Windows.h>
#include <string>
#include <vector>

class TextOutputCMD : public TextOutput {
private:
    HANDLE originalConsoleHandle = 0;
    HANDLE gameConsoleHandle = 0;
public:
    TextOutputCMD();
    ~TextOutputCMD();
    void pushOutput(TextGraphics& textGraphics);
};

