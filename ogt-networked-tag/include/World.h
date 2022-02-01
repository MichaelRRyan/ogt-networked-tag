#ifndef WORLD_H
#define WORLD_H

#include <iostream>

#include "WorldInterface.h"
#include "Cell.h"
#include "Player.h"

class World : public WorldInterface
{
public:

	World();
	~World();

	void update();
	void draw(sf::RenderWindow & t_window);

	void createPlayer(char t_id, sf::Vector2i const & t_tilePos);
	Player * getPlayer(char t_id);

	std::map<char, Player *> & getPlayers();

	virtual bool isTileEmpty(int t_x, int t_y) const override;

private:

	void setupMaze();

	sf::Texture m_terrainTexture; // Textures for the tiles
	sf::Sprite m_tileSprite; // sprite for each tile

	Cell m_maze[MAX_ROWS][MAX_COLS];
	std::map<char, Player*> m_players;

};

#endif // !WORLD_H
