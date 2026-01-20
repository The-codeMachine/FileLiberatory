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
	~Database();
	
	// Raw access
	sqlite3* raw();

	// Move Only

	// Statement preparation
	PreparedStatement prepare(const std::string& sql); // does not own the returned prepared statement

	// Execution functions
	// this will execute and return a certain value (get)

	// this will execute and returns nothing (insert/update/delete)

	// Error handling
	std::string errMsg();

private:
	sqlite3* raw_; // owns the db

};

// ---- Prepared Statement ----
class PreparedStatement {
public:
	// Constructor
	PreparedStatement(Database& db, const std::string& sql);

	// Destructor
	~PreparedStatement();

	// Move Only

	// Public getters
	std::string getText(const uint16_t& col);
	int getInt(const uint16_t& col);
	double getDouble(const uint16_t& col);

	// Column info
	int getColumnCount();
	uint16_t getColumnBytes(const uint16_t& col);
	
	// Setters

	// Execution functions
	// this will execute and return a value (get)

	// this will execute and return nothing (insert/update/delete)

	// Reset functions (clear bindings)
	void reset();

private:
	Database& db_; // does not own the db_ 
	sqlite3_stmt* raw_; // owns the stmt_

};