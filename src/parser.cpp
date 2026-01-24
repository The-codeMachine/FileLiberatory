#include <parser.hpp>
#include <database.hpp>

#include <vector>
#include <algorithm>
#include <unordered_map>

// Generates small unique IDs: a, b, c, ... , z, A .. Z, 0..9, aa, ab, ...
static std::string generateToken(size_t id) {
    const std::string chars =
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "0123456789";

    std::string out;
    size_t base = chars.size();

    do {
        out.push_back(chars[id % base]);
        id /= base;
    } while (id > 0);

    return out;
}

std::string parseInput(const std::string& input) {
    // prefix -> token
    static std::unordered_map<std::string, std::string> usedPrefixes;

    static size_t nextId = 0;

    // Find longest matching prefix
    std::string bestPrefix;
    std::string bestToken;

    for (const auto& [prefix, token] : usedPrefixes) {
        if (input.rfind(prefix, 0) == 0) { // starts_with
            if (prefix.size() > bestPrefix.size()) {
                bestPrefix = prefix;
                bestToken = token;
            }
        }
    }

    // if found replace and return
    if (!bestPrefix.empty())
        return bestToken + input.substr(bestPrefix.size());

    // create new prefix
    size_t slashCount = 0;
    size_t pos = 0;

    for (; pos < input.size(); ++pos) {
        if (input[pos] == '/' || input[pos] == '\\') {
            slashCount++;
            if (slashCount == 4)
                break;
        }
    }

    std::string prefix = input.substr(0, pos);

    // generate new token
    std::string token = "<" + generateToken(nextId++) + ">";

    usedPrefixes[prefix] = token;

    return token + input.substr(prefix.size());
}