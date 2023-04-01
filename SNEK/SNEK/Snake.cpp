#include "Snake.h"

Snake::Snake(WORD color, Soil::Coords2D startPosition, Soil::Coords2D startDirection) : color{ color }, head { startPosition }, direction_tick{ startDirection }, direction_frame{ startDirection } {
	
}
