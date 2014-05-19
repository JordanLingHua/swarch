#include "NetworkManager.h"

//**Using readqueue and writequeue objects to control packet send and receive.  


NetworkManager::NetworkManager(void)
	:portNum(4682), isConnected(false), threadsCreated(false), clientNum(-1), update(true)
{
	ip = sf::IpAddress::getLocalAddress();
	//ip = sf::IpAddress("###.###.###.###");
}


NetworkManager::~NetworkManager(void)
{
	//We need both the destructor and the disconnectFromServer(), but
	//since the code was syntatically the same, we might as well do this.....
	disconnectFromServer();

}

void NetworkManager::connectToServer(std::string username, std::string password)
{
	sf::TcpSocket* socket = new sf::TcpSocket;
	if(socket->connect(ip, portNum) == sf::TcpSocket::Done)//if server acknowledges back with a done(which indicates we connected)
	{
		//socket->setBlocking(false);

		tcpThread = new TcpThreader(socket);

		sf::Packet pkt;

		pkt << USER_INFO_CODE << username << password;//when this function is called in main.cpp, send the username and password passed in
		
		tcpThread->writeLock.lock();
		tcpThread->writeQueue.push(pkt);
		tcpThread->writeLock.unlock();

		isConnected = true;
	}
	else
		delete socket;
}

void NetworkManager::disconnectFromServer()
{
	if(isConnected)
	{
		isConnected = false;

		tcpThread->disconnect();
		delete tcpThread;
		int temp = 10;
	}
}