#pragma once

class SNEKControlScheme {
private:

public:
	char up;
	char down;
	char left;
	char right;
	char action;

	SNEKControlScheme(char up, char down, char left, char right, char action) : up{ up }, down{ down }, left{ left }, right{ right }, action{ action } {
	}
};