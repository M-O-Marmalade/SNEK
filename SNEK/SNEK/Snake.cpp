#include "Snake.h"

Snake::Snake(int startX, int startY, char startDirection) : direction_tick{ startDirection }, direction_frame{ startDirection } {
	this->head[0] = startX;
	this->head[1] = startY;
}
