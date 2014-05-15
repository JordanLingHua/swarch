#include "Player.h"
#include <iostream>

Player::Player(sf::TcpSocket* socket, int number)
	:startingPos(500.f,500.f), socket(socket), isPlayerDisconnected(false), read(&Player::readFromClient, this), 
	write(&Player::writeToClient, this), score(0), playerNum(number), isReadDone(false), isWriteDone(false)
{
	body.setSize(sf::Vector2f(INITIAL_SIZE, INITIAL_SIZE));
	body.setOrigin(body.getLocalBounds().width/2, body.getLocalBounds().height/2);
	body.setPosition(startingPos);

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