#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <sqlite3.h>
#include <string>

// A DatabaseManager is an object the opens and runs SQL functions on a database
class DatabaseManager
{
public:
	DatabaseManager(void);
	~DatabaseManager(void);

	// Returns true if the inputted user exists in the database
	// Returns false if the inputted user does not exist in the database
	bool doesUserExistInDB(std::string userName);

	// Returns true if the inputted password matches the password under the inputted user in the database
	// Returns false if the inputted password does not match the password under the inputted user in the database
	bool doesPasswordMatchUser(std::string userName, std::string passWord);
	
	// Updates a saved user's score if the score is increased
	void updateEntry(std::string userName, int score);

	// Inserts a new user into the database along with a password and a preset score of 0
	void insertEntry(std::string userName, std::string passWord);

private:
	sqlite3 *db;
	sqlite3_stmt *stmt;
	const char *errMSG;
	const char *tail;
	int error;
	bool dbOpened;
};

#endif // DATABASEMANAGER_H