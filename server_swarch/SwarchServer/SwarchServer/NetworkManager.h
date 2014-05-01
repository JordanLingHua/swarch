#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <list>
#include <iostream>

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
};

#endif // NETWORKMANAGER_H