#include "LoginScene.h"
#include "GameScene.h"

//Other stuff
#include "UserData.h"


int main()
{
	// Create the window that will be used for login and gameplay
	sf::RenderWindow window(sf::VideoMode(WINDOWSIZEX, WINDOWSIZEY), "Swarch", sf::Style::Default^sf::Style::Resize);

	// The currentScene is the scene that the game is currently running
	//Scene* currentScene = new LoginScene;
	Scene* currentScene = new GameScene;

	// Set up a timer to calculate the time between frames
	sf::Clock deltaTimer;
	sf::Time lastFrameTime;


	//Other stuff
	UserData* userStuff = new UserData("","");



	// While we are running our program
	while(window.isOpen())
	{
		// Compute the time between frames, or the delta time
		float deltaTime = (deltaTimer.getElapsedTime() - lastFrameTime).asSeconds();
		lastFrameTime = deltaTimer.getElapsedTime();

		// Check for events
		sf::Event event;
		while(window.pollEvent(event))
		{
			// Send the event to the current scene

			//currentScene->processEvents(event, window);
			//Can be called as regular void function, but we want
			//to also get the user data to process it here in this case too!
			*userStuff = currentScene->processEvents(event, window);
		}
		
		// Update the current scene
		currentScene->update(deltaTime);

		// Empty the window
		window.clear(sf::Color(0, 0, 64));

		// Draw the current scene here
		currentScene->draw(window);

		// Display the content on the screen
		window.display();
	}

	// Delete the scene upon closing
	delete currentScene;
    return 0;
}