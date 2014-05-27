#ifndef NETWORKOPPONENT_H
#define NETWORKOPPONENT_H

#include <SFML\Graphics.hpp>

class NetworkOpponent
{
public:
	NetworkOpponent(int score, float posX, float posY, int clientNum, std::string name);
	~NetworkOpponent(void);

	float delay, dx, dy;
	sf::RectangleShape body;
	int score, clientNum;
	std::string name;
};

#endif // NETWORKOPPONENT_H