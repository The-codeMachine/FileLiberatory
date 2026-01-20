#include <database.hpp>

#include <stdexcept>

// ---- Database ----
// Constructor
Database::Database(const std::string& path) {
	if (sqlite3_open(path.c_str(), &raw_) != SQLITE_OK)
		throw std::runtime_error("Failed to open sqlite3 database: " + std::string(sqlite3_errmsg(raw_)));
}

// Destructor
Database::~Database() {
	// must outlive all prepared statements references it
	sqlite3_close(raw_);
}

// Raw access
sqlite3* Database::raw() {
	return raw_;
}

// Move only

// Prepared statement handling
PreparedStatement Database::prepare(const std::string& sql) {
	PreparedStatement stmt(*this, sql);

	return stmt;
}

// Error handling
std::string Database::errMsg() {
	return std::string(sqlite3_errmsg(raw_));
}

// ---- Prepared Statement ----
// Constructor
PreparedStatement::PreparedStatement(Database& db, const std::string& sql) {
	if (sqlite3_prepare(db.raw(), sql.c_str(), -1, &raw_, NULL) != SQLITE_OK)
		throw std::runtime_error("Failed to prepare a statement: " + db.errMsg());
}
