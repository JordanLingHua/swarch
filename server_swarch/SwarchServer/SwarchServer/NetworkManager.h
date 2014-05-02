#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <list>
#include <iostream>

#include "DatabaseManager.h"

class NetworkManager
{
public:
	NetworkManager(void);
	~NetworkManager(void);

	void networkInput();
	void run();
	bool isProgramDone();

private:
	sf::TcpListener listener;
	sf::SocketSelector selector;
	std::list<sf::TcpSocket*> clientList;
	bool done;

	sf::Mutex clientLock;
	DatabaseManager dm;
};

#endif // NETWORKMANAGER_H