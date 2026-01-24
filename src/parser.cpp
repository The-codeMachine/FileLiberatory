#include <parser.hpp>
#include <database.hpp>

#include <algorithm>

std::string PrefixManager::encode(const std::string& path) {
	// try exisiting prefixes (longest first)
    for (auto& p : sortedPrefixes_) {
        if (path.starts_with(p.prefix))
            return "<" + std::to_string(p.id) + ">" + path.substr(p.prefix.size());
    }

    // record prefix candidates
    collectCandidates(path);

    return path;
}

std::string PrefixManager::decode(const std::string& encoded) {
    if (encoded.starts_with("<")) {
        auto end = encoded.find('>');
        unsigned int id = std::stoi(encoded.substr(1, end - 1));

        PreparedStatement stmt = db_.prepare("SELECT prefix FROM prefixes WHERE id = ?;");
        stmt.bindInt(1, id);

        if (stmt.step())
            return stmt.getText(0) + encoded.substr(end + 1);
    }

    return encoded;
}

void PrefixManager::collectCandidates(const std::string& path) {
    size_t pos = 0;
    unsigned int depth = 0;

    while ((pos = path.find_first_of("/\\", pos + 1)) != std::string::npos) {
        if (++depth < 2)
            continue;

        std::string prefix = path.substr(0, pos);

        auto& entry = byPrefix_[prefix];
        entry.prefix = prefix;
        entry.uses++;

        if (entry.uses == MIN_USES && prefix.size() >= MIN_CHARS)
            promotePrefix(prefix);
    }
}

void PrefixManager::promotePrefix(const std::string& prefix) {
    static PreparedStatement stmt = db_.prepare("INSERT OR IGNORE INTO prefixes(prefix, uses VALUES (?, 0);");
    stmt.reset();

    stmt.bindText(1, prefix);
    stmt.exec();

    static PreparedStatement idQuery = db_.prepare("SELECT id FROM prefixes WHERE prefix = ?;");
    idQuery.reset();
    idQuery.bindText(1, prefix);

    if (idQuery.step()) {
        int id = idQuery.getInt(0);
        Prefix p{ id, prefix, 0 };

        byPrefix_[prefix] = p;
        rebuildSorted();
    }
}

void PrefixManager::rebuildSorted() {
    sortedPrefixes_.clear();
    for (auto& [k, v] : byPrefix_)
        sortedPrefixes_.push_back(v);

    std::sort(sortedPrefixes_.begin(),
        sortedPrefixes_.end(),
        [](auto& a, auto& b) {
            return a.prefix.size() > b.prefix.size();
        });
}

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