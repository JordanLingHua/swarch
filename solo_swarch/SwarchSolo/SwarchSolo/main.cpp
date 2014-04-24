#include <SFML/Graphics.hpp>
#include <list>
#include <array>

int main()
{
    //Set up like a unity c# file as much as possible. 
	//Avoid making header files to call things IF POSSIBLE.  
	//May need it for:  ..

	//Initialize.  Can change in loop.
	
	// Preset the window size
	int windowSizeX = 500;
	int windowSizeY = 500;

	// Set the text box sizes
	int textBoxX = 300;
	int textBoxY = 50;

	// Set the size of the initial swarch
	int myBoxSizeX = 10;
	int myBoxSizeY = 10;
	//int myBoxSize = 10;//comment out.  Just send only myBoxSizeX
	//since myBoxSizeX is == myBoxSizeY.  When receive, 
	//give myBoxSizeX and myBoxSizeY the same value packet (whatever it's called)

// Create GUI for login
	sf::Font font;
	// We must load the font as SFML doesn't contain a default font anymore
	font.loadFromFile("arial.ttf");
	// Create strings and sf::Texts to enable text to be written on the window
	std::string user, password;
	sf::Text userT, passwordT;

	// Set the font, set the origin and set the color
	userT.setFont(font);
	passwordT.setFont(font);
	userT.setOrigin(sf::Vector2f(0, userT.getLocalBounds().height/2));
	passwordT.setOrigin(sf::Vector2f(0, passwordT.getLocalBounds().height/2));
	userT.setColor(sf::Color::Black);
	passwordT.setColor(sf::Color::Black);

	// Create GUI for actual game
	sf::Text userName;
	userName.setColor(sf::Color::Yellow);
	userName.setString("ErrorName");
	userName.setFont(font);

	// Create the window
	sf::RenderWindow window(sf::VideoMode(windowSizeX, windowSizeY), "Swarch", sf::Style::Default^sf::Style::Resize);
	sf::RectangleShape myBox(sf::Vector2f(myBoxSizeX,myBoxSizeY));
	myBox.setFillColor(sf::Color::Cyan);

	// These texts are for the UI, "Username: ", "Password: ", and "Swarch" respectively
	sf::Text userTitle, passTitle, gameTitle;
	userTitle.setFont(font);
	passTitle.setFont(font);
	gameTitle.setFont(font);
	userTitle.setString("Username: ");
	passTitle.setString("Password: ");
	gameTitle.setString("Swarch");
	userTitle.setOrigin(sf::Vector2f(0, userTitle.getLocalBounds().height/2));
	passTitle.setOrigin(sf::Vector2f(0, passTitle.getLocalBounds().height/2));
	gameTitle.setOrigin(sf::Vector2f(gameTitle.getLocalBounds().width/2, gameTitle.getLocalBounds().height/2));
	userTitle.setPosition(sf::Vector2f(20, 200));
	passTitle.setPosition(sf::Vector2f(20, 300));
	gameTitle.setPosition(sf::Vector2f(windowSizeX/2, 100));


	// These are the boxes our players will enter info into
	sf::RectangleShape userBox, passBox;
	userBox.setSize(sf::Vector2f(textBoxX, textBoxY));
	passBox.setSize(sf::Vector2f(textBoxX, textBoxY));
	userBox.setOrigin(sf::Vector2f(0, userBox.getLocalBounds().height/2));
	passBox.setOrigin(sf::Vector2f(0, passBox.getLocalBounds().height/2));
	userBox.setPosition(sf::Vector2f(userTitle.getGlobalBounds().width + 20, 210));
	passBox.setPosition(sf::Vector2f(passTitle.getGlobalBounds().width + 20, 310));

	// Set the position of the text on top of each box
	userT.setPosition(userBox.getGlobalBounds().left + 5, userBox.getGlobalBounds().top);
	passwordT.setPosition(passBox.getGlobalBounds().left + 5, passBox.getGlobalBounds().top);

	// Create the "enter" button and set its position
	sf::Texture texture;
	texture.loadFromFile("swarchEnter.png");
	sf::Sprite enterButton;
	enterButton.setTexture(texture);
	enterButton.setOrigin(sf::Vector2f(enterButton.getLocalBounds().width/2, enterButton.getLocalBounds().height/2));
	enterButton.setPosition(400 ,passBox.getGlobalBounds().top + passBox.getLocalBounds().height/2 + 80);

	// Wait for the user to enter a name and password
	bool nameAndPasswordValid = false;
	bool enterUser = true, enterPass = false;
	while(window.isOpen() && !nameAndPasswordValid)
	{
		// Set the text to whatever the user enters
		userT.setString(user);
		passwordT.setString(password);

		sf::Event event;
		while(window.pollEvent(event))
		{
			// If the user tries to close the window, end the simulation
			if(event.type == sf::Event::Closed)
			{
					window.close();
			}
			// If the user presses a key
			else if(event.type == sf::Event::KeyPressed)
			{
				// Was the key Return?
				if(event.key.code == sf::Keyboard::Return)
				{
					// We must check to see if the password and name are valid, but for now this will suffice
					nameAndPasswordValid = true;
					userName.setString(user);
				}
				// Was the key BackSpace?
				else if(event.key.code == sf::Keyboard::BackSpace)
				{
					// If user selected, delete from user
					if(enterUser)
						user = user.substr(0, user.size() - 1);
					// If password selected, delete from password
					else if(enterPass)
						password = password.substr(0, password.size() - 1);
				}
			}
			// Enter the text
			else if(event.type == sf::Event::TextEntered)
			{
				// If the text isn't a backspace, return or delete key
				if(event.text.unicode != 8 && event.text.unicode != 15 && event.text.unicode != 127)
				{
					if(enterUser)
						user += event.text.unicode;
					else if(enterPass)
						password += event.text.unicode;
				}
			}
			// If the mouse is pressed, select the box pressed on
			else if(event.type == sf::Event::MouseButtonPressed)
			{
				sf::Vector2i mousePos = sf::Mouse::getPosition(window);

				if(userBox.getGlobalBounds().contains(mousePos.x, mousePos.y))
				{
					enterUser = true;
					enterPass = false;
				}
				else if(passBox.getGlobalBounds().contains(mousePos.x, mousePos.y))
				{
					enterUser = false;
					enterPass = true;
				}
				else if(enterButton.getGlobalBounds().contains(mousePos.x, mousePos.y))
				{
					// We must check to see if the password and name are valid, but for now this will suffice
					nameAndPasswordValid = true;
					userName.setString(user);
				}
			}
		}
		
		// Empty the window
		window.clear(sf::Color(0, 0, 64));

		// Draw the Login GUI
		window.draw(userTitle);
		window.draw(passTitle);
		window.draw(gameTitle);
		window.draw(userBox);
		window.draw(passBox);
		// Draw the user text
		window.draw(userT);
		window.draw(passwordT);
		window.draw(enterButton);
		window.display();
	}

	// Preset values for the initial game
	float mySpeed = 200.f;
	float delay = 0;
	int randPelletLocX;
	int randPelletLocY;

	float dx = 0, dy = 1;

	int numOfPellets = 4;

	// Create an array of pellets
	sf::RectangleShape* pelletList = new sf::RectangleShape[numOfPellets];

	// Fill the array will "pellets"
	for(int i = 0; i < numOfPellets; i++)
	{
		pelletList[i] =  sf::RectangleShape(sf::Vector2f(5,5));
		pelletList[i].setFillColor(sf::Color::White);

		// Choose a random location
        randPelletLocX = 1+rand()%(windowSizeX);
	    randPelletLocY = 1+rand()%(windowSizeY);

		pelletList[i].setPosition(randPelletLocX, randPelletLocY);
	}

	//Game loop (actual main)

	// Start the delta timer
	sf::Clock deltaTimer;
	sf::Time lastFrameTime;

	// While the game is being played
    while (window.isOpen())
    {
		// The delta time is the time between frames
		// Delta time ensures that actions are consistent between users
		float deltaTime = (deltaTimer.getElapsedTime() - lastFrameTime).asSeconds();
		lastFrameTime = deltaTimer.getElapsedTime();

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }


		//ALL OBJECT DRAWING GOES BETWEEN 
		//window.clear() and window.display().  All the logic 
		//comes after.  just make sure that window.clear()
		//and window.display() is called before the logic part
		//of this while(window.isOpen() loop.  
        window.clear(sf::Color(0, 0, 64));//called each frame

			window.draw(userName);
		    window.draw(myBox);
			for(int i = 0; i < numOfPellets; i++)
			{
				window.draw(pelletList[i]);
			}

        window.display();//called each frame


		//Game Logic start here:  
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		{
			//myBox.move(0, -mySpeed*deltaTime);
			dx = 0, dy = -1;
		}
		else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
		{
			//myBox.move(0, mySpeed*deltaTime);
			dx = 0, dy = 1;
		}
		else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		{
			//myBox.move(mySpeed*deltaTime, 0);
			dx = 1, dy = 0;
		}
		else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		{
			//myBox.move(-mySpeed*deltaTime, 0);
			dx = -1, dy = 0;
		}

		// Check if collides with a pellet
		for(int pelletCount = 0; pelletCount < numOfPellets; pelletCount++)
		{
			if(myBox.getGlobalBounds().intersects(pelletList[pelletCount].getGlobalBounds()))
			{
				// "Delete" the pellet and make a new one
				randPelletLocX = 1+rand()%(windowSizeX);
				randPelletLocY = 1+rand()%(windowSizeY);
				pelletList[pelletCount].setPosition(randPelletLocX, randPelletLocY);

				// Increase the player size
				myBox.setSize(sf::Vector2f(myBox.getSize().x + 2, myBox.getSize().y+2));
			}
		}

		// Check if I collide with a wall
		if(myBox.getGlobalBounds().top < 0 || myBox.getGlobalBounds().top + myBox.getGlobalBounds().height > windowSizeY ||
			myBox.getGlobalBounds().left < 0 || myBox.getGlobalBounds().left + myBox.getGlobalBounds().width > windowSizeX)
		{
			// Resize the player and set them to the center of the screen
			myBox.setSize(sf::Vector2f(10,10));
			myBox.setPosition(sf::Vector2f(windowSizeX/2 - myBox.getLocalBounds().width/2, windowSizeY/2 - myBox.getLocalBounds().height/2));
			delay = 0;
		}
	
		// Not a good delay as speeds may vary between computers
		// We are using the TA's implementation
		delay -= 4;

		if(delay <= 0)
		{
			myBox.move(dx*mySpeed*deltaTime, dy*mySpeed*deltaTime);
			delay = (myBox.getLocalBounds().width - 10) / 2;
		}
    }
	// Free any allocated resources
	delete[] pelletList;

    return 0;
}

//Go here only if appropriate.  
//Type ideas at place to put code in mind instead


//Ideas:




//Ideas to put inside initializer:  







//Ideas to put in While Loop:  

//while(numPellets < 4) 
		//spawnPellet(pellet, randomLocation);
		//numPellets++;

		//if(playerCollidesWithPellet)
		//player.size.width and length += 2
		//player.Speed -= ...
		//numPellets--
