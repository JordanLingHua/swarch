#include "NetworkManager.h"
#include <list>

int main()
{
	//Putting networkManager code to use here //delete comment if causes problems
	NetworkManager netMan;

	//Create thread object //delete comment if causes problems
	sf::Thread acceptThread(&NetworkManager::userJoin, &netMan);
	acceptThread.launch();

	sf::Clock deltaTimer;
	sf::Time lastFrameTime;

	//while flag is not triggered //delete comment if causes problems
	while(!netMan.isProgramDone())
	{
		float deltaTime = (deltaTimer.getElapsedTime()-lastFrameTime).asSeconds();
		lastFrameTime = deltaTimer.getElapsedTime();

		//Execute networkManager's functions
		netMan.run(deltaTime);
	}

   system("pause");
    return 0;
}