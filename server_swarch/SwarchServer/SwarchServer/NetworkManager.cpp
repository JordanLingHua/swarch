#include "NetworkManager.h"


NetworkManager::NetworkManager(void)
	:done(false), clientCount(0)
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

	//needed for game logic
	pelletList = new sf::RectangleShape[NUM_PELLETS];
	
	for(int i = 0; i < NUM_PELLETS; i++)
	{
		pelletList[i] = sf::RectangleShape(sf::Vector2f(5,5));

		randPelletLocX = 1+rand()%(WINDOWSIZEX);
		randPelletLocY = 1+rand()%(WINDOWSIZEY);


		pelletList[i].setPosition(randPelletLocX, randPelletLocY);
	}



}


NetworkManager::~NetworkManager(void)
{
	done = true;

	for(auto it = clientList.begin(); it != clientList.end(); it++)
	{
		delete *it;
	}

	delete[] pelletList;
}


//Multithreading added in milestone 4 to allow multiple users join //delete comment if causes problems
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
					Player* p = new Player(client, ++clientCount);
					
					clientJoinLock.lock();
					clientList.push_back(p);
					clientJoinLock.unlock();

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

	gameProcess(deltaTime);

	processInput();
}

bool NetworkManager::isProgramDone()
{
	return done;
}

// Handles user input to the server
void NetworkManager::processInput()
{
	// Go through each client
	clientJoinLock.lock();
	
	for(auto it = clientList.begin(); it != clientList.end(); it++)
	{
		// Check if we need to remove the client
		if((**it).isPlayerDisconnected)//**it's are player objects
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
				int code;
				pkt >> code;
				
				// If a user info code
				if(code == 0)
				{
					std::string user, pass;
					pkt >> user >> pass;
					sf::Packet packet;
					packet << INPUT_COMMAND;
					if(dm.doesUserExistInDB(user))
					{
						// Check to see if the password matches
						if(dm.doesPasswordMatchUser(user, pass))
						{
							packet << "y" << (**it).playerNum;

							(**it).writeLock.lock();
							(**it).writeQueue.push(packet);
							(**it).writeLock.unlock();
						}
						else
						{
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
						packet << "y" << (**it).playerNum;
						
						(**it).writeLock.lock();
						(**it).writeQueue.push(packet);
						(**it).writeLock.unlock();
					}
				}
				// If it's a directional code
				else if(code == 1)
				{
					int clientNum;
				//	float posX, posY;
					pkt >> clientNum >> (**it).dirX >> (**it).dirY;

					std::cout << "The new direction for client " << clientNum << " is: " << (**it).dirX << " , " << (**it).dirY << std::endl;

					// Send the direction to the other clients
					/*for(auto iter = clientList.begin(); iter != clientList.end(); iter++)
					{
						if((**iter).playerNum != clientNum)
						{
							sf::Packet packet;
							packet << 1 << clientNum << (**it).dirX << (**it).dirY;

							(**iter).writeLock.lock();
							(**iter).writeQueue.push(packet);
							(**iter).writeLock.unlock();
						}
					}*/
				}
			}
		}
	}
	clientJoinLock.unlock();
}

// Handles the game logic
void NetworkManager::gameProcess(float deltaTime)
{

	for(auto it = clientList.begin(); it != clientList.end(); it++)
	{
		
		//
		//move the player's body.  It's movement directions are already getting sent in processInputs
		(**it).body.move((**it).dirX *(SPEED)* deltaTime,(**it).dirY *(SPEED)*deltaTime);


		//Player collision on player
		for(auto iter = clientList.begin(); iter != clientList.end(); iter++)
		{
			if((**it).body.getGlobalBounds().intersects((**iter).body.getGlobalBounds()))
			{

				if((**it).body.getLocalBounds().width > (**iter).body.getLocalBounds().width )
				{
					(**iter).body.setSize(sf::Vector2f(INITIAL_SIZE, INITIAL_SIZE));
					//(**iter).body.setPosition(sf::Vector2f(WINDOWSIZEX/2 - (**iter).body.getLocalBounds().width/2, WINDOWSIZEY/2 - (**iter).body.getLocalBounds().height/2));
					(**iter).body.setPosition(sf::Vector2f(1+rand()%(WINDOWSIZEX),1+rand()%(WINDOWSIZEY)));
				}
				else if((**it).body.getLocalBounds().width < (**iter).body.getLocalBounds().width )
				{
					(**it).body.setSize(sf::Vector2f(INITIAL_SIZE, INITIAL_SIZE));
					//(**it).body.setPosition(sf::Vector2f(WINDOWSIZEX/2 - (**it).body.getLocalBounds().width/2, WINDOWSIZEY/2 - (**it).body.getLocalBounds().height/2));
					(**it).body.setPosition(sf::Vector2f(1+rand()%(WINDOWSIZEX),1+rand()%(WINDOWSIZEY)));
				}
				else
				{
					(**iter).body.setSize(sf::Vector2f(INITIAL_SIZE, INITIAL_SIZE));
					//(**iter).body.setPosition(sf::Vector2f(WINDOWSIZEX/2 - (**iter).body.getLocalBounds().width/2, WINDOWSIZEY/2 - (**iter).body.getLocalBounds().height/2));
					(**iter).body.setPosition(sf::Vector2f(1+rand()%(WINDOWSIZEX),1+rand()%(WINDOWSIZEY)));

					(**it).body.setSize(sf::Vector2f(INITIAL_SIZE, INITIAL_SIZE));
					//(**it).body.setPosition(sf::Vector2f(WINDOWSIZEX/2 - (**it).body.getLocalBounds().width/2, WINDOWSIZEY/2 - (**it).body.getLocalBounds().height/2));
					(**it).body.setPosition(sf::Vector2f(1+rand()%(WINDOWSIZEX),1+rand()%(WINDOWSIZEY)));

				}


			}
		}

		
		// If my player collides with a pellet
		for(int pelletCount = 0; pelletCount < NUM_PELLETS; pelletCount++)
		{
			if((**it).body.getGlobalBounds().intersects(pelletList[pelletCount].getGlobalBounds()))
			{
				// "Delete" the pellet and make a new one
				randPelletLocX = 1+rand()%(WINDOWSIZEX);
				randPelletLocY = 1+rand()%(WINDOWSIZEY);
				pelletList[pelletCount].setPosition(randPelletLocX, randPelletLocY);
	
				// Increase the player size
				(**it).body.setSize(sf::Vector2f((**it).body.getSize().x + 2, (**it).body.getSize().y+2));
				std::cout << "Client " << (**it).socket->getRemoteAddress().toString() << " has collided with and ate a pellet!" << std::endl;

			}
		}





		if((**it).body.getGlobalBounds().top < 0 || (**it).body.getGlobalBounds().top +(**it).body.getGlobalBounds().height > WINDOWSIZEY ||
		(**it).body.getGlobalBounds().left < 0 || (**it).body.getGlobalBounds().left + (**it).body.getGlobalBounds().width > WINDOWSIZEX)
		{
			// Resize the player and set them to the center of the screen
			(**it).body.setSize(sf::Vector2f(INITIAL_SIZE, INITIAL_SIZE));
			(**it).body.setPosition(sf::Vector2f(WINDOWSIZEX/2 - (**it).body.getLocalBounds().width/2, WINDOWSIZEY/2 - (**it).body.getLocalBounds().height/2));
			//delay = 0.0f;


			std::cout << "Client " << (**it).socket->getRemoteAddress().toString() << " has hit a wall!" << std::endl;

		}


		
		
		




	}


	
}