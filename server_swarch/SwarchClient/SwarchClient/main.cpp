#include "LoginScene.h"
#include "GameScene.h"

//Other stuff
#include "UserData.h"

//**In this main function, we use ONLY the original 
//Scene object so that we can simply change the scene object's functionality by
//assigning it to an inheritance variation of the Scene object!!!!

//Re-assign scene object and display it using the window commands from SFML's library

int main()
{
	// Create the window that will be used for login and gameplay
	sf::RenderWindow window(sf::VideoMode(WINDOWSIZEX, WINDOWSIZEY), "Swarch", sf::Style::Default^sf::Style::Resize);

	// The currentScene is the scene that the game is currently running
	//Scene* currentScene = new LoginScene;
	Scene* currentScene = new LoginScene;

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


		//Switching from login screen to game screen is done with the following first two if else ifs.  
		if(userStuff->startGame)//if startgame flag enabled true somewhere,
		{
			delete currentScene;
			currentScene = new GameScene(userStuff->userNameStorage);//switch to gamescene 
			//and pass the username to gamescene so that gamescene's frames its main loop will display user name
			userStuff->startGame = false;
		}
		else if(userStuff->startLogin)// if login flag enabled true somewhere,
		{
			delete currentScene;
			currentScene = new LoginScene;// switch to login scene.  **Notice how useful using a Scene inheritance is useful here, because
			//you get to switch between different variants of the scene object.  
			userStuff->startLogin = false;
		}
	}

	// Delete the scene upon closing
	delete currentScene;
	delete userStuff;
    return 0;
}