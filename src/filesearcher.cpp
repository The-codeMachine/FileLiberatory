#include <filesearcher.hpp>
#include <parser.hpp>

#include <iostream>
#include <stdexcept>


void scanFolder(Database& db, const std::filesystem::path& root, PrefixManager& prefixes) {
	// Begin transaction 
	db.prepare("BEGIN TRANSACTION;").exec();

	static PreparedStatement insertStmt = db.prepare(
		"INSERT OR IGNORE INTO files(path, is_dir) VALUES (?, ?);"
	);

	try {
		for (auto it = std::filesystem::recursive_directory_iterator(
			root,
			std::filesystem::directory_options::skip_permission_denied);
			it != std::filesystem::recursive_directory_iterator();
			++it) {
			const std::filesystem::path& p = it->path();

			bool isDir = it->is_directory();

			insertStmt.reset();
			insertStmt.bindText(1, prefixes.encode(p.string()));
			insertStmt.bindInt(2, isDir ? 1 : 0);

			insertStmt.exec();
		}
	}
	catch (const std::exception& e) {
		std::cerr << "Filesystem error: " << e.what() << "\n";
	}

	db.prepare("COMMIT;").exec();
}

Folder search(const std::filesystem::path& path) {
	if (!std::filesystem::exists(path))
		throw std::runtime_error("Path does not exist: " + path.string());

	if (!std::filesystem::is_directory(path))
		throw std::runtime_error("Path is not a directory: " + path.string());

	Folder out;
	out.name = path.filename().string();
	out.path = path.string();
}

// seaches through the entire computer and adds the files to the database
void initSearch(const std::string& path) {
	Database db(path);

	std::error_code ec;

	// Walk through the entire C drive
	for (auto it = std::filesystem::recursive_directory_iterator("C:/", ec); 
		it != std::filesystem::recursive_directory_iterator(); it.increment(ec)) {
		if (ec) {
			std::cerr << "Filesystem had an error: " << ec.message() << "\n";
			continue;
		}

		const std::filesystem::directory_entry& entry = *it;
		static PreparedStatement stmt = db.prepare("INSERT INTO files (path) VALUES (?);");
		stmt.reset();

		stmt.bindText(1, entry.path().string());
		if (!stmt.exec()) {
			std::cerr << "Database error: " << db.errMsg() << "\n";
			continue;
		}
	}
}

Folder lowSearch(const std::filesystem::path& root) {
	if (!std::filesystem::exists(root))
		throw std::runtime_error("Path does not exist: " + root.string());

	if (!std::filesystem::is_directory(root))
		throw std::runtime_error("Path is not a directory: " + root.string());

	Folder out;
	out.name = root.filename().string();
	out.path = root.string();

	std::error_code ec;

	// Walk entire tree
	for (std::filesystem::recursive_directory_iterator it = std::filesystem::recursive_directory_iterator(root, 
		std::filesystem::directory_options::skip_permission_denied, ec);
		it != std::filesystem::recursive_directory_iterator();
		it.increment(ec)) {
		if (ec)
			throw std::runtime_error("Filesystem error while iterating: " + ec.message());

		const std::filesystem::directory_entry& entry = *it;
		if (!entry.is_regular_file())
			continue;

		const std::filesystem::path& path = entry.path();

		out.files.emplace_back(File{
			path.filename().string(),
			path.string(),
			path.extension().string()
			});
	}

	return out;
}