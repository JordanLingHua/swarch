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
	done = true;

	// If we haven't deleted all of the sockets, delete them
	for(auto it = clientList.begin(); it != clientList.end(); it++)
	{
		delete *it;
	}
}

void NetworkManager::networkInput()
{
	std::cout << "Accepting input" << std::endl;

	while(!done)
	{
		// Make the selector wait for a message
		if(selector.wait(sf::milliseconds(10.f)))
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
					if(selector.isReady((**it)))
					{
						// Read the packet in
						sf::Packet pkt;
						if((**it).receive(pkt) == sf::TcpSocket::Disconnected)
						{
							selector.remove(**it);
							auto itToErase = it;
							it++;
							delete (*itToErase);
							clientList.erase(itToErase);
							std::cout << "A client has disconnected" << std::endl;
							continue;
						}
						else
						{
							std::string user, pass;
							if(pkt >> user >> pass)
							{
								if(dm.doesUserExistInDB(user))
								{
									// Check to see if the password matches
									if(dm.doesPasswordMatchUser(user, pass))
									{
										sf::Packet packet;
										packet << "y";
										(**it).send(packet);
									}
									else
									{
										sf::Packet packet;
										packet << "n";
										(**it).send(packet);
									}
								}
								else
								{
									// The user doesn't exist, create the entry!
									dm.insertEntry(user, pass);
									
									// Send a yes to the client
									sf::Packet packet;
									packet << "y";
									(**it).send(packet);
								}
							}
						}
					}
				}
			}
		}
	}

	std::cout << "Ending thread" << std::endl;
}

void NetworkManager::run()
{
	// If we press the escape key, end the simulation
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
	{
		done = true;
	}
}

bool NetworkManager::isProgramDone()
{
	return done;
}