#include "Game.h"

Game::Game() :
	m_window{ sf::VideoMode{ WINDOW_WIDTH, WINDOW_HEIGHT, 32u }, "OGT Game" },
	m_network{ nullptr },
	m_exitGame{ false }
{
	if (m_hudIconsTexure.loadFromFile("ASSETS\\IMAGES\\hud_icons.png"))
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
	if (m_network != nullptr)
	{
		std::cout << "WARNING: A server can't be created as there is already a "
			<< "network instance running." << std::endl;
		return;
	}

	// Creates a server on port 1111, false=do not loopback to localhost (others can connect).
	m_server = new Server(1111, false);
	m_network = m_server;

	m_server->setPacketRecievedCallback([&](PacketType type, std::string message)
		{ packetRecieved(type, message); });

	m_world.createPlayer(m_server->getLocalId(), { 7, 2 });
	std::cout << "Joining..." << std::endl;
}

bool Game::startClient(std::string const& t_ip)
{
	if (m_network != nullptr)
	{
		std::cout << "WARNING: A client can't be created as there is already a "
			<< "network instance running." << std::endl;
		return false;
	}

	// Create client to connect to a server on port 1111.
	m_client = new Client(t_ip.c_str(), 1111);

	m_client->setPacketRecievedCallback([&](PacketType type, std::string message)
		{ packetRecieved(type, message); });

	if (!m_client->Connect()) //If client fails to connect...
		return false;

	m_network = m_client;

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
			char id = m_network->getLocalId();

			// If an id has been set.
			if (id != m_network->m_UNASSIGNED_ID)
			{
				Player * localPlayer = m_world.getPlayer(id);
				sf::Vector2i pos = localPlayer->getTilePosition();
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
					if (m_client)
						m_client->requestToMove(newPos.x, newPos.y);
					else if (m_world.isTileEmpty(newPos.x, newPos.y))
					{
						m_server->setPlayerPosition(id, newPos.x, newPos.y);
						localPlayer->setTilePosition({ newPos.x, newPos.y });
					}
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

	m_world.update();

	// If the local id is set.
	if (m_network)
	{
		char id = m_network->getLocalId();
		if (id != m_network->m_UNASSIGNED_ID)
		{
			Player* localPlayer = m_world.getPlayer(id);
			if (localPlayer)
			{
				m_scoreText.setString(std::to_string(localPlayer->getScore()));
				m_livesText.setString(std::to_string(localPlayer->getLives()));
			}
		}
	}
}

void Game::render()
{
	m_window.clear();
	m_world.draw(m_window);
	drawGameplay();
	m_window.display();
}

void Game::setUpGame()
{
	//setUpMaze();
	//m_player.respawn();
	//m_maze[2][12].setTileType(Tile::None);
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
	case PacketType::RequestToMove:
		if (t_string.size() == 3)
		{
			char id = t_string.at(0);
			int tileX = static_cast<int>(t_string.at(1));
			int tileY = static_cast<int>(t_string.at(2));

			Player * player = m_world.getPlayer(id);

			if (player && m_world.isTileEmpty(tileX, tileY))
			{
				m_server->setPlayerPosition(id, tileX, tileY);
				player->setTilePosition({ tileX, tileY });
				std::cout << "Moving player id " << (int)id << "..." << std::endl;
			}
		}
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
			
			m_world.createPlayer(id, { tileX, tileY });
			std::cout << "Joining..." << std::endl;
		}
		break;
	case PacketType::PlayerJoined:
		if (t_string.size() == 3)
		{
			char id = t_string.at(0);
			int tileX = static_cast<int>(t_string.at(1));
			int tileY = static_cast<int>(t_string.at(2));
			m_world.createPlayer(id, { tileX, tileY });
			std::cout << "Setting up player..." << std::endl;
		}
		break;
	case PacketType::MovePlayer:
		if (t_string.size() == 3)
		{
			Player * player = m_world.getPlayer(t_string.at(0));
			if (player != nullptr)
			{
				int newTileX = static_cast<int>(t_string.at(1));
				int newTileY = static_cast<int>(t_string.at(2));
				player->setTilePosition({ newTileX, newTileY });
				std::cout << "Moving player..." << std::endl;
			}
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
