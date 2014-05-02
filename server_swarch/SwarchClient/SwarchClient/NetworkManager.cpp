#include "NetworkManager.h"


NetworkManager::NetworkManager(void)
	:socket(nullptr), portNum(4682), isConnected(false), threadsCreated(false)
{
	ip = sf::IpAddress::getLocalAddress();
	//ip = sf::IpAddress("###.###.###.###");
}


NetworkManager::~NetworkManager(void)
{
	if(isConnected)
	{
		isConnected = false;

		socket->disconnect();
		delete socket;
	}
}

void NetworkManager::connectToServer(std::string username, std::string password)
{
	socket = new sf::TcpSocket;
	if(socket->connect(ip, portNum) == sf::TcpSocket::Done)//if server acknowledges back with a done(which indicates we connected)
	{
		socket->setBlocking(false);

		sf::Packet pkt;

		pkt << username << password;//when this function is called in main.cpp, send the username and password passed in
		socket->send(pkt);

		isConnected = true;
	}
	else
		delete socket;
}

void NetworkManager::sendMessagesToServer()
{
	while(isConnected)
	{
		while(!writeQueue.empty())
		{
			socket->send(writeQueue.front());
			writeQueue.pop();
		}
	}
	threadsCreated = false;
}

void NetworkManager::receiveMessagesFromServer()
{
	if(isConnected)
	{
		sf::Packet pkt;
		socket->receive(pkt);

		if(!pkt.endOfPacket())
		{
			readQueue.push(pkt);
		}
	}
}

void NetworkManager::disconnectFromServer()
{
	if(isConnected)
	{
		isConnected = false;

		socket->disconnect();
		delete socket;
	}
}