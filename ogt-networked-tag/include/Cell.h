#ifndef CELL_H
#define CELL_H

#include <SFML\Graphics.hpp>
#include "Globals.h"

class Cell
{
	Tile m_tileType;
	sf::IntRect m_texturePosition;
	sf::Vector2f m_position;

public:
	Cell();

	inline Tile getTileType() { return m_tileType; }
	inline sf::IntRect getTexturePosition() { return m_texturePosition; }

	void setTileType(Tile t_tileType);
	inline void setPosition(sf::Vector2f t_position) { m_position = t_position; }
};

#endif // !CELL_H
