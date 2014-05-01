#ifndef LOGINSCENE_H
#define LOGINSCENE_H

#include "Scene.h"

//For user object
#include "UserData.h"

#define TEXTBOXSIZEX 300
#define TEXTBOXSIZEY 50

// The LoginScene is a type of Scene in which users can type in their username and password
class LoginScene : public Scene
{
public:
	LoginScene(void);
	~LoginScene(void);

	// Update is ran every frame and is used to update the login states
	void update(float deltaTime);
	// Draw is ran every frame and is used to draw the login menu
	void draw(sf::RenderWindow& window);
	// ProcessEvents is used to process events created by the window
	//void processEvents(sf::Event& evt, sf::RenderWindow& window);
	UserData processEvents(sf::Event& evt, sf::RenderWindow& window);

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