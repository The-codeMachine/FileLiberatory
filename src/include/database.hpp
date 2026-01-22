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
	Database(const Database&) = delete;
	Database& operator=(const Database&) = delete;

	Database(Database&& other) noexcept;
	Database& operator=(Database&& other) noexcept;

	// Statement preparation
	PreparedStatement prepare(const std::string& sql); // does not own the returned prepared statement

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
	PreparedStatement(const PreparedStatement&) = delete;
	PreparedStatement& operator=(const PreparedStatement&) = delete;

	PreparedStatement(PreparedStatement&& other) noexcept;
	PreparedStatement& operator=(PreparedStatement&& other) noexcept;

	// Public getters
	std::string getText(uint16_t col);
	int getInt(uint16_t col);
	double getDouble(uint16_t col);
	const void* getBlob(uint16_t col);

	// Column info
	int getColumnCount();
	uint16_t getColumnBytes(uint16_t col);
	
	// Binding
	void bindInt(uint16_t col, int value);
	void bindDouble(uint16_t col, double value);
	void bindText(uint16_t col, const std::string& value);
	void bindBlob(uint16_t col, const void* data, uint16_t size);
	void bindNull(uint16_t col);

	// Execution functions
	// this will execute and return a value (get)
	bool step();

	// this will execute and return nothing (insert/update/delete)
	bool exec(); // returns error code 

	// Reset functions (clear bindings)
	void reset();

private:
	Database& db_; // does not own the db_ 
	sqlite3_stmt* raw_; // owns the stmt_

};