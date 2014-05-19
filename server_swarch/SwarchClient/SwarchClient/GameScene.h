#ifndef GAMESCENE_H
#define GAMESCENE_H

#include "Scene.h"
#include "NetworkOpponent.h"
#include <list>

//Confirmed that Not needed since Scene.h has it.  But just in case.  
#include "UserData.h"

#define WINDOWSIZEX 500
#define WINDOWSIZEY 500
#define SPEED 20
#define PLAYERSIZE 10

#define DIRECTION_COMMAND 1
#define PLAYER_JOIN 2
#define PLAYER_DISCONNECT 3
#define PLAYER_EATEN 4
#define PLAYER_DEATH 5
#define PELLET_EATEN 6
#define INITIALIZATION_CODE 20

// The GameScene is a type of Scene in which users can play Swarch
class GameScene : public Scene
{
public:
	GameScene(std::string username = "Error");
	~GameScene(void);

	// Update is ran every frame and is used to update our game state
	void update(float deltaTime, NetworkManager& netMan);
	// Draw is ran every frame to draw our game state
	void draw(sf::RenderWindow& window);
	// ProcessEvents is used to process events the window creates
	//void processEvents(sf::Event& evt, sf::RenderWindow& window);
	UserData processEvents(sf::Event& evt, sf::RenderWindow& window, NetworkManager& netMan);

private:
	void processInput(NetworkManager& netMan);

	sf::Font font;
	sf::Text userName;

	float delay, dx, dy;
	int randPelletLocX, randPelletLocY, numOfPellets, playerNum, score;
	sf::RectangleShape* pelletList, myBox;
	std::list<NetworkOpponent> playerList;
	bool upPress, downPress, rightPress, leftPress;
};

#endif // GAMESCENE_H