#ifndef GLOBALS_H
#define GLOBALS_H

enum class Direction
{
	None,
	North,
	South,
	West,
	East
};

enum class Tile
{
	None,
	Rock,
	Coin,
	Moveable
};

const int MAX_ROWS{ 25 };
const int MAX_COLS{ 25 };

const unsigned MAX_GHOSTS{ 4 };
const unsigned WINDOW_WIDTH{ 800u };
const unsigned WINDOW_HEIGHT{ 800u };

// Texture Information
const int TILE_SIZE{ 32 };
const int CHAR_WIDTH{ 32 }; // Character Width - the width of each character sprite
const int CHAR_HEIGHT{ 64 }; // Character height - the height of each character sprite
const int CHAR_SPACING{ 54 }; // The horisontal spacing between each character on the sprite sheet (including margin)

const int MAX_SCORE{ 312 }; // The number of points the player has to score to win
const int MAX_PLAYERS{ 10 }; // The number of players saved to a scoreboard

// Global game functions
class Global
{
public:
	// Public global helper functions
	static sf::Vector2i getDirectionVector(Direction t_direction)
	{
		switch (t_direction)
		{
		case Direction::None:
			return sf::Vector2i{ 0,0 }; // Return a null vector for a null direction
		case Direction::North:
			return sf::Vector2i{ 0,-1 }; // Return a north vector
		case Direction::South:
			return sf::Vector2i{ 0,1 };  // Return a south vector
		case Direction::West:
			return sf::Vector2i{ -1,0 };  // Return a west vector
		case Direction::East:
			return sf::Vector2i{ 1,0 };  // Return a east vector
		default:
			return sf::Vector2i{ 0,0 }; // Return a null vector for a null direction
		}
	}
};

#endif // !GLOBALS_H
