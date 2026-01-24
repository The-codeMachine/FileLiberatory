#pragma once
#include <database.hpp>

#include <string>
#include <vector>
#include <unordered_map>

struct Prefix {
	int id;
	std::string prefix;
	int uses;
};

class PrefixManager {
public:
	// Constructor
	PrefixManager(Database& db) : db_(db) {
		loadExisting();
	}

	std::string encode(const std::string& path);
	std::string decode(const std::string& encoded);

private:
	Database& db_;

	std::unordered_map<std::string, Prefix> byPrefix_;
	std::vector<Prefix> sortedPrefixes_;

	static constexpr unsigned int MIN_USES = 250;
	static constexpr unsigned int MIN_CHARS = 20;

	void loadExisting();
	void promotePrefix(const std::string& prefix);
	void collectCandidates(const std::string& path);
	void rebuildSorted();
};

std::string parseInput(const std::string& input);