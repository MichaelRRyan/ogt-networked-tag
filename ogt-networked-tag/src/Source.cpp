/*
Author: Michael Rainsford Ryan
Starter code: Pindrought
Date: 29/12/2021
*/

#ifdef _DEBUG 
#pragma comment(lib,"sfml-graphics-d.lib") 
#pragma comment(lib,"sfml-audio-d.lib") 
#pragma comment(lib,"sfml-system-d.lib") 
#pragma comment(lib,"sfml-window-d.lib") 
#pragma comment(lib,"sfml-network-d.lib") 
#else 
#pragma comment(lib,"sfml-graphics.lib") 
#pragma comment(lib,"sfml-audio.lib") 
#pragma comment(lib,"sfml-system.lib") 
#pragma comment(lib,"sfml-window.lib") 
#pragma comment(lib,"sfml-network.lib") 
#endif

#include "Game.h"
#include <ctime>
#include <cstdlib>

#include "Server.h"
#include "Client.h"
#include <iostream>

int main()
{
	srand(static_cast<unsigned>(time(nullptr))); // Set the random value seed

	Game game; // Create a game class

	int num = 0;
	std::cout << "1 to start server, 2 to connect to a server: ";
	std::cin >> num;

	if (num == 1)
	{
		game.startServer();
	}
	else if (num == 2)
	{
		// Starts the client, breaks out if it fails.
		if (!game.startClient("127.0.0.1"))
		{
			std::cout << "Failed to connect to server..." << std::endl;
			system("pause");
			return -1;
		}
	}

	game.run(); // Run the game class

	return EXIT_SUCCESS;
}