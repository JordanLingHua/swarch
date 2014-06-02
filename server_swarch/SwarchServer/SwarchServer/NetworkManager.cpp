#include "NetworkManager.h"
#include "JSON.h"

NetworkManager::NetworkManager(void)
	:done(false), clientCount(0), webServerPort(1000)
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

	srand (time(NULL));

	//needed for game logic
	pelletList = new sf::RectangleShape[NUM_PELLETS];
	
	for(int i = 0; i < NUM_PELLETS; i++)
	{
		pelletList[i] = sf::RectangleShape(sf::Vector2f(5,5));

		randPelletLocX = 1+rand()%(WINDOWSIZEX);
		randPelletLocY = 1+rand()%(WINDOWSIZEY);

		pelletList[i].setOrigin(2.5f, 2.5f);
		pelletList[i].setPosition(randPelletLocX, randPelletLocY);
	}

	webServerIP = sf::IpAddress("127.0.0.1");
}


NetworkManager::~NetworkManager(void)
{
	done = true;

	for(auto it = clientList.begin(); it != clientList.end(); it++)
	{
		// Save info into database and web server
		dm.updateEntry((**it).playerName, (**it).score);
		std::wstring nameEntered((**it).playerName.begin(), (**it).playerName.end());
		sendUpdatePlayer(nameEntered.c_str(), (float)(**it).score);

		delete *it;
	}
	
	// A one time use, send all the info to our webserver
/*	std::list<PlayerScore> scoreList = dm.updateWebServer();
	for(auto iter = scoreList.begin(); iter != scoreList.end(); iter++)
	{
		std::wstring userName((*iter).name.begin(), (*iter).name.end());
		sendAddPlayer(userName.c_str(), (float)(*iter).score);
	}*/

	sendSaveInfo();

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
	// If we press the escape key, end the simulation
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
	{
		done = true;
	}

	processInput();
	gameProcess(deltaTime);
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
			// Save info into database and web server
			dm.updateEntry((**it).playerName, (**it).score);
			std::wstring nameEntered((**it).playerName.begin(), (**it).playerName.end());
			sendUpdatePlayer(nameEntered.c_str(), (float)(**it).score);
			sendSaveInfo();

			// Remove the player
			selector.remove(*(**it).socket);
			auto itToErase = it;
			it++;
			(*itToErase)->endPlayer();
			sendPlayerDisconnect((*itToErase)->playerNum);
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

							(**it).setName(user);
							sendSetup(*it);
							sendPlayerJoin((**it).playerNum, (**it).body.getPosition().x, (**it).body.getPosition().y, user);	
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
						std::wstring nameEntered(user.begin(), user.end());
						sendAddPlayer(nameEntered.c_str(), 0.0f);
						sendSaveInfo();

						// Send a yes to the client
						packet << "y" << (**it).playerNum;
						
						(**it).writeLock.lock();
						(**it).writeQueue.push(packet);
						(**it).writeLock.unlock();

						(**it).setName(user);	
						sendSetup(*it);
						sendPlayerJoin((**it).playerNum, (**it).body.getPosition().x, (**it).body.getPosition().y, user);			
					}
				}
				// If it's a directional code
				else if(code == DIRECTION_COMMAND)
				{
					int clientNum;
					float posX, posY;
					pkt >> clientNum >> (**it).dirX >> (**it).dirY >> posX >> posY;

					(**it).body.setPosition(posX, posY);

					// Send the direction to the other clients
					sendDirection(clientNum, (**it).dirX, (**it).dirY, (**it).body.getPosition().x, (**it).body.getPosition().y);
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
		//std::cout << (**it).body.getGlobalBounds().top << " " << (**it).body.getLocalBounds().height << " " << (**it).body.getGlobalBounds().left << " " << (**it).body.getLocalBounds().width << std::endl;

		float delay, xMove, yMove;
		delay = ((**it).body.getLocalBounds().width - 10.0f);
		if(delay >= SPEED)
		{
			delay = 98.f;
		}
		
		xMove = (**it).dirX*(SPEED - delay)*deltaTime;
		yMove = (**it).dirY*(SPEED - delay)*deltaTime;



		//move the player's body.  It's movement directions are already getting sent in processInputs
		(**it).body.move(xMove, yMove);

		//Player collision on player
		for(auto iter = clientList.begin(); iter != clientList.end(); iter++)
		{
			if((**it).playerNum != (**iter).playerNum && (**it).body.getGlobalBounds().intersects((**iter).body.getGlobalBounds()))
			{
				if((**it).body.getLocalBounds().width > (**iter).body.getLocalBounds().width )
				{
					(**it).body.setSize(sf::Vector2f((**it).body.getSize().x +(**iter).body.getSize().x, (**it).body.getSize().y +(**iter).body.getSize().y));
					(**it).score += 10;

					(**iter).body.setSize(sf::Vector2f(INITIAL_SIZE, INITIAL_SIZE));
					//(**iter).body.setPosition(sf::Vector2f(WINDOWSIZEX/2 - (**iter).body.getLocalBounds().width/2, WINDOWSIZEY/2 - (**iter).body.getLocalBounds().height/2));
					(**iter).body.setPosition(sf::Vector2f(1+rand()%(WINDOWSIZEX),1+rand()%(WINDOWSIZEY)));
					
					//update win and lose condition
					if((**iter).numLives > 0)
					{
						(**iter).numLives--;
					}
					if((**iter).numLives <= 0)
					{
						(**iter).lost = true;
					}


					sendPlayerEaten((**it).playerNum, (**it).body.getSize().x, 
						(**iter).score, (**iter).playerNum, (**iter).body.getPosition().x, 
						(**iter).body.getPosition().y);

					std::wstring nameEntered((**it).playerName.begin(), (**it).playerName.end());
					sendUpdatePlayer(nameEntered.c_str(), (float)(**it).score);
					sendSaveInfo();
				}
				else if((**it).body.getLocalBounds().width < (**iter).body.getLocalBounds().width )
				{
					(**iter).body.setSize(sf::Vector2f((**it).body.getSize().x +(**iter).body.getSize().x, (**it).body.getSize().y +(**iter).body.getSize().y));
					(**iter).score += 10;

					(**it).body.setSize(sf::Vector2f(INITIAL_SIZE, INITIAL_SIZE));
					//(**it).body.setPosition(sf::Vector2f(WINDOWSIZEX/2 - (**it).body.getLocalBounds().width/2, WINDOWSIZEY/2 - (**it).body.getLocalBounds().height/2));
					(**it).body.setPosition(sf::Vector2f(1+rand()%(WINDOWSIZEX),1+rand()%(WINDOWSIZEY)));

					sendPlayerEaten((**iter).playerNum, (**iter).body.getLocalBounds().width, 
						(**it).score, (**it).playerNum, (**it).body.getPosition().x, 
						(**it).body.getPosition().y);

					std::wstring nameEntered((**iter).playerName.begin(), (**iter).playerName.end());
					sendUpdatePlayer(nameEntered.c_str(), (float)(**iter).score);
					sendSaveInfo();

					//update win and lose condition
					if((**it).numLives > 0)
					{
						(**it).numLives--;
					}
					if((**it).numLives <= 0)
					{
						(**it).lost = true;
					}
				}
				else
				{
					(**iter).body.setSize(sf::Vector2f(INITIAL_SIZE, INITIAL_SIZE));
					//(**iter).body.setPosition(sf::Vector2f(WINDOWSIZEX/2 - (**iter).body.getLocalBounds().width/2, WINDOWSIZEY/2 - (**iter).body.getLocalBounds().height/2));
					(**iter).body.setPosition(sf::Vector2f(1+rand()%(WINDOWSIZEX),1+rand()%(WINDOWSIZEY)));

					(**it).body.setSize(sf::Vector2f(INITIAL_SIZE, INITIAL_SIZE));
					//(**it).body.setPosition(sf::Vector2f(WINDOWSIZEX/2 - (**it).body.getLocalBounds().width/2, WINDOWSIZEY/2 - (**it).body.getLocalBounds().height/2));
					(**it).body.setPosition(sf::Vector2f(1+rand()%(WINDOWSIZEX),1+rand()%(WINDOWSIZEY)));

					sendPlayerDeath((**it).playerNum, (**it).body.getPosition().x, (**it).body.getPosition().y);
					sendPlayerDeath((**iter).playerNum, (**iter).body.getPosition().x, (**iter).body.getPosition().y);

					//update win and lose condition
					if((**iter).numLives > 0)
					{
						(**iter).numLives--;
					}
					if((**iter).numLives <= 0)
					{
						(**iter).lost = true;
					}

					if((**it).numLives > 0)
					{
						(**it).numLives--;
					}
					if((**it).numLives <= 0)
					{
						(**it).lost = true;
					}
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
				(**it).score += 1;

				sendPelletEaten(pelletCount, randPelletLocX, randPelletLocY, (**it).playerNum, (**it).body.getSize().x);
				std::cout << "Client " << (**it).socket->getRemoteAddress().toString() << " has collided with and ate a pellet!" << std::endl;

				std::wstring nameEntered((**it).playerName.begin(), (**it).playerName.end());
				sendUpdatePlayer(nameEntered.c_str(), (float)(**it).score);
				sendSaveInfo();
			}
		}

		if((**it).body.getGlobalBounds().top < 0.0f || (**it).body.getGlobalBounds().top +(**it).body.getGlobalBounds().height > (float)WINDOWSIZEY ||
		(**it).body.getGlobalBounds().left < 0.0f || (**it).body.getGlobalBounds().left + (**it).body.getGlobalBounds().width > (float)WINDOWSIZEX)
		{
			// Resize the player and set them to the center of the screen
			(**it).body.setSize(sf::Vector2f(INITIAL_SIZE, INITIAL_SIZE));
			//(**it).body.setPosition(sf::Vector2f(WINDOWSIZEX/2 - (**it).body.getLocalBounds().width/2, WINDOWSIZEY/2 - (**it).body.getLocalBounds().height/2));
			(**it).setRandomPosition();
			//delay = 0.0f;

			sendPlayerDeath((**it).playerNum, (**it).body.getPosition().x, (**it).body.getPosition().y);

			std::cout << "Client " << (**it).socket->getRemoteAddress().toString() << " has hit a wall!" << std::endl;

			
			//update win and lose condition
			if((**it).numLives > 0)
			{
				(**it).numLives--;
			}
			if((**it).numLives <= 0)
			{
				(**it).lost = true;
			}

		}
	}


	//If one of the players is the remaining player that does not have a lose condition, 
	//that player has won!
	int numClients = clientList.size();
	int numLostClients = 0;
	for(auto iter2 = clientList.begin(); iter2 != clientList.end(); iter2++)
	{
		if((**iter2).numLives <= 0 && (**iter2).lost == true)
		{
			numLostClients++;
		}
	}

	//If all but one client lost, search for that client that won
	if(numLostClients >= numClients - 1 && numClients > 1)
	{
		done = true;

		for(auto iter3 = clientList.begin(); iter3 != clientList.end(); iter3++)
		{
			if((**iter3).lost == false )
			{
				//(**iter2).won = true;
				sf::Packet pkt;
				pkt << WIN_COMMAND << (**iter3).playerName;

				for(auto iter4 = clientList.begin(); iter4 != clientList.end(); iter4++)
				{
				(**iter4).writeLock.lock();
				(**iter4).writeQueue.push(pkt);
				(**iter4).writeLock.unlock();
				}
			}
		}
	}
}

void NetworkManager::sendSetup(Player* p)
{
	sf::Packet pkt;
	pkt << INITIALIZATION_CODE;
	pkt << p->body.getPosition().x << p->body.getPosition().y;

	// Add in all of the pellet data
	for(int i = 0; i < NUM_PELLETS; i++)
	{
		pkt << pelletList[i].getPosition().x << pelletList[i].getPosition().y;	
	}
	// Say how many players are in the game
	pkt << clientList.size() - 1;
	// Add in all of the player data
	for(auto it = clientList.begin(); it!= clientList.end(); it++)
	{
		if((**it).playerNum != p->playerNum)
			pkt << (**it).playerNum << (**it).body.getPosition().x << (**it).body.getPosition().y << (**it).dirX << (**it).dirY << (**it).body.getLocalBounds().width << (**it).score << (**it).playerName;
	}

	p->writeLock.lock();
	p->writeQueue.push(pkt);
	p->writeLock.unlock();
}

void NetworkManager::sendPlayerJoin(int clientNum, float posX, float posY, std::string name)
{
	for(auto it = clientList.begin(); it != clientList.end(); it++)
	{
		if((**it).playerNum != clientNum)
		{
			sf::Packet pkt;
			pkt << PLAYER_JOIN << clientNum << posX << posY << name;

			(**it).writeLock.lock();
			(**it).writeQueue.push(pkt);
			(**it).writeLock.unlock();
		}
	}
}

void NetworkManager::sendPlayerDisconnect(int clientNum)
{
	for(auto it = clientList.begin(); it != clientList.end(); it++)
	{
		if((**it).playerNum != clientNum)
		{
			sf::Packet pkt;
			pkt << PLAYER_DISCONNECT << clientNum;

			(**it).writeLock.lock();
			(**it).writeQueue.push(pkt);
			(**it).writeLock.unlock();
		}
	}
}

void NetworkManager::sendPlayerEaten(int hunter, float width, int score, int prey, float posX, float posY)
{
	for(auto it = clientList.begin(); it != clientList.end(); it++)
	{
		sf::Packet pkt;

		pkt << PLAYER_EATEN << hunter << width << score << prey << posX << posY;

		(**it).writeLock.lock();
		(**it).writeQueue.push(pkt);
		(**it).writeLock.unlock();
	}
}

void NetworkManager::sendPlayerDeath(int clientNum, float posX, float posY)
{
	for(auto it = clientList.begin(); it != clientList.end(); it++)
	{
		sf::Packet pkt;

		pkt << PLAYER_DEATH << clientNum << posX << posY;

		(**it).writeLock.lock();
		(**it).writeQueue.push(pkt);
		(**it).writeLock.unlock();
	}
}

void NetworkManager::sendPelletEaten(int pelletNum, float posX, float posY, int clientNum, float width)
{
	for(auto it = clientList.begin(); it != clientList.end(); it++)
	{
		sf::Packet pkt;

		pkt << PELLET_EATEN << pelletNum << posX << posY << clientNum << width;

		(**it).writeLock.lock();
		(**it).writeQueue.push(pkt);
		(**it).writeLock.unlock();
	}
}

void NetworkManager::sendDirection(int clientNum, float dirX, float dirY, float posX, float posY)
{
	for(auto iter = clientList.begin(); iter != clientList.end(); iter++)
	{
		if((**iter).playerNum != clientNum)
		{
			sf::Packet packet;
			packet << DIRECTION_COMMAND << clientNum << dirX << dirY << posX << posY;

			(**iter).writeLock.lock();
			(**iter).writeQueue.push(packet);
			(**iter).writeLock.unlock();
		}
	}
}

// Sends info for a new player to the web server upon creation, so the web server can state who currently plays the game
void NetworkManager::sendAddPlayer(const wchar_t * name, const float score)
{
	// We create a JSON type object so our web server can parse the data in Javascript
	// The AddScore JSONObject holds an action, name and score
	JSONObject data;
	data[L"action"] = new JSONValue(L"AddScore");
	data[L"name"] = new JSONValue(name);
	data[L"score"] = new JSONValue(score);

	// We calculate the new JSON object from the JSONObject we just constructed, preparing it for use in Javascript
	JSONValue* val = new JSONValue(data);
	data.clear();

	// We transform the JSONValue into a string so we can send it across the network
	std::wstring dataString = val->Stringify();
	delete val;

	// In order to send the value over the network, we convert it into a normal string
	std::string notSoWide;
	notSoWide.assign(dataString.begin(), dataString.end());
	
	// We construct our packet and feed the new JSON string into the packet
	sf::Packet packet;
	packet.append(notSoWide.c_str(), notSoWide.length());

	// We send the packet across the server, which may or may not make it
	// Although webSocket is a UdpSocket, it doesn't connect to any server
	// webSocket attempts to send any data
	webSocket.send(packet, webServerIP, webServerPort);
}

// Sends info to the web server regarding all scores received by each player
void NetworkManager::sendUpdatePlayer(const wchar_t * name, const float score)
{
	// We create a JSON type object so our web server can parse the data in Javascript
	// The UpdateScore JSONObject holds an action, name and score
	JSONObject data;
	data[L"action"] = new JSONValue(L"UpdateScore");
	data[L"name"] = new JSONValue(name);
	data[L"score"] = new JSONValue(score);

	// We calculate the new JSON object from the JSONObject we just constructed, preparing it for use in Javascript
	JSONValue* val = new JSONValue(data);
	data.clear();

	// We transform the JSONValue into a string so we can send it across the network
	std::wstring dataString = val->Stringify();
	delete val;

	// In order to send the value over the network, we convert it into a normal string
	std::string notSoWide;
	notSoWide.assign(dataString.begin(), dataString.end());

	// We construct our packet and feed the new JSON string into the packet
	sf::Packet packet;
	packet.append(notSoWide.c_str(), notSoWide.length());

	// We send the packet across the server, which may or may not make it
	// Although webSocket is a UdpSocket, it doesn't connect to any server
	// webSocket attempts to send any data
	webSocket.send(packet, webServerIP, webServerPort);
}

// Tells the server to save the info to a text file
void NetworkManager::sendSaveInfo()
{
	// We create a JSON type object so our web server can parse the data in Javascript
	// The SaveScores JSONObject holds only an action
	JSONObject data;
	data[L"action"] = new JSONValue(L"SaveScores");

	// We calculate the new JSON object from the JSONObject we just constructed, preparing it for use in Javascript
	JSONValue* val = new JSONValue(data);
	data.clear();

	// We transform the JSONValue into a string so we can send it across the network
	std::wstring dataString = val->Stringify();
	delete val;

	// In order to send the value over the network, we convert it into a normal string
	std::string notSoWide;
	notSoWide.assign(dataString.begin(), dataString.end());

	// We construct our packet and feed the new JSON string into the packet
	sf::Packet packet;
	packet.append(notSoWide.c_str(), notSoWide.length());

	// We send the packet across the server, which may or may not make it
	// Although webSocket is a UdpSocket, it doesn't connect to any server
	// webSocket attempts to send any data
	webSocket.send(packet, webServerIP, webServerPort);
}