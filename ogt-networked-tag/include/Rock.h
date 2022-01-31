#pragma once

#include "Cell.h"
#include "Globals.h"

class Rock
{

	Direction dir;
	bool moving = false;
	sf::Vector2i m_pos;
	const int MAX_MOVE_TIME{ 4 };
	int moveTime;

public:

	Rock();
	void update(Cell t_maze[][MAX_COLS]);
	void moveRock(sf::Vector2i t_pos, Direction t_direction);
};

