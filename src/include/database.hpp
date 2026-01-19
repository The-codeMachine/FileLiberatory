#pragma once
#include <sqlite3.h>

#include <string>

// This file will include and setup all the SQLite information
// This will essentially remove the friction of using SQLite

// Forward declare
class PreparedStatement;

// ---- Database ----
class Database {
public:
	// Constructor
	Database(const std::string& path);

	// Destructor
	
	// Move Only

	// Statement preparation
	PreparedStatement prepare(const std::string& sql); // does not own the returned prepared statement

	// Execution functions
	// this will execute and return a certain value (get)

	// this will execute and returns nothing (insert/update/delete)

private:
	sqlite3* db_; // owns the db

};

// ---- Prepared Statement ----
class PreparedStatement {
public:
	// Constructor
	PreparedStatement(const Database& db, const std::string& sql);

	// Destructor

	// Move Only

	// Public getters

	// Execution functions
	// this will execute and return a value (get)

	// this will execute and return nothing (insert/update/delete)

	// Reset functions (clear bindings)

	// Reset function (deletion)

private:
	// Getters 

private:
	Database& db_; // does not own the db_ 
	sqlite3_stmt* stmt_; // owns the stmt_

};