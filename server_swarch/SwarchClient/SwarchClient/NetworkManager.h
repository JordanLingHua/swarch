#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include <queue>
#include "TcpThreader.h"

#define USER_INFO_CODE 0

//This is just our object that reads and writes packets to the server...:D
class NetworkManager
{
public:
	NetworkManager(void);
	~NetworkManager(void);

	void connectToServer(std::string username, std::string password);
	void disconnectFromServer();
	
	bool connectionMade()
	{
		return isConnected;
	}
	bool threadsRunning()
	{
		return threadsCreated;
	}
	bool removeThreads()
	{
		return deleteThreads;
	}
	void threadsMade()
	{
		threadsCreated = true;
	}

	TcpThreader* tcpThread;
	int clientNum;

private:
	sf::IpAddress ip;
	int portNum;
	bool isConnected, threadsCreated, deleteThreads;
};

#endif // NETWORKMANAGER_H