#ifndef PLAYER_H
#define PLAYER_H

#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <queue>

#define INITIAL_SIZE 10
#define STARTINGX 500
#define STARTINGY 500
#define SPEED 20//not worrying about speed in this commit

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

	sf::Mutex readLock, writeLock;//Everytime we read/write a certain packet,    //delete comment if causes problems
	int score, playerNum;
	float dirX, dirY;
	//int dirX, dirY;
	//dirX stores either -1, 0, or 1.  dirY stores either -1, 0, 1  <---Assume client will send these.  
private:
	sf::Vector2f startingPos;
	sf::Thread read, write;  //Player uses threads instead of sockets this time.  //We will still traverse a list of player objects in netMan   //delete comment if causes problems
	bool isReadDone, isWriteDone;
};

#endif //PLAYER_H