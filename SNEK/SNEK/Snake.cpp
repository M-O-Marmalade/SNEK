#include "Snake.h"

Snake::Snake(WORD color, Coords2D startPosition, Coords2D startDirection) : color{ color }, head { startPosition }, direction_tick{ startDirection }, direction_frame{ startDirection } {
	
}
