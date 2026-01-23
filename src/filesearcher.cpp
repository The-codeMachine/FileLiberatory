#include <filesearcher.hpp>

#include <stdexcept>

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
	for (std::filesystem::recursive_directory_iterator it = std::filesystem::recursive_directory_iterator(root, ec);
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