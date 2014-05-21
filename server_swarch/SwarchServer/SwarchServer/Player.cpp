#include "Player.h"
#include <iostream>


//All of these called for each player object inside netman //delete comment if causes problems

Player::Player(sf::TcpSocket* socket, int number)
	:socket(socket), isPlayerDisconnected(false), read(&Player::readFromClient, this), 
	write(&Player::writeToClient, this), score(0), playerNum(number), isReadDone(false), isWriteDone(false),
	dirX(0.0f), dirY(1.0f), numLives(3), won(false), lost(false)
{
	body.setSize(sf::Vector2f(INITIAL_SIZE, INITIAL_SIZE));
	//body.setOrigin(body.getLocalBounds().width/2, body.getLocalBounds().height/2);

	setRandomPosition();

	read.launch();
	write.launch();
}

Player::~Player(void)
{
	isPlayerDisconnected = true;
}

void Player::readFromClient()
{
	while(!isPlayerDisconnected)
	{
		sf::Packet pkt;
		if(socket->receive(pkt) == sf::TcpSocket::Disconnected)
		{
			isPlayerDisconnected = true;
		}
		else
		{
			if(!pkt.endOfPacket())
			{
				readLock.lock();
				readQueue.push(pkt);
				readLock.unlock();
			}
		}
	}

	isReadDone = true;
	std::cout << "Reading thread done" << std::endl;
}

void Player::writeToClient()
{
	while(!isPlayerDisconnected)
	{
		if(!writeQueue.empty())
		{
			writeLock.lock();
			
			sf::Packet pkt;
			pkt = writeQueue.front();
			writeQueue.pop();
			socket->send(pkt);
			
			writeLock.unlock();
		}
	}

	isWriteDone = true;
	std::cout << "Writing thread done" << std::endl;
}

// May not work
/*void Player::setScore(int score)
{
	Player::score = score;
}*/

void Player::endPlayer()
{
	if(!isReadDone)
		read.terminate();

	if(!isWriteDone)
		write.terminate();

	delete socket;
}

void Player::setRandomPosition()
{
	body.setPosition(1+(rand()%WINDOW_SIZE), 1+(rand()%WINDOW_SIZE));
}