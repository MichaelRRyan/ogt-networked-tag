#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>
#include <map>

#include "Globals.h"
#include "World.h"
#include "Server.h"
#include "Client.h"


class Game
{
public:
	Game();
	~Game();

	void run();

	void startServer();
	bool startClient(std::string const & t_ip);

private:

	void processEvents();
	void update(sf::Time t_deltaTime);
	void render();
	void setUpGame();
	void setUpFontAndText();
	void drawGameplay();

	void packetRecieved(PacketType t_packetType, std::string t_string);

	sf::RenderWindow m_window; // Render window
	bool m_exitGame; // Closes the window if true

	sf::Texture m_hudIconsTexure; // Textures for the hud

	sf::Font m_twosonFont; // Font for text objects

	sf::Text m_scoreText; // Text for player score
	sf::Text m_livesText; // Text for player lives
	sf::Sprite m_hudIcons; // Icons for the player's hud

	// Networking variables.
	Server * m_server;
	Client * m_client;

	// Is either a server or client, serves as a common interface.
	Network* m_network;

	World m_world; // The game world.

};

#endif // !GAME

