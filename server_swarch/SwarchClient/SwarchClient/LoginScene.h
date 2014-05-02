#ifndef LOGINSCENE_H
#define LOGINSCENE_H

#include "Scene.h"
#include "md5.h"

//For user object
#include "UserData.h"

#define TEXTBOXSIZEX 300
#define TEXTBOXSIZEY 50

//This code is called twice in LoginScene.cpp to enable GameScene object to be assigned in main.cpp
/*
//Login scene fulfills its role.  changeToGame() function called to update the flag so that the 
//Scene* object in main.cpp can be re-assigned to a GameScene object!
			userObject->changeToGame();
*/



// The LoginScene is a type of Scene in which users can type in their username and password
class LoginScene : public Scene
{
public:
	LoginScene(void);
	~LoginScene(void);

	// Update is ran every frame and is used to update the login states
	void update(float deltaTime, NetworkManager& netMan);
	// Draw is ran every frame and is used to draw the login menu
	void draw(sf::RenderWindow& window);
	// ProcessEvents is used to process events created by the window
	//void processEvents(sf::Event& evt, sf::RenderWindow& window);
	UserData processEvents(sf::Event& evt, sf::RenderWindow& window, NetworkManager& netMan);

	//functions that return username and password values.  
	//Called in game func that uses
	//a LoginScene object.  
	//bool enableGetUserNameAndPassword;
	//std::string getUserName(void);
	//std::string getPassword(void);

	//User object to return
	UserData* userObject;


private:
	sf::Font font;
	std::string user, password, projectedPassword;
	sf::Text userT, passwordT, userTitle, passTitle, gameTitle;
	sf::RectangleShape userBox, passBox;
	sf::Texture texture;
	sf::Sprite enterButton;
	bool enterUser, enterPass;
};

#endif // LOGINSCENE_H