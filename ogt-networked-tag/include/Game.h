#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>

#include "Globals.h"
#include "Cell.h"
#include "Server.h"
#include "Client.h"
#include "Player.h"


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
	void setUpMaze();
	void setUpFontAndText();
	void drawGameplay();
	void initInfoRecieved(char t_id);
	void playerPositionRecieved(char t_id, char t_x, char t_y);

	sf::RenderWindow m_window; // Render window
	bool m_exitGame; // Closes the window if true

	sf::Texture m_terrainTexture; // Textures for the tiles
	sf::Texture m_hudIconsTexure; // Textures for the hud

	sf::Font m_twosonFont; // Font for text objects

	sf::Text m_scoreText; // Text for player score
	sf::Text m_livesText; // Text for player lives
	sf::Sprite m_tileSprite; // sprite for each tile
	sf::Sprite m_hudIcons; // Icons for the player's hud


	Cell m_maze[MAX_ROWS][MAX_COLS];

	// Networking variables.
	Server * m_server;
	Client * m_client;

	std::map<char, sf::Vector2f> m_playerPositions;
	char m_localId;
	Player m_player;

};

#endif // !GAME

