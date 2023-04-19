#include "SNEKPlayer.h"

SNEKPlayer::SNEKPlayer(Soil::ASCIIColor color, Soil::Coords2D startPosition, Soil::Coords2D startDirection, SNEKControlScheme controlScheme) : color{ color }, head{ startPosition }, direction_tick{ startDirection }, direction_frame{ startDirection }, controls{ controlScheme } {
	
}
