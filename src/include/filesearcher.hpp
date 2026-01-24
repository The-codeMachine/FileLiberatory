#pragma once
#include <database.hpp>

#include <vector>
#include <string>
#include <filesystem>

struct File {
	std::string name;
	std::string path;
	std::string type;
};

struct Folder {
	std::vector<File> files;
	std::string name;
	std::string path;
};

void scanFolder(Database& db, const std::filesystem::path& root);

Folder search(const std::filesystem::path& path); 

Folder lowSearch(const std::filesystem::path& root);