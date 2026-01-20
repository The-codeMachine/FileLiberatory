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
PreparedStatement::PreparedStatement(Database& db, const std::string& sql) : db_(db) {
	if (sqlite3_prepare(db.raw(), sql.c_str(), -1, &raw_, NULL) != SQLITE_OK)
		throw std::runtime_error("Failed to prepare a statement: " + db.errMsg());
}

// Destructor
PreparedStatement::~PreparedStatement() {
	// must be deleted before the database reference is
	if (raw_) {
		sqlite3_finalize(raw_);
		raw_ = nullptr;
	}
}

// Getters 
std::string PreparedStatement::getText(const uint16_t& col) {
	const unsigned char* result = sqlite3_column_text(raw_, col);

	return std::string(reinterpret_cast<const char*>(result));
}

int PreparedStatement::getInt(const uint16_t& col) {
	return sqlite3_column_int(raw_, col);
}

double PreparedStatement::getDouble(const uint16_t& col) {
	return sqlite3_column_double(raw_, col);
}

// Column Info
int PreparedStatement::getColumnCount() {
	return sqlite3_column_count(raw_);
}

uint16_t PreparedStatement::getColumnBytes(const uint16_t& col) {
	return sqlite3_column_bytes(raw_, col);
}