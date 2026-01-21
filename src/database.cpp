#include <database.hpp>

#include <cassert>
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

// Move Only
PreparedStatement::PreparedStatement(PreparedStatement&& other) noexcept : raw_(other.raw_), db_(other.db_) {
	other.raw_ = nullptr;
}

PreparedStatement& PreparedStatement::operator=(PreparedStatement&& other) noexcept {
	if (this != &other) {
		assert(&db_ == &other.db_ && "Cannot move-assign across databases"); // no reassignment using different databases

		delete raw_;
		raw_ = other.raw_;
		other.raw_ = nullptr;
	}

	return *this;
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

// Binding
void PreparedStatement::bindInt(const uint16_t& col, int value) {
	if (sqlite3_bind_int(raw_, col, value) != SQLITE_OK) 
		throw std::runtime_error("Failed to bind to column: " + std::to_string(col));
}

void PreparedStatement::bindDouble(const uint16_t& col, double value) {
	if (sqlite3_bind_double(raw_, col, value) != SQLITE_OK)
		throw std::runtime_error("Failed to bind to column: " + std::to_string(col));
}

void PreparedStatement::bindText(const uint16_t& col, const std::string& value) {
	if (sqlite3_bind_text(raw_, col, value.c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK)
		throw std::runtime_error("Failed to bind to column: " + std::to_string(col));
}

void PreparedStatement::bindBlob(const uint16_t& col, const void* data, size_t size) {
	if (sqlite3_bind_blob(raw_, col, data, size, nullptr) != SQLITE_OK)
		throw std::runtime_error("Failed to bind to column: " + std::to_string(col));
}

void PreparedStatement::bindNull(const uint16_t& col) {
	if (sqlite3_bind_null(raw_, col) != SQLITE_OK)
		throw std::runtime_error("Failed to bind to column: " + std::to_string(col));
}

// Execution Functions 
// this will execute and return a value (get)
bool PreparedStatement::step() {
	if (sqlite3_step(raw_) != SQLITE_OK) 
		return false;

	return true;
}

// this will execute and return nothing (insert/update/delete)
bool PreparedStatement::exec() {
	if (sqlite3_step(raw_) != SQLITE_OK) 
		return false;

	return true;
}

// Reset function
void PreparedStatement::reset() {
	sqlite3_clear_bindings(raw_);
	sqlite3_reset(raw_);
}