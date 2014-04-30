#ifndef GAMESCENE_H
#define GAMESCENE_H

#include "Scene.h"

#define WINDOWSIZEX 500
#define WINDOWSIZEY 500
#define SPEED 200.0f
#define PLAYERSIZE 10

// The GameScene is a type of Scene in which users can play Swarch
class GameScene : public Scene
{
public:
	GameScene(void);
	~GameScene(void);

	// Update is ran every frame and is used to update our game state
	void update(float deltaTime);
	// Draw is ran every frame to draw our game state
	void draw(sf::RenderWindow& window);
	// ProcessEvents is used to process events the window creates
	void processEvents(sf::Event& evt, sf::RenderWindow& window);

private:
	sf::Font font;
	sf::Text userName;

	float delay, dx, dy;
	int randPelletLocX, randPelletLocY, numOfPellets;
	sf::RectangleShape* pelletList, myBox;
};

#endif // GAMESCENE_H