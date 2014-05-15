#include "NetworkManager.h"

//**Using readqueue and writequeue objects to control packet send and receive.  


NetworkManager::NetworkManager(void)
	:socket(nullptr), portNum(4682), isConnected(false), threadsCreated(false)
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

//This is not being used right now.  sending done manually for now (ex) like in connectToServer)
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
	deleteThreads = true;
}

//Called in the main loop
void NetworkManager::receiveMessagesFromServer()
{
	if(isConnected)
	{
		sf::Packet pkt;
		socket->receive(pkt);

		//if the packet isn't empty
		if(!pkt.endOfPacket())
		{
			//grab from the packet and put into our readQueue queue field for this network manager object.  
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