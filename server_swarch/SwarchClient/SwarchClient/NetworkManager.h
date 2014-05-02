#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include <queue>

class NetworkManager
{
public:
	NetworkManager(void);
	~NetworkManager(void);

	void connectToServer(std::string username, std::string password);
	void sendMessagesToServer();
	void receiveMessagesFromServer();
	void disconnectFromServer();
	
	bool connectionMade()
	{
		return isConnected;
	}
	bool threadsRunning()
	{
		return threadsCreated;
	}
	void threadsMade()
	{
		threadsCreated = true;
	}

	std::queue<sf::Packet> writeQueue, readQueue;

private:
	sf::TcpSocket* socket;
	sf::IpAddress ip;
	int portNum;
	bool isConnected, threadsCreated;
};

#endif // NETWORKMANAGER_H