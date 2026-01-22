#pragma once
#include <vector>
#include <string>

struct File {
	const std::string name;
	const std::string path;
	const std::string type;
};

struct Folder {
	std::vector<File> files;
	const std::string name;
	const std::string path;
};

std::vector<Folder> lowSearch(const std::string& path);