#ifndef SCENE_H
#define SCENE_H

#include <SFML/Graphics.hpp>

// The Scene class is an virtual class that allows us to create a variety of different scenes
class Scene
{
public:
	// Update is called every frame and is used to update the current state of the scene
	virtual void update(float deltaTime) = 0;
	// Draw is called every frame, drawing the current state
	virtual void draw(sf::RenderWindow& window) = 0;
	// HandleEvent is called every time an event is processed by the program, performing the appropriate action per scene
	virtual void processEvents(sf::Event& evt, sf::RenderWindow& window) = 0;
};

#endif // SCENE_H