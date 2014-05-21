#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <list>
#include <iostream>

#include "DatabaseManager.h"
#include "Player.h"

#define INITIALIZATION_CODE 20
#define CREATE_PELLET 21
#define CREATE_PLAYER 22
#define INPUT_COMMAND 0
#define DIRECTION_COMMAND 1
#define PLAYER_JOIN 2
#define PLAYER_DISCONNECT 3
#define PLAYER_EATEN 4
#define PLAYER_DEATH 5
#define PELLET_EATEN 6
#define WIN_COMMAND 7

//Game Logic
#define NUM_PELLETS 4
#define WINDOWSIZEX 500
#define WINDOWSIZEY 500

class NetworkManager
{
public:
	NetworkManager(void);
	~NetworkManager(void);

	void userJoin();
	void run(float deltaTime);
	bool isProgramDone();

	bool clientsJoined;
	sf::Mutex clientJoinLock;

private:

	void processInput();
	void gameProcess(float deltaTime);
	
	void sendDirection(int clientNum, float dirX, float dirY, float posX, float posY);
	void sendSetup(Player* p);
	void sendPlayerJoin(int clientNum, float posX, float posY);
	void sendPlayerDisconnect(int clientNum);
	void sendPlayerEaten(int hunter, float width, int score, int prey, float posX, float posY);
	void sendPlayerDeath(int clientNum, float posX, float posY);
	void sendPelletEaten(int pelletNum, float posX, float posY, int clientNum, float width);

	sf::TcpListener listener;
	sf::SocketSelector selector;
	std::list<Player*> clientList;
	bool done;
	int clientCount;
	DatabaseManager dm;

	//Game Logic
	sf::RectangleShape* pelletList;
	float randPelletLocX;
	float randPelletLocY;

};

#endif // NETWORKMANAGER_H