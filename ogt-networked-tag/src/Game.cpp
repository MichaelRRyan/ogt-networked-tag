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
	//m_player.setCharName("Jeff");
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

	m_client->setPacketRecievedCallback([&](PacketType type, std::string message)
		{ packetRecieved(type, message); });

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
				
				sf::Vector2i pos = m_players[m_localId].getPos();
				sf::Vector2i newPos = pos;

				if (nextEvent.key.code == sf::Keyboard::Left)
				{
					newPos.x -= 1;
				}
				else if (nextEvent.key.code == sf::Keyboard::Right)
				{
					newPos.x += 1;
				}
				else if (nextEvent.key.code == sf::Keyboard::Up)
				{
					newPos.y -= 1;
				}
				else if (nextEvent.key.code == sf::Keyboard::Down)
				{
					newPos.y += 1;
				}

				if (newPos != pos)
				{
					m_client->requestToMove(newPos.x, newPos.y);
					//m_players[m_localId].setPos(newPos);
				}
			}
		}
	}
}

void Game::update(sf::Time t_deltaTime)
{
	if (m_exitGame)
	{
		m_window.close();
	}

	// If the local id is set.
	if (m_localId != 100)
	{
		m_scoreText.setString(std::to_string(m_players[m_localId].getScore()));
		m_livesText.setString(std::to_string(m_players[m_localId].getLives()));
	}

	for (auto & playerPair : m_players)
		playerPair.second.update(m_maze);
}

void Game::render()
{
	m_window.clear();

	drawGameplay();

	for (auto& playerPair : m_players)
		m_window.draw(playerPair.second);
	

	m_window.display();
}

void Game::setUpGame()
{
	setUpMaze();
	//m_player.respawn();
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

void Game::packetRecieved(PacketType t_packetType, std::string t_string)
{
	switch (t_packetType)
	{
	case PacketType::GameStarted:
		break;
	case PacketType::GameEnded:
		break;
	case PacketType::JoinInfo:
		if (t_string.size() == 3)
		{
			char id = t_string.at(0);
			int tileX = static_cast<int>(t_string.at(1));
			int tileY = static_cast<int>(t_string.at(2));
			m_localId = id;
			m_players.emplace(id, Player()); // Inserts a new player.
			m_players[id].setPos({ tileX, tileY });
			m_players[id].setCharacter(id);
			//m_playerPositions[id] = sf::Vector2f{ 0.0f, (float)(static_cast<int>(t_id) * TILE_SIZE) };
		}
		break;
	case PacketType::MovePlayer:
		if (t_string.size() == 5)
		{
			char id = t_string.at(0);
			int newTileX = static_cast<int>(t_string.at(1));
			int newTileY = static_cast<int>(t_string.at(2));
			int oldTileX = static_cast<int>(t_string.at(3));
			int oldTileY = static_cast<int>(t_string.at(4));
			m_players[id].setPos({ newTileX, newTileY });

			//m_player.move({ oldTileX - newTileX, oldTileY - newTileY }, m_maze);
			//m_playerPositions[id] = sf::Vector2f{ tileX* TILE_SIZE, tileY* TILE_SIZE };
		}
		break;
	case PacketType::PlayerDied:
		break;
	case PacketType::RockMoved:
		break;
	default:
		break;
	}
}
