#include "NetworkManager.h"
#include <list>

int main()
{
	NetworkManager netMan;

	sf::Thread acceptThread(&NetworkManager::userJoin, &netMan);
	acceptThread.launch();

	sf::Clock deltaTimer;
	sf::Time lastFrameTime;

	while(!netMan.isProgramDone())
	{
		float deltaTime = (deltaTimer.getElapsedTime()-lastFrameTime).asSeconds();
		lastFrameTime = deltaTimer.getElapsedTime();

		netMan.run(deltaTime);
	}

   system("pause");
    return 0;
}