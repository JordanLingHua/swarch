#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>
#include <list>
#include <iostream>

class NetworkManager
{
public:
	NetworkManager(void);
	~NetworkManager(void);

	void createConnectionThread();
	void networkInput();
	void run();

private:
	sf::TcpListener listener;
	sf::SocketSelector selector;
	std::list<sf::Socket*> clientList;
	std::list<sf::Thread> threadList;
	bool done;
};

#endif // NETWORKMANAGER_H