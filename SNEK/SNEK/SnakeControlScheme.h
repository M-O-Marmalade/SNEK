#pragma once

class SnakeControlScheme {
private:

public:
	char up;
	char down;
	char left;
	char right;
	char action;

	SnakeControlScheme(char up, char down, char left, char right, char action) : up{ up }, down{ down }, left{ left }, right{ right }, action{ action } {
	}
};