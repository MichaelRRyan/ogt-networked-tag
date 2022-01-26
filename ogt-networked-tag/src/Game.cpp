#include "Game.h"

Game::Game() :
	m_window{ sf::VideoMode{ WINDOW_WIDTH, WINDOW_HEIGHT, 32u }, "OGT Game" },
	m_exitGame{ false }
{
	if (!m_terrainTexture.loadFromFile("ASSETS\\IMAGES\\terrain_atlas.png"))
	{
		// Error loading file
	}
	m_tileSprite.setTexture(m_terrainTexture);

	if (!m_hudIconsTexure.loadFromFile("ASSETS\\IMAGES\\hud_icons.png"))
	{
		// Error loading file
	}
	m_hudIcons.setTexture(m_hudIconsTexure);

	setUpGame();
	setUpFontAndText();
	m_player.setCharName("Jeff");
}

Game::~Game()
{
}

void Game::run()
{
	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;
	const float FPS = 60.0f;
	sf::Time timePerFrame = sf::seconds(1.0f / FPS);
	while(m_window.isOpen())
	{
		processEvents();
		timeSinceLastUpdate += clock.restart();

		if (timeSinceLastUpdate > timePerFrame)
		{
			timeSinceLastUpdate -= timePerFrame;
			processEvents();
			update(timePerFrame);
		}
		render();
	}

}

void Game::processEvents()
{
	sf::Event nextEvent;
	while(m_window.pollEvent(nextEvent))
	{
		if (sf::Event::Closed == nextEvent.type)
		{
			m_window.close();
		}
	}
}

void Game::update(sf::Time t_deltaTime)
{
	if (m_exitGame)
	{
		m_window.close();
	}

	m_scoreText.setString(std::to_string(m_player.getScore()));
	m_livesText.setString(std::to_string(m_player.getLives()));

	m_player.update(m_maze);
}

void Game::render()
{
	m_window.clear();

	drawGameplay();
	m_window.draw(m_player);

	m_window.display();
}

void Game::setUpGame()
{
	setUpMaze();
	m_player.respawn();
	m_maze[2][12].setTileType(Tile::None);
}

void Game::setUpMaze()
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
				m_maze[row][col].setTileType(Tile::Coin);

			if (row == 0 || row == MAX_ROWS - 1 || col == 0 || col == MAX_COLS - 1)
				m_maze[row][col].setTileType(Tile::Rock);
		}
	}

}

void Game::setUpFontAndText()
{
	if (!m_twosonFont.loadFromFile("ASSETS\\FONTS\\twoson.ttf"))
	{
		// Error loading font file
	}
	m_scoreText.setFont(m_twosonFont);
	m_scoreText.setPosition(static_cast<float>(WINDOW_WIDTH / 2 + 70), 0.0f);

	m_livesText.setFont(m_twosonFont);
	m_livesText.setPosition(static_cast<float>(WINDOW_WIDTH / 2 - 60), 0.0f);
}

void Game::drawGameplay()
{
	for (int row = 0; row < MAX_ROWS; row++)
	{
		for (int col = 0; col < MAX_COLS; col++)
		{
			m_tileSprite.setPosition(static_cast<float>(32 * col), static_cast<float>(32 * row));
			int grassType = (col + row) % 3;
			m_tileSprite.setTextureRect(sf::IntRect{ TILE_SIZE * (21 + grassType), TILE_SIZE * 5, TILE_SIZE, TILE_SIZE });
			m_window.draw(m_tileSprite);

			if (m_maze[row][col].getTileType() != Tile::None)
			{
				m_tileSprite.setTextureRect(m_maze[row][col].getTexturePosition());
				m_window.draw(m_tileSprite);
			}
		}
	}

	m_hudIcons.setTextureRect({ 64, 0, 288, 32 });
	m_hudIcons.setPosition(256.0f, 0.0f);
	m_window.draw(m_hudIcons);

	m_hudIcons.setTextureRect({ 0, 0, 32, 32 });
	m_hudIcons.setPosition(static_cast<float>(WINDOW_WIDTH / 2 - 100), 0.0f);
	m_window.draw(m_hudIcons);

	m_hudIcons.setTextureRect({ 32, 0, 32, 32 });
	m_hudIcons.setPosition(static_cast<float>(WINDOW_WIDTH / 2 + 20), 0.0f);
	m_window.draw(m_hudIcons);

	m_window.draw(m_scoreText);
	m_window.draw(m_livesText);
}
