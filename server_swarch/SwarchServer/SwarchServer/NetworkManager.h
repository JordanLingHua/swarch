#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <list>
#include <iostream>

#include "DatabaseManager.h"
#include "Player.h"

#define INPUT_COMMAND 0
#define DIRECTION_COMMAND 1

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
	void gameProcess();

	sf::TcpListener listener;
	sf::SocketSelector selector;
	std::list<Player*> clientList;
	bool done;
	int clientCount;
	DatabaseManager dm;
};

#endif // NETWORKMANAGER_H