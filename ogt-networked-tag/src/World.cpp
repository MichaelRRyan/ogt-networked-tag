#include "World.h"
#include "..\include\World.h"

World::World()
{
	if (m_terrainTexture.loadFromFile("ASSETS\\IMAGES\\terrain_atlas.png"))
		m_tileSprite.setTexture(m_terrainTexture);

	setupMaze();
}

World::~World()
{
	for (auto & playerPair : m_players)
		delete playerPair.second;
}

void World::update()
{
	for (auto& playerPair : m_players)
		playerPair.second->update(m_maze);
}

void World::draw(sf::RenderWindow & t_window)
{
	for (int row = 0; row < MAX_ROWS; row++)
	{
		for (int col = 0; col < MAX_COLS; col++)
		{
			m_tileSprite.setPosition(static_cast<float>(32 * col), static_cast<float>(32 * row));
			int grassType = (col + row) % 3;
			m_tileSprite.setTextureRect(sf::IntRect{ TILE_SIZE * (21 + grassType), TILE_SIZE * 5, TILE_SIZE, TILE_SIZE });
			t_window.draw(m_tileSprite);

			if (m_maze[row][col].getTileType() != Tile::None)
			{
				m_tileSprite.setTextureRect(m_maze[row][col].getTexturePosition());
				t_window.draw(m_tileSprite);
			}
		}
	}

	for (auto& playerPair : m_players)
		t_window.draw(*playerPair.second);
}

void World::createPlayer(char t_id, sf::Vector2i const& t_tilePos)
{
	m_players[t_id] = new Player();
	m_players[t_id]->setPos(t_tilePos);
	m_players[t_id]->setCharacter((int)t_id);
}

Player* World::getPlayer(char t_id)
{
	if (m_players.count(t_id))
		return m_players[t_id];
	else
		return nullptr;
}

std::map<char, Player*>& World::getPlayers()
{
	return m_players;
}

Tile World::getTileType(int t_x, int t_y) const
{
	return m_maze[t_y][t_x].getTileType();
}

bool World::isTileEmpty(int t_x, int t_y) const
{
	return m_maze[t_y][t_x].getTileType() == Tile::None;
}

void World::setupMaze()
{
	int mazeSetup[MAX_ROWS][MAX_COLS]{ // Setup a temporary 2D maze to setup the main maze
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
	{ 0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0 },
	{ 0,0,1,1,0,1,1,0,0,0,0,0,0,0,0,0,0,0,1,1,0,1,1,0,0 },
	{ 0,0,1,1,0,1,1,0,0,1,1,0,0,1,1,1,0,0,1,1,0,1,1,0,0 },
	{ 0,0,0,0,0,1,1,0,0,1,1,1,0,0,1,1,0,0,1,1,0,0,0,0,0 },
	{ 0,1,1,1,0,0,0,0,0,1,1,1,1,0,0,1,0,0,0,0,0,1,1,1,0 },
	{ 0,1,1,1,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,1,1,1,0 },
	{ 0,0,0,0,0,1,0,0,1,1,1,1,1,3,1,3,1,0,0,1,0,0,0,0,0 },
	{ 0,0,1,1,0,1,0,0,1,1,1,1,0,0,0,0,1,0,0,1,0,1,1,0,0 },
	{ 0,0,1,1,0,1,0,0,1,1,0,0,0,0,0,0,0,0,0,1,0,1,1,0,0 },
	{ 0,0,1,1,0,1,0,0,1,1,0,1,1,1,0,0,0,0,0,1,0,1,1,0,0 },
	{ 0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0 },
	{ 0,0,1,1,0,1,0,0,0,0,0,1,1,0,0,0,0,0,0,1,0,1,1,0,0 },
	{ 0,0,1,1,0,1,0,0,0,0,1,1,1,0,0,1,1,0,0,1,0,1,1,0,0 },
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0 },
	{ 0,0,1,1,0,1,0,0,1,0,0,0,0,1,1,1,1,0,0,1,0,1,1,0,0 },
	{ 0,0,1,1,0,1,0,0,1,3,1,3,1,1,1,1,1,0,0,1,0,1,1,0,0 },
	{ 0,0,1,1,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,1,1,0,0 },
	{ 0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0 },
	{ 0,3,1,1,1,0,0,0,0,1,0,0,1,1,1,1,0,0,0,0,1,1,1,3,0 },
	{ 0,0,0,0,0,0,0,0,0,1,1,0,0,1,1,1,0,0,0,0,0,0,0,0,0 },
	{ 0,0,1,1,0,1,1,0,0,1,1,1,0,0,1,1,0,0,1,1,0,1,1,0,0 },
	{ 0,0,1,1,0,1,1,0,0,0,0,0,0,0,0,0,0,0,1,1,0,1,1,0,0 },
	{ 0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0 },
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }
	};

	for (int row = 0; row < MAX_ROWS; row++)
	{
		for (int col = 0; col < MAX_COLS; col++)
		{
			m_maze[row][col].setTileType(static_cast<Tile>(mazeSetup[row][col]));
			if (mazeSetup[row][col] == 0)
				m_maze[row][col].setTileType(Tile::None);

			if (row == 0 || row == MAX_ROWS - 1 || col == 0 || col == MAX_COLS - 1)
				m_maze[row][col].setTileType(Tile::Rock);
		}
	}
}