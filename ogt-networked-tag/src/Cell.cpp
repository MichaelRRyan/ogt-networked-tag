#include "Cell.h"

Cell::Cell()
{
	m_tileType = Tile::None;
}

void Cell::setTileType(Tile t_tileType)
{
	m_tileType = t_tileType;

	if (t_tileType == Tile::Rock)
		m_texturePosition = sf::IntRect{ TILE_SIZE * 10,TILE_SIZE * 7,TILE_SIZE,TILE_SIZE };
	else if (t_tileType == Tile::Coin)
		m_texturePosition = sf::IntRect{ TILE_SIZE * 12,TILE_SIZE * 24,TILE_SIZE,TILE_SIZE };
	else if (t_tileType == Tile::Moveable)
		m_texturePosition = sf::IntRect{ TILE_SIZE * 15,TILE_SIZE * 15,TILE_SIZE,TILE_SIZE };
}
