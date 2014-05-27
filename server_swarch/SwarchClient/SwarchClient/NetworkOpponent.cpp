#include "NetworkOpponent.h"


NetworkOpponent::NetworkOpponent(int score, float posX, float posY, int clientNum, std::string name)
	:delay(0.0f), dx(0.0f), dy(1.0f), score(score), clientNum(clientNum), name(name)
{
	body.setSize(sf::Vector2f(10, 10));
	body.setOrigin(sf::Vector2f(5,5));
	body.setPosition(posX, posY);
	body.setFillColor(sf::Color::Red);
}


NetworkOpponent::~NetworkOpponent(void)
{
}