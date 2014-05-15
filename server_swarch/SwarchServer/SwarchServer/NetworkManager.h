#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <list>
#include <iostream>

#include "DatabaseManager.h"
#include "Player.h"

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
	//std::list<sf::TcpSocket*> clientList;
	std::list<Player*> clientList;
	bool done;

	DatabaseManager dm;
};

#endif // NETWORKMANAGER_H