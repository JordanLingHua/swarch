#include "NetworkOpponent.h"


NetworkOpponent::NetworkOpponent(int score, float posX, float posY, int clientNum, std::string oName, int width)
	:delay(0.0f), dx(0.0f), dy(1.0f), score(score), clientNum(clientNum), name(oName)
{
	body.setSize(sf::Vector2f(width, width));
	body.setOrigin(sf::Vector2f(5,5));
	body.setPosition(posX, posY);
	body.setFillColor(sf::Color::Red);
	name = oName;
}


NetworkOpponent::~NetworkOpponent(void)
{
}