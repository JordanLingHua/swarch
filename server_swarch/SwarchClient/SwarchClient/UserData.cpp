#include "UserData.h"


UserData::UserData(void)
{
}


UserData::~UserData(void)
{
}

UserData::UserData(std::string userName, std::string password)
{
	userNameStorage = userName;
	passwordStorage = password;

}
