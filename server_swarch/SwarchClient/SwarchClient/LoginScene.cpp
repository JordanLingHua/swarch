#include "LoginScene.h"

LoginScene::LoginScene(void)
	:enterUser(true), enterPass(false)
{
	// We must load a font as SFML doesn't provide a default font to use
	font.loadFromFile("arial.ttf");

	// Set the fonts for each sf::Text
	userT.setFont(font);
	passwordT.setFont(font);
	userTitle.setFont(font);
	passTitle.setFont(font);
	gameTitle.setFont(font);

	// Preset the origins and the colors for our text fields
	userT.setOrigin(sf::Vector2f(0, userT.getLocalBounds().height/2));
	passwordT.setOrigin(sf::Vector2f(0, passwordT.getLocalBounds().height/2));
	userT.setColor(sf::Color::Black);
	passwordT.setColor(sf::Color::Black);

	// Create the login menu GUI Text
	userTitle.setString("Username: ");
	passTitle.setString("Password: ");
	gameTitle.setString("Swarch");
	userTitle.setOrigin(sf::Vector2f(0, userTitle.getLocalBounds().height/2));
	passTitle.setOrigin(sf::Vector2f(0, passTitle.getLocalBounds().height/2));
	gameTitle.setOrigin(sf::Vector2f(gameTitle.getLocalBounds().width/2, gameTitle.getLocalBounds().height/2));
	userTitle.setPosition(sf::Vector2f(20, 200));
	passTitle.setPosition(sf::Vector2f(20, 300));
	gameTitle.setPosition(sf::Vector2f(500/2, 100));

	// Create the boxes that will hold our username and password
	userBox.setSize(sf::Vector2f(TEXTBOXSIZEX, TEXTBOXSIZEY));
	passBox.setSize(sf::Vector2f(TEXTBOXSIZEX, TEXTBOXSIZEY));
	userBox.setOrigin(sf::Vector2f(0, userBox.getLocalBounds().height/2));
	passBox.setOrigin(sf::Vector2f(0, passBox.getLocalBounds().height/2));
	userBox.setPosition(sf::Vector2f(userTitle.getGlobalBounds().width + 20, 210));
	passBox.setPosition(sf::Vector2f(passTitle.getGlobalBounds().width + 20, 310));

	// Set the position of the text on top of each box
	userT.setPosition(userBox.getGlobalBounds().left + 5, userBox.getGlobalBounds().top);
	passwordT.setPosition(passBox.getGlobalBounds().left + 5, passBox.getGlobalBounds().top);

	// Create the "enter" button and set its position
	texture.loadFromFile("swarchEnter.png");
	enterButton.setTexture(texture);
	enterButton.setOrigin(sf::Vector2f(enterButton.getLocalBounds().width/2, enterButton.getLocalBounds().height/2));
	enterButton.setPosition(400 ,passBox.getGlobalBounds().top + passBox.getLocalBounds().height/2 + 80);
}

LoginScene::~LoginScene(void)
{
}

void LoginScene::update(float deltaTime)
{
	// Update the sf::Text with the string the user has entered
	userT.setString(user);
	passwordT.setString(projectedPassword);
}

void LoginScene::draw(sf::RenderWindow& window)
{
		// Draw the login GUI
		window.draw(userTitle);
		window.draw(passTitle);
		window.draw(gameTitle);
		window.draw(userBox);
		window.draw(passBox);
		
		// Draw the user text
		window.draw(userT);
		window.draw(passwordT);
		window.draw(enterButton);
}

void LoginScene::processEvents(sf::Event& evt, sf::RenderWindow& window)
{
	// If the user tries to close the window, end the simulation
	if(evt.type == sf::Event::Closed)
	{
		window.close();
	}
	// If the user presses a key
	else if(evt.type == sf::Event::KeyPressed)
	{
		// Was the key Return?
		if(evt.key.code == sf::Keyboard::Return)
		{
			// We must check to see if the password and name are valid, but for now this will suffice
			//userName.setString(user);

			// Tell the main program to go to the next scene with the new password
		}
		// Was the key BackSpace?
		else if(evt.key.code == sf::Keyboard::BackSpace)
		{
			// If user selected, delete from user
			if(enterUser)
				user = user.substr(0, user.size() - 1);
			// If password selected, delete from password
			else if(enterPass)
			{
				password = password.substr(0, password.size() - 1);
				projectedPassword = projectedPassword.substr(0, projectedPassword.size() - 1);
			}
		}
	}
	// Enter the text
	else if(evt.type == sf::Event::TextEntered)
	{
		// If the text isn't a backspace, return or delete key
		if(evt.text.unicode != 8 && evt.text.unicode != 15 && evt.text.unicode != 127)
		{
			if(enterUser)
				user += evt.text.unicode;
			else if(enterPass)
			{
				password += evt.text.unicode;
				projectedPassword += "*";
			}
		}
	}
	// If the mouse is pressed, select the box pressed on
	else if(evt.type == sf::Event::MouseButtonPressed)
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
			//userName.setString(user);
		}
	}
}