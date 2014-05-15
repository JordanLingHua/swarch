#ifndef PLAYER_H
#define PLAYER_H

#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <queue>

#define INITIAL_SIZE 10
#define STARTINGX 500;
#define STARTINGY 500;

class Player
{
public:
	Player(sf::TcpSocket* socket, int number);
	~Player(void);

	void readFromClient();
	void writeToClient();

	void setScore(int score);
	void endPlayer();

	sf::RectangleShape body;
	sf::TcpSocket* socket;
	bool isPlayerDisconnected;
	std::queue<sf::Packet> readQueue, writeQueue;

	sf::Mutex readLock, writeLock;
	int score, playerNum;
	float dirX, dirY;
private:
	sf::Vector2f startingPos;
	sf::Thread read, write;
	bool isReadDone, isWriteDone;
};

#endif //PLAYER_H