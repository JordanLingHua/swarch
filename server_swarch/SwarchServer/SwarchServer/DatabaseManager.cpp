#include "DatabaseManager.h"
#include <iostream>

DatabaseManager::DatabaseManager(void)
{
	// Attempts to open a database, saving the database into db
	error = sqlite3_open("Swarch.db", &db);

	// If an error occurred
	if(error)
	{
		// Close the database
		std::cout << "Couldn't open the database" << std::endl;
		sqlite3_close(db);
		dbOpened = false;
	}
	else
		dbOpened =true;
}


DatabaseManager::~DatabaseManager(void)
{
	// Close the database so we don't lose info
	sqlite3_close(db);
}

// Returns true if the inputted user exists in the database
// Returns false if the inputted user does not exist in the database
bool DatabaseManager::doesUserExistInDB(std::string userName)
{
	// Construct a query that will check if a user exists in the table based on a username
	// A query is specific to the SQL language, which uses specific keywords
	// The query we are using counts the total amount of passwords based on the username sent in by the user
	// The value will either be 0 or 1
	std::string query = "SELECT COUNT(passWord) FROM swarchTable WHERE userName = ?";
	
	// Prepare the query for use by the sqlite
	// Essentially, sqlite3_prepare_v2 takes the keywords found in the above query and saves them for when we run our query
	// If there is a keyword that doesn't exist in the database, such as '?', sqlite will remember it and be ready to swap the '?' for something else
	error = sqlite3_prepare_v2(db, query.c_str(), query.length(), &stmt, &tail);

	// Sets the user name in place for the '?'
	// sqlite3_bind_text is used to prevent SQL Injections.
	// If a user tries to input a SQL query to break our database, we bind the text to the '?'
	// the integer in the parameters is used to select which '?' is used.
	// Unsure about the SQLITE_TRANSIENT, will research
	sqlite3_bind_text(stmt, 1, userName.c_str(), userName.length(), SQLITE_TRANSIENT);

	// If an error occurred, return false
	if(error != SQLITE_OK)
	{
		std::cout << "Couldn't prepare sql" << std::endl;
		return false;
	}

	// sqlite3_step() runs our query on the database
	// If there are multiple entries, we would use a while loop instead.
	// Since each userName is unique, we can just use an if statement instead
	if(sqlite3_step(stmt) == SQLITE_ROW)
	{
		// Collect the count of how many passwords exist under the username
		int count = sqlite3_column_int(stmt, 0);
		std::cout << count << std::endl;

		// If there is a password, return true
		if(count > 0)
		{
			std::cout << "They match!" << std::endl;
			sqlite3_finalize(stmt);
			return true;
		}
	}

	std::cout << "No match could be found" << std::endl;
	
	// sqlite3_finalize() frees up our SQL statement that we set up in sqlite3_prepare_v2()
	// We must do this whenever we try to run a SQL query
	sqlite3_finalize(stmt);
	return false;
}

// Returns true if the inputted password matches the password under the inputted user in the database
// Returns false if the inputted password does not match the password under the inputted user in the database
bool DatabaseManager::doesPasswordMatchUser(std::string userName, std::string passWord)
{
	// Construct a query that will check if a user exists in the table based on a username
	// The query we are using receives the password from the inputted user
	std::string query = "SELECT passWord FROM swarchTable WHERE userName = ?";

	// Prepare the query for use by the sqlite
	error = sqlite3_prepare_v2(db, query.c_str(), query.length(), &stmt, &tail);

	// Sets the user name in place for the '?'
	sqlite3_bind_text(stmt, 1, userName.c_str(), userName.length(), SQLITE_TRANSIENT);

	// If an error occurred, return false
	if(error != SQLITE_OK)
	{
		std::cout << "Couldn't prepare sql" << std::endl;
		return false;
	}

	// sqlite3_step() runs our query on the database
	if(sqlite3_step(stmt) == SQLITE_ROW)
	{
		// Acquires the password from the database (which we recast from a const char* to a string)
		std::string password = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
		std::cout << passWord << " vs " << password << std::endl;
		// If the passwords match, return true;
		if(password.compare(passWord) == 0)
		{
			std::cout << "They match!" << std::endl;
			sqlite3_finalize(stmt);
			return true;
		}
	}

	std::cout << "No match" << std::endl;

	// sqlite3_finalize() frees up our SQL statement that we set up in sqlite3_prepare_v2()
	sqlite3_finalize(stmt);
	return false;
}

// Updates a saved user's score if the score is increased
void DatabaseManager::updateEntry(std::string userName, int score)
{
	// Construct a query that will check if a user exists in the table based on a username
	// The query we are using updates the score of a user if their name matches and they have improved their score
	std::string query = "UPDATE swarchTable SET score = ? WHERE userName = ? AND score < ?";

	// Prepare the query for use by the sqlite
	error = sqlite3_prepare_v2(db, query.c_str(), query.length(), &stmt, &tail);

	// Sets the user name in place for the '?'
	sqlite3_bind_int(stmt, 1, score);
	sqlite3_bind_text(stmt, 2, userName.c_str(), userName.length(), SQLITE_TRANSIENT);
	sqlite3_bind_int(stmt, 3, score);

	// If the SQL file failed, exit the method
	if(error != SQLITE_OK)
	{
		std::cout << "Couldn't prepare sql" << std::endl;
		sqlite3_finalize(stmt);
		return;
	}

	// Run our SQL Query
	// If their are errors while processing the query, exit the method
	if(sqlite3_step(stmt) == SQLITE_ERROR)
	{
		std::cout << "Entry not updated" << std::endl;
		sqlite3_finalize(stmt);
		return;
	}

	std::cout << "Entry updated" << std::endl;

	// sqlite3_finalize() frees up our SQL statement that we set up in sqlite3_prepare_v2()
	sqlite3_finalize(stmt);
}

// Inserts a new user into the database along with a password and a preset score of 0
void DatabaseManager::insertEntry(std::string userName, std::string passWord)
{
	// Construct a query that will insert a new user into our database
	// The query we are using inserts a new entry based on username, password and score
	std::string query = "INSERT INTO swarchTABLE VALUES (?, ?, 0)";

	// Prepares the SQL Query for use
	error = sqlite3_prepare_v2(db, query.c_str(), query.length(), &stmt, &tail);

	// Binds info to the '?'
	sqlite3_bind_text(stmt, 1, userName.c_str(), userName.length(), SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 2, passWord.c_str(), passWord.length(), SQLITE_TRANSIENT);

	// If the SQL file wasn't prepared correctly, exit the method
	if(error != SQLITE_OK)
	{
		std::cout << "Couldn't prepare sql" << std::endl;
		sqlite3_finalize(stmt);
		return;
	}

	// Run our SQL program
	if(sqlite3_step(stmt) == SQLITE_ERROR)
	{
		std::cout << "Entry not added" << std::endl;
		sqlite3_finalize(stmt);
		return;
	}

	std::cout << "Entry added" << std::endl;

	// sqlite3_finalize() frees up our SQL statement that we set up in sqlite3_prepare_v2()
	sqlite3_finalize(stmt);
}

bool DatabaseManager::doesTableExistInDB()
{
	// Construct a query that will insert a new user into our database
	// The query we are using inserts a new entry based on username, password and score
	std::string query = "SELECT COUNT(*) FROM swarchTable";

	// Prepares the SQL Query for use
	error = sqlite3_prepare_v2(db, query.c_str(), query.length(), &stmt, &tail);

	// If the SQL file wasn't prepared correctly, exit the method
	if(error != SQLITE_OK)
	{
		std::cout << "Couldn't prepare sql" << std::endl;
		sqlite3_finalize(stmt);
		return false;
	}

	// Run our SQL program
	if(sqlite3_step(stmt) == SQLITE_ROW)
	{
		int count = sqlite3_column_int(stmt, 0);
		std::cout << count << std::endl;

		// If there is a password, return true
		if(count >= 0)
		{
			std::cout << "The table exists!" << std::endl;
			sqlite3_finalize(stmt);
			return true;
		}
	}

	std::cout << "The table doesn't exist" << std::endl;

	// sqlite3_finalize() frees up our SQL statement that we set up in sqlite3_prepare_v2()
	sqlite3_finalize(stmt);

	return false;
}

void DatabaseManager::createTable()
{
	// Construct a query that will create a table in our database
	std::string query = "CREATE TABLE swarchTable(userName VARCHAR(50), passWord VARCHAR(50), score INT)";

	// Prepares the SQL Query for use
	error = sqlite3_prepare_v2(db, query.c_str(), query.length(), &stmt, &tail);

	// If the SQL file wasn't prepared correctly, exit the method
	if(error != SQLITE_OK)
	{
		std::cout << "Couldn't prepare sql" << std::endl;
		sqlite3_finalize(stmt);
		return;
	}

	// Run our SQL program
	if(sqlite3_step(stmt) == SQLITE_ERROR)
	{
		std::cout << "Table failed to create" << std::endl;
		sqlite3_finalize(stmt);
		return;
	}

	std::cout << "Table created" << std::endl;

	// sqlite3_finalize() frees up our SQL statement that we set up in sqlite3_prepare_v2()
	sqlite3_finalize(stmt);
}

// Sends updates to the webserver regarding updated player info
std::list<PlayerScore> DatabaseManager::updateWebServer()
{
	// Construct a score list to be sent to the NetworkManager for transmission
	std::list<PlayerScore> scoreList;

	// Construct a query that will return all usernames and scores from within our table
	std::string query = "SELECT * FROM swarchTable";

	// Prepare the query for use by the sqlite
	error = sqlite3_prepare_v2(db, query.c_str(), query.length(), &stmt, &tail);

	// If an error occurred, return false
	if(error != SQLITE_OK)
	{
		std::cout << "Couldn't prepare sql" << std::endl;
		return scoreList;
	}

	// sqlite3_step() runs our query on the database
	while(sqlite3_step(stmt) == SQLITE_ROW)
	{
		// Acquires the password from the database (which we recast from a const char* to a string)
		std::string user = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
		int userScore = sqlite3_column_int(stmt, 2);

		// Save the name and score
		PlayerScore info;
		info.name = user;
		info.score = userScore;
		scoreList.push_back(info);
	}

	// sqlite3_finalize() frees up our SQL statement that we set up in sqlite3_prepare_v2()
	sqlite3_finalize(stmt);

	// Send the file
	return scoreList;
}