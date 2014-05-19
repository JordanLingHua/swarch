#ifndef NETWORKOPPONENT_H
#define NETWORKOPPONENT_H

#include <SFML\Graphics.hpp>

class NetworkOpponent
{
public:
	NetworkOpponent(int score, float posX, float posY, int clientNum);
	~NetworkOpponent(void);

	float dx, dy;
	sf::RectangleShape body;
	int score, clientNum;
};

#endif // NETWORKOPPONENT_H