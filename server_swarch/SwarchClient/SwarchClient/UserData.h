#ifndef USERDATA_H
#define USERDATA_H

#include <string>;

// Jordan will make variables that can get updated for this object

class UserData
{
public:
	UserData(void);
	~UserData(void);

	std::string userNameStorage;
	std::string passwordStorage;
	bool startLogin;
	bool startGame;

	void changeToGame();
	void changeToLogin();

	//Thought implementation could work this way before.  Good to experiment!
	//static std::string userNameStorage;
	//static std::string passwordStorage;

	UserData(std::string userName, std::string password);


};

#endif // USERDATA_H