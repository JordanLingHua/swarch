#include "NetworkManager.h"


NetworkManager::NetworkManager(void)
	:done(false)
{
	// Choose a port to listen to
	listener.listen(4682);

	// The listener will allow us to parse messages based on whether a player is trying to join
	// or if the player is trying to send us a message
	selector.add(listener);
}


NetworkManager::~NetworkManager(void)
{
	done = false;

	// If we haven't deleted all of the sockets, delete them
	for(auto it = clientList.begin(); it != clientList.end(); it++)
	{
		delete *it;
	}
}

void NetworkManager::createConnectionThread()
{
	sf::Thread thread();
}

void NetworkManager::networkInput()
{
	while(!done)
	{
		// Make the selector wait for a message
		if(selector.wait())
		{
			// If the selector has received an invite from a user, try to add them
			if(selector.isReady(listener))
			{
				// Create a connection to our client
				sf::TcpSocket* client = new sf::TcpSocket;
				
				// Accept the client
				if(listener.accept(*client) == sf::Socket::Done)
				{
					std::cout << "A client has joined the server" << std::endl;
					selector.add(*client);
					clientList.push_back(client);
					
					// Allow for input without threading
					client->setBlocking(false);
				}
				// Delete the client if they cannot be accepted
				else
				{
					delete client;
				}
			}
			else
			{
				for(auto it = clientList.begin(); it != clientList.end(); it++)
				{
				
				}
			}
		}
	}
}

void NetworkManager::run()
{
	// While the server isn't finished
	while(!done)
	{
		// If we press the escape key, end the simulation
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
		{
			done = true;
		}

		
	}
}