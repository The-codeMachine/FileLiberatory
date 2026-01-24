#include <filesearcher.hpp>
#include <database.hpp>

#include <iostream>
#include <chrono>

// When starting up for the first time it will load all the files into a SQLite database.
// This SQLite database will store all the files paths so that searching is quick.

// Will use Kernel/OS APIs to find the updated files (and update the database).
// Whenever the program boots up it will check for changes (in the database record directory changed timestamps and compare it).

int main() {
	auto start = std::chrono::steady_clock::now();

	try {
		Database db("file_index.db");
		db.prepare("CREATE TABLE IF NOT EXISTS files ("
			"id INTEGER PRIMARY KEY,"
			"path TEXT NOT NULL UNIQUE,"
			"is_dir INTEGER NOT NULL"
			");").exec();
		db.prepare("CREATE TABLE IF NOT EXISTS files ("
			"id INTEGER PRIMARY KEY,"
			"prefix TEXT UNIQUE,"
			"uses INTEGER"
			");").exec();

		scanFolder(db, "D:/Developer/Projects");
	}
	catch (const std::exception& e) {
		std::cerr << "There was an error: " << e.what() << "\n";
	}

	auto end = std::chrono::steady_clock::now();
	auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

	std::cout << "Time taken: " << time << " ms\n";

	return 0;
}