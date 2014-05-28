#ifndef GAMESCENE_H
#define GAMESCENE_H

#include "Scene.h"
#include "NetworkOpponent.h"
#include <list>
#include <sstream>

//Confirmed that Not needed since Scene.h has it.  But just in case.  
#include "UserData.h"

#define WINDOWSIZEX 500
#define WINDOWSIZEY 500
#define SPEED 100
#define PLAYERSIZE 10

#define DIRECTION_COMMAND 1
#define PLAYER_JOIN 2
#define PLAYER_DISCONNECT 3
#define PLAYER_EATEN 4
#define PLAYER_DEATH 5
#define PELLET_EATEN 6
#define INITIALIZATION_CODE 20
#define PLAYER_WON 7

// This isn't the same object that stores the player info
// PlayerScore is used to sort and project the names of clients in order of score
struct PlayerScore
{
	std::string name;
	int score;
};

class ScoreSort
{
public:
	bool operator()(PlayerScore& s1, PlayerScore& s2) // Returns true if s1 is earlier that s2
    {
       if (s1.score < s2.score)
		   return true;
      
       return false;
    }
};

// The GameScene is a type of Scene in which users can play Swarch
class GameScene : public Scene
{
public:
	GameScene(std::string username = "Error");
	~GameScene(void);

	// Update is ran every frame and is used to update our game state
	void update(float deltaTime, NetworkManager& netMan);
	// Draw is ran every frame to draw our game state
	void draw(sf::RenderWindow& window);
	// ProcessEvents is used to process events the window creates
	//void processEvents(sf::Event& evt, sf::RenderWindow& window);
	UserData processEvents(sf::Event& evt, sf::RenderWindow& window, NetworkManager& netMan);

private:
	void processInput(NetworkManager& netMan);
	void sortScores();
	void disconnectClient(int clientNum);

	sf::Font font;
	sf::Text userName, winnerText;
	std::stringstream winnerStream;
	std::string name;

	float delay, dx, dy;
	int randPelletLocX, randPelletLocY, numOfPellets, playerNum, score;
	sf::RectangleShape* pelletList, myBox;
	std::list<NetworkOpponent> playerList;
	bool upPress, downPress, rightPress, leftPress;
	bool playerWon;
};

#endif // GAMESCENE_H