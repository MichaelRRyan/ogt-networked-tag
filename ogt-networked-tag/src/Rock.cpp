#include "Rock.h"

Rock::Rock()
{
	moveTime = MAX_MOVE_TIME;
}

void Rock::update(Cell t_maze[][MAX_COLS])
{
	if (moveTime > 0)
	{
		moveTime--;
	}
	else
	{
		if (moving)
		{
			sf::Vector2i dirVector = Global::getDirectionVector(dir);
			sf::Vector2i desiredPosition = m_pos + dirVector;

			if (t_maze[desiredPosition.y + dirVector.y][desiredPosition.x + dirVector.x].getTileType() != Tile::Rock &&
				t_maze[desiredPosition.y + dirVector.y][desiredPosition.x + dirVector.x].getTileType() != Tile::Moveable)
			{
				t_maze[desiredPosition.y][desiredPosition.x].setTileType(t_maze[desiredPosition.y + dirVector.y][desiredPosition.x + dirVector.x].getTileType());
				t_maze[desiredPosition.y + dirVector.y][desiredPosition.x + dirVector.x].setTileType(Tile::Moveable);
				m_pos = desiredPosition;
				desiredPosition += dirVector;
			}
			else
			{
				moving = false;
			}
		}
		moveTime = MAX_MOVE_TIME;
	}
}

void Rock::moveRock(sf::Vector2i t_pos, Direction t_direction)
{
	dir = t_direction;
	m_pos = t_pos;
	moving = true;
}
