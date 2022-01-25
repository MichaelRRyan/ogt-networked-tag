#include "Game.h"

Game::Game() :
	m_window{ sf::VideoMode{ WINDOW_WIDTH, WINDOW_HEIGHT, 32u }, "OGT Game" },
	m_exitGame{ false },
	m_localId{ 100 }
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
}

Game::~Game()
{
	if (m_server) delete m_server;
	if (m_client) delete m_client;
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

void Game::startServer()
{
	// Creates a server on port 1111, false=do not loopback to localhost (others can connect).
	m_server = new Server(1111, false);
}

bool Game::startClient(std::string const& t_ip)
{
	// Create client to connect to a server on port 1111.
	m_client = new Client(t_ip.c_str(), 1111);

	m_client->setInitInfoCallback([&](char id)
		{ initInfoRecieved(id); });

	m_client->setPlayerPositionCallback([&](char id, char x, char y)
		{ playerPositionRecieved(id, x, y); });

	if (!m_client->Connect()) //If client fails to connect...
		return false;

	return true;
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
		else if (sf::Event::KeyPressed == nextEvent.type)
		{
			// If an id has been set.
			if (m_localId != 100)
			{
				sf::Vector2f pos = m_playerPositions[m_localId];
				m_client->sendPlayerPosition(((int)pos.x) / TILE_SIZE + 1, ((int)pos.y) / TILE_SIZE);
			}
		}
	}
}

void Game::update(sf::Time t_deltaTime)
{
	if (m_exitGame)
		m_window.close();

	updateNetworking();
}

void Game::updateNetworking()
{
	if (m_server)
	{
		// Accepts a new connection (if someones trying to connect).
		m_server->ListenForNewConnection();
	}

	if (m_client)
	{
		int x = 0;
		int y = 0;
		std::cin >> x;
		std::cin >> y;
		m_client->sendPlayerPosition(x, y);
	}
}

void Game::render()
{
	m_window.clear();

	drawGameplay();

	m_window.display();
}

void Game::setUpGame()
{
	setUpMaze();
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

	// Draws placeholder players at their positions.
	for (auto & player : m_playerPositions)
	{
		m_tileSprite.setPosition(player.second);
		m_tileSprite.setTextureRect({ 320, 448, 32, 32 });
		m_window.draw(m_tileSprite);
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

void Game::initInfoRecieved(char t_id)
{
	m_playerPositions[t_id] = sf::Vector2f{ 0.0f, 0.0f };
	m_localId = t_id;
}

void Game::playerPositionRecieved(char t_id, char t_x, char t_y)
{
	m_playerPositions[t_id] = sf::Vector2f{ 
		static_cast<float>(t_x) * TILE_SIZE, static_cast<float>(t_y) * TILE_SIZE };
}
