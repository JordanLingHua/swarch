#include "GameScene.h"


//This code is called twice in LoginScene.cpp to enable GameScene object to be assigned in main.cpp
/*
//Login scene fulfills its role.  changeToGame() function called to update the flag so that the 
//Scene* object in main.cpp can be re-assigned to a GameScene object!
			userObject->changeToGame();
*/

GameScene::GameScene(std::string username)
	:delay(0), dx(0), dy(1), numOfPellets(4), upPress(false), downPress(false), rightPress(false), leftPress(false)
{
	// We must load a font as SFML doesn't provide a default font to use
	font.loadFromFile("arial.ttf");

	// Preset the username
	userName.setColor(sf::Color::Yellow);
	userName.setString("ErrorName");
	userName.setFont(font);
	userName.setString(username);

	// Create the player icon
	myBox.setFillColor(sf::Color::Cyan);
	myBox.setSize(sf::Vector2f(PLAYERSIZE,PLAYERSIZE));
	myBox.setOrigin(myBox.getLocalBounds().width/2, myBox.getLocalBounds().height/2);

	// Construct the pelletList
	pelletList = new sf::RectangleShape[numOfPellets];

	// Populate the pelletList with pellets
	for(int i = 0; i < numOfPellets; i++)
	{
		pelletList[i] =  sf::RectangleShape(sf::Vector2f(5,5));
		pelletList[i].setFillColor(sf::Color::White);
	}
}


GameScene::~GameScene(void)
{
	// Delete the pelletList to remove memory losses
	delete[] pelletList;
}

void GameScene::update(float deltaTime, NetworkManager& netMan)
{
	// Get the input from the user to calculate direction
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && dy != -1.0f && !upPress && netMan.update)
	{
		dx = 0.0f, dy = -1.0f;
		sf::Packet pkt;
		pkt << DIRECTION_COMMAND << netMan.clientNum << dx << dy << myBox.getPosition().x << myBox.getPosition().y;

		if(netMan.connectionMade())
		{
			netMan.tcpThread->writeLock.lock();
			netMan.tcpThread->writeQueue.push(pkt);
			netMan.tcpThread->writeLock.unlock();
		}

		upPress = true;
	}
	else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && dy != 1.0f && !downPress && netMan.update)
	{
		dx = 0.0f, dy = 1.0f;
		sf::Packet pkt;
		pkt << DIRECTION_COMMAND << netMan.clientNum << dx << dy << myBox.getPosition().x << myBox.getPosition().y;

		if(netMan.connectionMade())
		{
			netMan.tcpThread->writeLock.lock();
			netMan.tcpThread->writeQueue.push(pkt);
			netMan.tcpThread->writeLock.unlock();
		}

		downPress = true;
	}
	else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && dx != 1.0f && !rightPress && netMan.update)
	{
		dx = 1.0f, dy = 0.0f;
		sf::Packet pkt;
		pkt << DIRECTION_COMMAND << netMan.clientNum << dx << dy << myBox.getPosition().x << myBox.getPosition().y;

		if(netMan.connectionMade())
		{
			netMan.tcpThread->writeLock.lock();
			netMan.tcpThread->writeQueue.push(pkt);
			netMan.tcpThread->writeLock.unlock();
		}

		rightPress = true;
	}
	else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && dx != -1.0f && !leftPress && netMan.update)
	{
		dx = -1.0f, dy = 0.0f;
		sf::Packet pkt;
		pkt << DIRECTION_COMMAND << netMan.clientNum << dx << dy << myBox.getPosition().x << myBox.getPosition().y;
		if(netMan.connectionMade())
		{
			netMan.tcpThread->writeLock.lock();
			netMan.tcpThread->writeQueue.push(pkt);
			netMan.tcpThread->writeLock.unlock();
		}
		leftPress = true;
	}

	processInput(netMan);

	float xMove, yMove;
	xMove = dx*SPEED*deltaTime;
	yMove = dy*SPEED*deltaTime;

	myBox.move(xMove, yMove);

	for(auto it = playerList.begin(); it!= playerList.end(); it++)
	{
		(*it).body.move((*it).dx*SPEED*deltaTime, (*it).dy*SPEED*deltaTime);
		//(*it).move(dx*SPEED*deltaTime, dy*SPEED*deltaTime);
	}
}

void GameScene::draw(sf::RenderWindow& window)
{
	window.draw(userName);
	// Draw the pellets
	for(int i = 0; i < numOfPellets; i++)
	{
		window.draw(pelletList[i]);
	}
	window.draw(myBox);
	// Draw the players
	for(auto it = playerList.begin(); it != playerList.end(); it++)
	{
		window.draw((*it).body);
	}
}

//void GameScene::processEvents(sf::Event& evt, sf::RenderWindow& window)
UserData GameScene::processEvents(sf::Event& evt, sf::RenderWindow& window, NetworkManager& netMan)//returns type userData b/c consequence it is needed for use in loginScene child.  optional.  Not needed here
{
	UserData user("","");
	if (evt.type == sf::Event::Closed)
		window.close();
	if(evt.type == sf::Event::KeyReleased && netMan.update)
	{
		if(evt.key.code == sf::Keyboard::Up)
		{
			upPress = false;
		}
		if(evt.key.code == sf::Keyboard::Down)
		{
			downPress = false;
		}
		if(evt.key.code == sf::Keyboard::Right)
		{
			rightPress = false;
		}
		if(evt.key.code == sf::Keyboard::Left)
		{
			leftPress = false;
		}
	}
	if(evt.type == sf::Event::LostFocus)
		netMan.update = false;
	else if(evt.type == sf::Event::GainedFocus)
		netMan.update = true;



	if(sf::Keyboard::isKeyPressed(sf::Keyboard::BackSpace))
	{
		user.changeToLogin();//Switch back to game scene as this project alone
		//Later, disconnect from server also implemented here with some easy calls!

		netMan.disconnectFromServer();
	}

	return user;//Because syntax requires this
}

// Handles all of the input received from the server
void GameScene::processInput(NetworkManager& netMan)
{
	if(netMan.connectionMade())
	{
		if(!netMan.tcpThread->readQueue.empty())
		{
			sf::Packet pkt;
			pkt = netMan.tcpThread->readQueue.front();
			netMan.tcpThread->readQueue.pop();

			int code;
			pkt >> code;

			if(code == INITIALIZATION_CODE)
			{
				float x, y;
				pkt >> x >> y;
				myBox.setPosition(x, y);

				// Add in all of the pellet data
				for(int i = 0; i < numOfPellets; i++)
				{
					float pelletX, pelletY;
					pkt >> pelletX >> pelletY;	

					pelletList[i].setPosition(pelletX, pelletY);
				}
				// Say how many players are in the game
				int clientCount;
				pkt >> clientCount;
				// Add in all of the player data
				for(int i = 0; i < clientCount; i++)
				{
					float bodyX, bodyY, dirX, dirY, width;
					int playerNum, score;

					pkt >> playerNum >> bodyX >> bodyY >> dirX >> dirY >> width >> score;

					NetworkOpponent body(score, bodyX, bodyY, playerNum);
					body.dx = dirX;
					body.dy = dirY;
					playerList.push_back(body);
				}
			}
			else if(code == DIRECTION_COMMAND)
			{
				int clientNum;
				float dirX, dirY, posX, posY;
				pkt >> clientNum >> dirX >> dirY >> posX >> posY;

				for(auto it = playerList.begin(); it!=playerList.end(); it++)
				{
					if((*it).clientNum == clientNum)
					{
						(*it).dx=dirX;
						(*it).dy=dirY;
						(*it).body.setPosition(posX, posY);
					}
				}
			}
			else if(code == PLAYER_JOIN)
			{
				int clientNum;
				float posX, posY;
				pkt >> clientNum >> posX >> posY;

				playerList.push_back(NetworkOpponent(0, posX, posY, clientNum));
			}
			else if(code == PLAYER_DISCONNECT)
			{
				int clientNum;
				pkt >> clientNum;

				for(auto it = playerList.begin(); it != playerList.end(); it++)
				{
					if((*it).clientNum == clientNum)
					{
						auto iterToErase = it;
						playerList.erase(iterToErase);
						continue;
					}
				}
			}
			else if(code == PLAYER_EATEN)
			{
				int hunter, score, prey;
				float width, posX, posY;
				pkt >> hunter >> width >> score >> prey >> posX >> posY;

				if(hunter == netMan.clientNum)
				{
					myBox.setSize(sf::Vector2f(myBox.getSize().x + width, myBox.getSize().y + width));

					for(auto it = playerList.begin(); it!=playerList.end(); it++)
					{
						if((*it).clientNum == prey)
						{
							(*it).body.setSize(sf::Vector2f(10,10));
							(*it).body.setPosition(posX, posY);
						}
					}
				}
				else
				{
					for(auto it = playerList.begin(); it!=playerList.end(); it++)
					{
						if((*it).clientNum == hunter)
						{
							(*it).body.setSize(sf::Vector2f(myBox.getSize().x + width, myBox.getSize().y + width));
							
							if(prey == netMan.clientNum)
							{
								myBox.setSize(sf::Vector2f(10,10));
								myBox.setPosition(posX, posY);
							}
						}
						else if((*it).clientNum == prey)
						{
							(*it).body.setSize(sf::Vector2f(10,10));
							(*it).body.setPosition(posX, posY);
						}
					}	
				}

			}
			else if(code == PLAYER_DEATH)
			{
				int clientNum;
				float posX, posY;
				pkt >> clientNum >> posX >> posY;

				if(clientNum == netMan.clientNum)
				{
					myBox.setSize(sf::Vector2f(10, 10));
					myBox.setPosition(posX, posY);
				}

				for(auto it=playerList.begin(); it!=playerList.end(); it++)
				{
					(*it).body.setSize(sf::Vector2f(10, 10));
					(*it).body.setPosition(posX, posY);
				}

			}
			else if(code == PELLET_EATEN)
			{
				int pelletNum, clientNum;
				float posX, posY;
				pkt >> pelletNum >> posX >> posY >> clientNum;	

				pelletList[pelletNum].setPosition(posX, posY);

				if(clientNum == netMan.clientNum)
				{
					myBox.setSize(sf::Vector2f(myBox.getSize().x +2, myBox.getSize().y + 2));
				}

				for(auto it=playerList.begin(); it!=playerList.end(); it++)
				{
					(*it).body.setSize(sf::Vector2f((*it).body.getSize().x +2, (*it).body.getSize().y +2));
				}
			}
		}
	}
}