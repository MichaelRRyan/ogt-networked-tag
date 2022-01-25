/*
Author: Michael Rainsford Ryan
Starter code: Pindrought
Date: 29/12/2021
*/

#include "Server.h"
#include "Client.h"
#include <iostream>

int main()
{
	int num = 0;
	std::cout << "1 to start server, 2 to connect to a server: ";
	std::cin >> num;

	if (num == 1)
	{
		Server MyServer(1111, false); //Create server on port 1111, false=do not loopback to localhost (others can connect)
		while (true)
		{
			MyServer.ListenForNewConnection(); //Accept new connection (if someones trying to connect)
		}
	}
	else if (num == 2)
	{
		Client myClient("127.0.0.1", 1111); //Create client to connect to server 127.0.0.1 [localhost] on port 1111
		if (!myClient.Connect()) //If client fails to connect...
		{
			std::cout << "Failed to connect to server..." << std::endl;
			system("pause");
			return -1;
		}
		std::string userinput; //holds the user's chat message
		while (true)
		{
			system("cls");
			for (int y = 0; y < 10; ++y)
			{
				for (int x = 0; x < 10; ++x)
				{

					bool drawn = false;

					for (auto& pair : myClient.m_playerPositions)
					{
						if (pair.second.first == x && pair.second.second == y)
						{
							std::cout << " " << (int)pair.first;
							drawn = true;
						}
					}

					if (!drawn)
						std::cout << " -";
				}

				std::cout << std::endl;
			}

			//std::getline(std::cin, userinput); // Get line if user presses enter and fill the buffer
			int x = 0;
			int y = 0;
			std::cin >> x;
			std::cin >> y;
			std::string message{ " " };
			message += static_cast<char>(x);
			message += static_cast<char>(y);
			myClient.SendString(message); // Send string to server
		}

		std::cin.get();
	}
	return 0;
}