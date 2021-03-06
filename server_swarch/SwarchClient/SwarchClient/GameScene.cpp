#include "GameScene.h"
#include <queue>

//This code is called twice in LoginScene.cpp to enable GameScene object to be assigned in main.cpp
/*
//Login scene fulfills its role.  changeToGame() function called to update the flag so that the 
//Scene* object in main.cpp can be re-assigned to a GameScene object!
			userObject->changeToGame();
*/

GameScene::GameScene(std::string username)
	:delay(0), dx(0), dy(1), numOfPellets(4), upPress(false), downPress(false), rightPress(false), leftPress(false), playerWon(false), score(0)
{
	// We must load a font as SFML doesn't provide a default font to use
	font.loadFromFile("arial.ttf");

	name = username;

	std::stringstream first;
	first << name << " - 0";

	// Preset the username
	userName.setColor(sf::Color::Yellow);
	userName.setString("ErrorName");
	userName.setFont(font);
	userName.setString(first.str());

	winnerText.setColor(sf::Color::Green);
	winnerText.setFont(font);
	winnerText.setOrigin(winnerText.getLocalBounds().width/2, winnerText.getLocalBounds().height/2 );
	winnerText.setPosition(WINDOWSIZEX/2, WINDOWSIZEY/2);

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


	if(!playerWon)
	{
		float xMove, yMove;
		delay = (myBox.getLocalBounds().width - 10.0f);
		
		if(delay >= SPEED)
		{
			delay = 98.f;
		}

		xMove = dx*(SPEED - delay)*deltaTime;
		yMove = dy*(SPEED - delay)*deltaTime;

		myBox.move(xMove, yMove);

		for(auto it = playerList.begin(); it!= playerList.end(); it++)
		{
			(*it).delay = ((*it).body.getLocalBounds().width - 10.0f);
			
			if((*it).delay >= SPEED)
			{
				(*it).delay = 98.f;
			}
			
			(*it).body.move((*it).dx*(SPEED - (*it).delay)*deltaTime, (*it).dy*(SPEED - (*it).delay)*deltaTime);
			//(*it).move(dx*SPEED*deltaTime, dy*SPEED*deltaTime);
		}
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

	if(playerWon)
		window.draw(winnerText);
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

			netMan.tcpThread->readLock.lock();
			pkt = netMan.tcpThread->readQueue.front();
			netMan.tcpThread->readQueue.pop();
			netMan.tcpThread->readLock.unlock();

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
					std::string Oname;

					pkt >> playerNum >> bodyX >> bodyY >> dirX >> dirY >> width >> score >> Oname;

					NetworkOpponent body(score, bodyX, bodyY, playerNum, Oname, width);
					body.dx = dirX;
					body.dy = dirY;
					playerList.push_back(body);
				}

				// Initialize the scoreboard for the playre to view
				sortScores();
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
				std::string Oname;
				pkt >> clientNum >> posX >> posY >> Oname;

				playerList.push_back(NetworkOpponent(0, posX, posY, clientNum, Oname, 10));

				// Add new player into the scoreboard
				sortScores();
			}
			else if(code == PLAYER_DISCONNECT)
			{
				int clientNum;
				pkt >> clientNum;

				disconnectClient(clientNum);

				// Remove player from the scoreboard
				sortScores();
			}
			else if(code == PLAYER_EATEN)
			{
				int hunter, newScore, prey;
				float width, posX, posY;
				pkt >> hunter >> width >> newScore >> prey >> posX >> posY;

				if(hunter == netMan.clientNum)
				{
					myBox.setSize(sf::Vector2f(width,width));
					score += 10;

					for(auto it = playerList.begin(); it!=playerList.end(); it++)
					{
						if((*it).clientNum == prey)
						{
							(*it).body.setSize(sf::Vector2f(10,10));
							(*it).body.setPosition(posX, posY);
						}

						// If we are bigger than our opponent
						if((*it).body.getLocalBounds().width < myBox.getLocalBounds().width)
						{
							(*it).body.setFillColor(sf::Color::Green);
						}
					}
				}
				else if(hunter != netMan.clientNum)
				{
					for(auto it = playerList.begin(); it!=playerList.end(); it++)
					{
						if((*it).clientNum == hunter)
						{
							(*it).body.setSize(sf::Vector2f(width, width));
							(*it).score += 10;

							if(prey == netMan.clientNum)
							{
								myBox.setSize(sf::Vector2f(10,10));
								myBox.setPosition(posX, posY);
							}

							if((*it).body.getLocalBounds().width > myBox.getLocalBounds().width)
							{
								(*it).body.setFillColor(sf::Color::Red);
							}
						}
						else if((*it).clientNum == prey)
						{
							(*it).body.setSize(sf::Vector2f(10,10));
							(*it).body.setPosition(posX, posY);

							// If we are bigger than the player that _just_ died, set them to green
							if((*it).body.getLocalBounds().width < myBox.getLocalBounds().width)
							{
								(*it).body.setFillColor(sf::Color::Green);
							}
							// If we are the same size as the player that _just_ died, set them to red
							else
							{
								(*it).body.setFillColor(sf::Color::Red);
							}
						}
					}	
				}
				// Sort scores as needed
				sortScores();
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

					for(auto it = playerList.begin(); it!=playerList.end(); it++)
					{
						(*it).body.setFillColor(sf::Color::Red);
					}
				}
				// If I'm not the player that died
				else
				{
					// Find the player that died
					for(auto it=playerList.begin(); it!=playerList.end(); it++)
					{
						if((*it).clientNum == clientNum)
						{
							(*it).body.setSize(sf::Vector2f(10, 10));
							(*it).body.setPosition(posX, posY);

							// If they are smaller than us, set them to Green
							if((*it).body.getLocalBounds().width > myBox.getLocalBounds().width)
							{
								(*it).body.setFillColor(sf::Color::Green);
							}
						}
					}
				}

			}
			else if(code == PELLET_EATEN)
			{
				int pelletNum, clientNum;
				float posX, posY, width;
				pkt >> pelletNum >> posX >> posY >> clientNum >> width;	

				pelletList[pelletNum].setPosition(posX, posY);

				if(clientNum == netMan.clientNum)
				{
					myBox.setSize(sf::Vector2f(width, width));
					score += 1;
					//after the pellet is consumed, if any of the other players are now smaller, make them green locally on your side
					for(auto it=playerList.begin(); it!=playerList.end(); it++)
					{
						if((*it).body.getLocalBounds().width < myBox.getLocalBounds().width)
						{
							(*it).body.setFillColor(sf::Color::Green);
						}
						else if((*it).body.getLocalBounds().width == myBox.getLocalBounds().width)
						{
							(*it).body.setFillColor(sf::Color::Red);
						}
					}
				}
				else
				{
					for(auto it=playerList.begin(); it!=playerList.end(); it++)
					{
						if((*it).clientNum == clientNum)
						{
							(*it).body.setSize(sf::Vector2f(width, width));
							(*it).score += 1;

							if((*it).body.getLocalBounds().width >= myBox.getLocalBounds().width)
							{
								(*it).body.setFillColor(sf::Color::Red);
							}
							else
							{
								(*it).body.setFillColor(sf::Color::Green);
							}
						}
					}
				}
				// Sort the score if needed
				// See function below.  We just modified our stringstream with the sortScores function
				sortScores();
			}
			else if(code == PLAYER_WON && !playerWon)
			{
				std::string winner;
				pkt >> winner;

				// set a boolean saying a player won
				playerWon = true;

				// set winner sf::text to winner string
				winnerStream << winner << " has won";

				winnerText.setString(winnerStream.str());
				winnerStream.str("");
				winnerStream.clear();
			}
		}
	}
}

void GameScene::sortScores()
{
	// Create a priority queue
	std::priority_queue<PlayerScore, std::vector<PlayerScore>, ScoreSort> scoreList;
				
	// Enter my player's info
	PlayerScore myScore;
	myScore.name = name;
	myScore.score = score;

	// Save my score
	scoreList.push(myScore);

	// For each opponent player, save their score and sort them
	for(auto it = playerList.begin(); it!= playerList.end(); it++)
	{
		// Enter my player's info
		PlayerScore opponentScore;
		opponentScore.name = (*it).name;
		opponentScore.score = (*it).score;

		scoreList.push(opponentScore);
	}

	// Now that we have the scores, assemble the score board string
	std::stringstream scoreStream;

	// While we still have scores in our score list
	while(!scoreList.empty())
	{
		// Retrieve the score
		PlayerScore temp;
		temp = scoreList.top();
		scoreList.pop();

		// Save the info in our scoreStream
		// \n iterates \n to the next element to be inserted into the stringstream
		scoreStream << temp.name << ": " << temp.score << "\n";
	}

	userName.setString(scoreStream.str());
}

void GameScene::disconnectClient(int clientNum)
{
	for(auto it = playerList.begin(); it != playerList.end(); it++)
	{
		// Check if we need to remove the client
		if((*it).clientNum == clientNum)//**it's are player objects
		{
			auto itToErase = it;
			it++;
			playerList.erase(itToErase);
			return;
		}
	}
}