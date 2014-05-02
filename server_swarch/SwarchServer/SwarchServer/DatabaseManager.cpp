#include "DatabaseManager.h"
#include <iostream>

DatabaseManager::DatabaseManager(void)
{
	error = sqlite3_open("Swarch.db", &db);

	if(error)
	{
		std::cout << "Couldn't open the database" << std::endl;
		sqlite3_close(db);
		dbOpened = false;
	}
	else
		dbOpened =true;
}


DatabaseManager::~DatabaseManager(void)
{
	sqlite3_close(db);
}

bool DatabaseManager::doesUserExistInDB(std::string userName)
{
	std::string query = "SELECT COUNT(passWord) FROM swarchTable WHERE userName = ?";
	error = sqlite3_prepare_v2(db, query.c_str(), query.length(), &stmt, &tail);
	sqlite3_bind_text(stmt, 1, userName.c_str(), userName.length(), SQLITE_TRANSIENT);

	if(error != SQLITE_OK)
	{
		std::cout << "Couldn't prepare sql" << std::endl;
		return false;
	}

	if(sqlite3_step(stmt) == SQLITE_ROW)
	{
		//std::string name = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
		int count = sqlite3_column_int(stmt, 0);
		std::cout << count << std::endl;
		if(count > 0)
		{
			std::cout << "They match!" << std::endl;
			sqlite3_finalize(stmt);
			return true;
		}
	}

	std::cout << "No match could be found" << std::endl;

	sqlite3_finalize(stmt);
	return false;
}

bool DatabaseManager::doesPasswordMatchUser(std::string userName, std::string passWord)
{
	std::string query = "SELECT passWord FROM swarchTable WHERE userName = ?";

	error = sqlite3_prepare_v2(db, query.c_str(), query.length(), &stmt, &tail);
	sqlite3_bind_text(stmt, 1, userName.c_str(), userName.length(), SQLITE_TRANSIENT);

	if(error != SQLITE_OK)
	{
		std::cout << "Couldn't prepare sql" << std::endl;
		return false;
	}

	if(sqlite3_step(stmt) == SQLITE_ROW)
	{
		std::string password = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
		std::cout << passWord << " vs " << password << std::endl;
		if(password.compare(passWord) == 0)
		{
			std::cout << "They match!" << std::endl;
			sqlite3_finalize(stmt);
			return true;
		}
	}

	std::cout << "No match" << std::endl;

	sqlite3_finalize(stmt);
	return false;
}
	
void DatabaseManager::updateEntry(std::string userName, int score)
{
	std::string query = "UPDATE swarchTable SET score = ? WHERE userName = ? AND score < ?";

	error = sqlite3_prepare_v2(db, query.c_str(), query.length(), &stmt, &tail);
	sqlite3_bind_int(stmt, 1, score);
	sqlite3_bind_text(stmt, 2, userName.c_str(), userName.length(), SQLITE_TRANSIENT);
	sqlite3_bind_int(stmt, 3, score);

	if(error != SQLITE_OK)
	{
		std::cout << "Couldn't prepare sql" << std::endl;
		sqlite3_finalize(stmt);
		return;
	}

	if(sqlite3_step(stmt) == SQLITE_ERROR)
	{
		std::cout << "Entry not updated" << std::endl;
		sqlite3_finalize(stmt);
		return;
	}

	std::cout << "Entry updated" << std::endl;
	sqlite3_finalize(stmt);
}

void DatabaseManager::insertEntry(std::string userName, std::string passWord)
{
	std::string query = "INSERT INTO swarchTABLE VALUES (?, ?, 0)";

	error = sqlite3_prepare_v2(db, query.c_str(), query.length(), &stmt, &tail);
	sqlite3_bind_text(stmt, 1, userName.c_str(), userName.length(), SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 2, passWord.c_str(), passWord.length(), SQLITE_TRANSIENT);

	if(error != SQLITE_OK)
	{
		std::cout << "Couldn't prepare sql" << std::endl;
		sqlite3_finalize(stmt);
		return;
	}

	if(sqlite3_step(stmt) == SQLITE_ERROR)
	{
		std::cout << "Entry not added" << std::endl;
		sqlite3_finalize(stmt);
		return;
	}

	std::cout << "Entry added" << std::endl;
	sqlite3_finalize(stmt);
}