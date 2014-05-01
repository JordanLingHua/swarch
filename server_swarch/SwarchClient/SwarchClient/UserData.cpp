#include "UserData.h"


UserData::UserData(void)
	:startGame(false), startLogin(false)
{
}


UserData::~UserData(void)
{
}

UserData::UserData(std::string userName, std::string password)
	:startGame(false), startLogin(false)
{
	userNameStorage = userName;
	passwordStorage = password;
}

void UserData::changeToGame()
{
	startGame = true;
}

void UserData::changeToLogin()
{
	startLogin = true;
}