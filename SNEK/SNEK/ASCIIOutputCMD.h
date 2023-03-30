#pragma once
#include "ASCIIOutput.h"

#include <Windows.h>
#include <string>
#include <vector>

class ASCIIOutputCMD : public ASCIIOutput {
private:
    HANDLE originalConsoleHandle = 0;
    HANDLE gameConsoleHandle = 0;
public:
    ASCIIOutputCMD();
    ~ASCIIOutputCMD();
    void pushOutput(ASCIIGraphics& textGraphics);
};

