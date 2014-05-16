#ifndef TCPTHREADER_H
#define TCPTHREADER_H

#include <SFML/Network.hpp>
#include <queue>

class TcpThreader
{
public:
	TcpThreader(sf::TcpSocket* sock);
	~TcpThreader(void);

	void readFromServer();
	void writeToServer();
	
	void disconnect();

	sf::TcpSocket* socket;
	std::queue<sf::Packet> readQueue, writeQueue;
	sf::Mutex readLock, writeLock;
	bool isConnected;
private:
	sf::Thread read, write;
	bool readDone, writeDone;
};

#endif // TCPTHREADER_H