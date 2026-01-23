#pragma once

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

Folder lowSearch(const std::filesystem::path& root);