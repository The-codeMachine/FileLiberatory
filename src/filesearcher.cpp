#include <filesearcher.hpp>

#include <fstream>
#include <filesystem>

// searches a folder and adds all the files into the files vector
Folder searchFolder(const std::string& path) {
	Folder out = {
		std::vector<File>(),
		std::filesystem::path(path).filename().string(),
		path
	};

	try {
		// Iterate over each entry in the directory
		for (const auto& entry : std::filesystem::directory_iterator(path)) {
			// get the path of the current entry 
			const std::filesystem::path entryPath = entry.path();

			// check if the entry is a regular file
			if (std::filesystem::is_regular_file(entryPath)) {
				// construct file and insert into the output
				File file = {
					entryPath.filename().string(),
					entryPath.string(),
					entryPath.extension().string()
				};

				out.files.push_back(file);
			}
		}

	}
	catch (const std::filesystem::filesystem_error& e) {
		throw std::runtime_error("Failed to loop through a path: " + std::string(e.what()));
	}

	return out;
}