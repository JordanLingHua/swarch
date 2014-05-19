#include "NetworkOpponent.h"


NetworkOpponent::NetworkOpponent(int score, float posX, float posY, int clientNum)
	:dx(0.0f), dy(1.0f), score(score), clientNum(clientNum)
{
	body.setSize(sf::Vector2f(10, 10));
	body.setPosition(posX, posY);
}


NetworkOpponent::~NetworkOpponent(void)
{
}