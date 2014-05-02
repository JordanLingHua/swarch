#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <sqlite3.h>
#include <string>

class DatabaseManager
{
public:
	DatabaseManager(void);
	~DatabaseManager(void);

	bool doesUserExistInDB(std::string userName);
	bool doesPasswordMatchUser(std::string userName, std::string passWord);
	
	void updateEntry(std::string userName, int score);
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