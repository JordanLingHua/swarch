#include "GameScene.h"


//This code is called twice in LoginScene.cpp to enable GameScene object to be assigned in main.cpp
/*
//Login scene fulfills its role.  changeToGame() function called to update the flag so that the 
//Scene* object in main.cpp can be re-assigned to a GameScene object!
			userObject->changeToGame();
*/

GameScene::GameScene(std::string username)
	:delay(0), dx(0), dy(1), numOfPellets(4)
{
	// We must load a font as SFML doesn't provide a default font to use
	font.loadFromFile("arial.ttf");

	// Preset the username
	userName.setColor(sf::Color::Yellow);
	userName.setString("ErrorName");
	userName.setFont(font);
	userName.setString(username);

	// Create the player icon
	myBox.setFillColor(sf::Color::Cyan);
	myBox.setSize(sf::Vector2f(PLAYERSIZE,PLAYERSIZE));
	myBox.setOrigin(myBox.getLocalBounds().width/2, myBox.getLocalBounds().height/2);
	myBox.setPosition(WINDOWSIZEX/2, WINDOWSIZEY/2);

	// Construct the pelletList
	pelletList = new sf::RectangleShape[numOfPellets];

	// Populate the pelletList with pellets
	for(int i = 0; i < numOfPellets; i++)
	{
		pelletList[i] =  sf::RectangleShape(sf::Vector2f(5,5));
		pelletList[i].setFillColor(sf::Color::White);

		// Choose a random location
        randPelletLocX = 1+rand()%(WINDOWSIZEX);
	    randPelletLocY = 1+rand()%(WINDOWSIZEY);

		pelletList[i].setPosition(randPelletLocX, randPelletLocY);
	}
}


GameScene::~GameScene(void)
{
	// Delete the pelletList to remove memory losses
	delete[] pelletList;
}

void GameScene::update(float deltaTime)
{
	// Get the input from the user to calculate direction
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
	{
		dx = 0, dy = -1;
	}
	else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
	{
		dx = 0, dy = 1;
	}
	else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
	{
		dx = 1, dy = 0;
	}
	else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
	{
		dx = -1, dy = 0;
	}

	// If my player collides with a pellet
	for(int pelletCount = 0; pelletCount < numOfPellets; pelletCount++)
	{
		if(myBox.getGlobalBounds().intersects(pelletList[pelletCount].getGlobalBounds()))
		{
			// "Delete" the pellet and make a new one
			randPelletLocX = 1+rand()%(WINDOWSIZEX);
			randPelletLocY = 1+rand()%(WINDOWSIZEY);
			pelletList[pelletCount].setPosition(randPelletLocX, randPelletLocY);
	
			// Increase the player size
			myBox.setSize(sf::Vector2f(myBox.getSize().x + 2, myBox.getSize().y+2));
		}
	}

	// If my player collides with a wall
	if(myBox.getGlobalBounds().top < 0 || myBox.getGlobalBounds().top + myBox.getGlobalBounds().height > WINDOWSIZEY ||
		myBox.getGlobalBounds().left < 0 || myBox.getGlobalBounds().left + myBox.getGlobalBounds().width > WINDOWSIZEX)
	{
		// Resize the player and set them to the center of the screen
		myBox.setSize(sf::Vector2f(PLAYERSIZE,PLAYERSIZE));
		myBox.setPosition(sf::Vector2f(WINDOWSIZEX/2 - myBox.getLocalBounds().width/2, WINDOWSIZEY/2 - myBox.getLocalBounds().height/2));
		delay = 0;
	}

	// Not a good delay as speeds may vary between computers
	// We are using the TA's implementation
	delay -= 4;

	if(delay <= 0)
	{
		myBox.move(dx*SPEED*deltaTime, dy*SPEED*deltaTime);
		delay = (myBox.getLocalBounds().width - 10) / 2;
	}
}

void GameScene::draw(sf::RenderWindow& window)
{
	window.draw(userName);
	window.draw(myBox);
	for(int i = 0; i < numOfPellets; i++)
	{
		window.draw(pelletList[i]);
	}
}

//void GameScene::processEvents(sf::Event& evt, sf::RenderWindow& window)
UserData GameScene::processEvents(sf::Event& evt, sf::RenderWindow& window)//returns type userData b/c consequence it is needed for use in loginScene child.  optional.  Not needed here
{
	UserData user("","");
	if (evt.type == sf::Event::Closed)
		window.close();

	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
	{
		user.changeToLogin();//Switch back to game scene as this project alone
		//Later, disconnect from server also implemented here with some easy calls!
	}

	return user;//Because syntax requires this
}