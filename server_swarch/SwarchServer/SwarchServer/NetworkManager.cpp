#include "NetworkManager.h"


NetworkManager::NetworkManager(void)
	:done(false)
{
	// Choose a port to listen to
	listener.listen(4682);

	// The listener will allow us to parse messages based on whether a player is trying to join
	// or if the player is trying to send us a message
	selector.add(listener);

	// Check to see if the table exists in the database
	// If not, create a table!
	if(!dm.doesTableExistInDB())
	{
		dm.createTable();
	}
}


NetworkManager::~NetworkManager(void)
{
	done = true;

	for(auto it = clientList.begin(); it != clientList.end(); it++)
	{
		delete *it;
	}
}

void NetworkManager::userJoin()
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
					Player* p = new Player(client, 1);
					clientList.push_back(p);

					// Allow for input without threading
					client->setBlocking(false);
				}
				// Delete the client if they cannot be accepted
				else
				{
					delete client;
				}
			}
		}
	}

	std::cout << "Ending thread" << std::endl;
}

void NetworkManager::run(float deltaTime)
{
	//std::cout << "The deltaTime is: " << deltaTime << std::endl;

	// If we press the escape key, end the simulation
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
	{
		done = true;
	}

	gameProcess();

	processInput();
}

bool NetworkManager::isProgramDone()
{
	return done;
}

void NetworkManager::processInput()
{
	// Go through each client
	for(auto it = clientList.begin(); it != clientList.end(); it++)
	{
		// Check if we need to remove the client
		if((**it).isPlayerDisconnected)
		{
			selector.remove(*(**it).socket);
			auto itToErase = it;
			it++;
			(*itToErase)->endPlayer();
			delete *itToErase;
			clientList.erase(itToErase);
			std::cout << "A client has disconnected" << std::endl;
			continue;
		}
		// Process client messages
		else
		{
			// If there are messages from this client
			if(!(**it).readQueue.empty())
			{
				// Get the message
				sf::Packet pkt = (**it).readQueue.front();
				
				(**it).readLock.lock();
				(**it).readQueue.pop();
				(**it).readLock.unlock();

				// Get the code and figure out what to do with the message
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

							(**it).writeLock.lock();
							(**it).writeQueue.push(packet);
							(**it).writeLock.unlock();
						}
						else
						{
							sf::Packet packet;
							packet << "n";
							(**it).writeLock.lock();
							(**it).writeQueue.push(packet);
							(**it).writeLock.unlock();
						}
					}
					else
					{
						// The user doesn't exist, create the entry!
						dm.insertEntry(user, pass);
								
						// Send a yes to the client
						sf::Packet packet;
						packet << "y";
						
						(**it).writeLock.lock();
						(**it).writeQueue.push(packet);
						(**it).writeLock.unlock();
					}
				}
			}
		}
	}
}

void NetworkManager::gameProcess()
{

}