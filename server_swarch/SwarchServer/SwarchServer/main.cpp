#include "NetworkManager.h"
#include <list>

int main()
{
	std::list<sf::Thread*> threadList;

	NetworkManager netMan;

	sf::Thread acceptThread(&NetworkManager::userJoin, &netMan);
	sf::Thread readThread(&NetworkManager::readFromUsers, &netMan);
	acceptThread.launch();
	readThread.launch();
	threadList.push_back(&acceptThread);
	threadList.push_back(&readThread);

	while(!netMan.isProgramDone())
	{
		netMan.run();
	}

   system("pause");
    return 0;
}