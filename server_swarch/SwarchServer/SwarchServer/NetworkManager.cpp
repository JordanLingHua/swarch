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

	for(auto it = clientList.begin(); it != clientList.end(); it++)
	{
		//disconnection doesn't need to be checked b/c like this method, processInputs is going to be called 
		//in the same loop anyways
		// If there are messages from this client
		if(!(**it).readQueue.empty())
		{

			//**Direction will ALWAYS be read and sent first before speed is read and sent
			//As of now, speed is not getting read and sent (05/14/14 5:55pm)
			// Get the message from the player's readQueue thread
				sf::Packet dirpkt = (**it).readQueue.front();
				
				(**it).readLock.lock();
				(**it).readQueue.pop();//When you pop, you are popping out a PACKET!!!
				(**it).readLock.unlock();
				
				//extract from dirpkt and assign it into (**it).dirX and (**it).dirY in that order
				if(dirpkt >> (**it).dirX >> (**it).dirY)//verify if the assignment process for packets returns true //Lots of dereferencing for dirX.  don't know if that's ok
				{

					//__send dirX first__
					if((**it).dirX < 0 || (**it).dirX == -1)
					{
						//send goLeft
						sf::Packet packet;
						//packet << -1;
						packet << -((**it).dirX);//fixed the dereferencing floating objects//Notice the re-initialization below
						(**it).writeLock.lock();
						(**it).writeQueue.push(packet);
						(**it).writeLock.unlock();
						(**it).dirX = 0;//re-initialize dirX
					}
					else if ((**it).dirX > 0 || (**it).dirX == 1)
					{
						//send goRight
						sf::Packet packet;
						//packet << 1;
						packet << (**it).dirX;//fixed the dereferencing floating objects//Notice the re-initialization below
						(**it).writeLock.lock();
						(**it).writeQueue.push(packet);
						(**it).writeLock.unlock();
						(**it).dirX = 0;//re-initialize dirX
					}
					else
					{
						//dirX is 0; send dontMoveLeftNORRight
						sf::Packet packet;
						packet << 0;
						(**it).writeLock.lock();
						(**it).writeQueue.push(packet);
						(**it).writeLock.unlock();
					}


					//__Send dirY__
					if((**it).dirY < 0 || (**it).dirY == -1)
					{
						//send goLeft
						sf::Packet packet;
						//packet << -1;
						packet << -((**it).dirY);//fixed the dereferencing floating objects//Notice the re-initialization below
						(**it).writeLock.lock();
						(**it).writeQueue.push(packet);
						(**it).writeLock.unlock();
						(**it).dirY = 0;//re-initialize dirY
					}
					else if ((**it).dirY > 0 || (**it).dirY == 1)
					{
						//send goRight
						sf::Packet packet;
						//packet << 1;
						packet << (**it).dirY;//fixed the dereferencing floating objects//Notice the re-initialization below
						(**it).writeLock.lock();
						(**it).writeQueue.push(packet);
						(**it).writeLock.unlock();
						(**it).dirX = 0;//re-initialize dirY
					}
					else
					{
						//dirY is 0; send dontMoveLeftNORRight
						sf::Packet packet;
						packet << 0;
						(**it).writeLock.lock();
						(**it).writeQueue.push(packet);
						(**it).writeLock.unlock();
					}




				}

				


		}


	}



}