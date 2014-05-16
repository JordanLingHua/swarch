#include "TcpThreader.h"

TcpThreader::TcpThreader(sf::TcpSocket* sock)
	:read(&TcpThreader::readFromServer, this), write(&TcpThreader::writeToServer, this), 
	isConnected(true), readDone(false), writeDone(false)
{
	socket = sock;

	read.launch();
	write.launch();
}

TcpThreader::~TcpThreader()
{
	isConnected = false;
	if(!readDone)
		read.terminate();
	if(!writeDone)
		write.terminate();

	disconnect();

	delete socket;
}

void TcpThreader::readFromServer()
{
	while(isConnected)
	{
		sf::Packet pkt;
		socket->receive(pkt);
	
		if(!pkt.endOfPacket())
		{
			readLock.lock();
			readQueue.push(pkt);
			readLock.unlock();
		}
	}
	readDone = true;
}

void TcpThreader::writeToServer()
{
	while(isConnected)
	{
		if(!writeQueue.empty())
		{
			writeLock.lock();
			
			sf::Packet pkt;
			pkt = writeQueue.front();
			writeQueue.pop();
			socket->send(pkt);
			
			writeLock.unlock();
		}
	}
	writeDone = true;
}

void TcpThreader::disconnect()
{
	isConnected = false;

	socket->disconnect();
}