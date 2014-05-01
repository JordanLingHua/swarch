#include "NetworkManager.h"
#include <list>

int main()
{
	std::list<sf::Thread*> threadList;

	NetworkManager netMan;

	sf::Thread acceptThread(&NetworkManager::networkInput, &netMan);
	acceptThread.launch();
	threadList.push_back(&acceptThread);

	while(!netMan.isProgramDone())
	{
		netMan.run();
	}

   system("pause");
    return 0;
}