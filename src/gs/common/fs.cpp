#include <gs/common/fs.h>
#include <gs/system/log.h>
#include <fstream>

#if defined(_MSC_VER)
//#include <filesystem> // C++17
#include <experimental/filesystem>
#include <direct.h> // for _getcwd
#else
#include <dirent.h> // for scan directory
#include <sys/stat.h> // for stat
#include <stdlib.h> // for free
#include <unistd.h> // for getcwd
#endif
#include <string.h> // for strlen

#if defined(_MSC_VER)
#define strcasecmp _stricmp
#define getcwd _getcwd
#define chdir _chdir

namespace std
{
	namespace fs = std::experimental::filesystem;
}
#endif

namespace gs
{
	namespace
	{
		void getDirnameAndFilenameFromPath(const std::string& path,
				std::string *dirname, std::string *basename)
		{
			std::size_t found = path.rfind('/');
#if defined(_MSC_VER)
			std::size_t foundWin = path.rfind('\\');
			if (foundWin < found) {
				found = foundWin;
			}
#endif
			if (found == std::string::npos) {
				if (dirname) {
					*dirname = "./";
				}
				if (basename) {
					*basename = path;
				}
				return;
			}

			if (dirname) {
				*dirname = path.substr(0, found + 1);
			}
			if (basename) {
				*basename = path.substr(found + 1);
			}
		}
	}
}

bool gs::fs::scanDirectory(const std::string& dirname, unsigned int& dirCount,
		std::vector<std::string>& entries,
		bool addSubDirs, bool addRegularFiles)
{
	dirCount = 0;
	entries.clear();
#if defined(_MSC_VER)
	for (unsigned int action = 0; action < 2; action++) {
		if (action == 0 && !addSubDirs) {
			continue;
		}
		if (action == 1 && !addRegularFiles) {
			continue;
		}
		for (const auto& entry : std::fs::directory_iterator(dirname)) {
			std::string filename(entry.path().filename().u8string());
			switch (action) {
				case 0: // folder check
					if (std::fs::is_directory(entry.status())) {
						filename += "/";
						entries.push_back(filename);
						dirCount++;
					}
				case 1: // file check
					if (std::fs::is_regular_file(entry.status())) {
						entries.push_back(filename);
					}
			}
		}
	}
#else
	struct dirent **lNamelist;
	int lCount(scandir(dirname.c_str(), &lNamelist, NULL, alphasort));
	if (lCount < 0) {
		return false;
	}

	for (unsigned int action = 0; action < 2; action++) {
		if (action == 0 && !addSubDirs) {
			continue;
		}
		if (action == 1 && !addRegularFiles) {
			continue;
		}
		for (int i = 0; i < lCount; i++) {
			std::string lFilename(dirname);
			lFilename += "/";
			lFilename += lNamelist[i]->d_name;
			struct stat lFileAttr;
			if (stat(lFilename.c_str(), &lFileAttr) < 0) {
				continue; // try next file
			}
			switch (action) {
				case 0: // folder check
					if (S_ISDIR(lFileAttr.st_mode)) {
						std::string lDirName(lNamelist[i]->d_name);
						if (lDirName == "." || lDirName == "..") {
							break;
						}
						entries.push_back(lDirName + "/");
						dirCount++;
					}
					break;
				case 1: // file check
					if (S_ISREG(lFileAttr.st_mode)) {
						entries.push_back(lNamelist[i]->d_name);
					}
					break;
			}
		}
	}

	for (int i = 0; i < lCount; i++) {
		free(lNamelist[i]);
	}
	free(lNamelist);
#endif
	return true;
}

bool gs::fs::removeDirEntries(unsigned int dirCount,
		std::vector<std::string>& entries)
{
	if (entries.empty()) {
		return false;
	}

	if (dirCount >= entries.size()) {
		entries.clear();
		return false;
	}

	if (dirCount == 0) {
		// nothing to remove
		return true;
	}

	for (unsigned int i = 0; i < entries.size() - dirCount; i++) {
		entries[i] = entries[dirCount + i];
	}

	for (unsigned int i = 0; i < dirCount; i++) {
		entries.pop_back();
	}

	if (entries.empty()) {
		// should not be possible
		return false;
	}
	return true;
}

std::string gs::fs::findDirectoryOfFile(const std::string& dirname, const std::string& filename)
{
	std::string dir = dirname;
	dir = getRemoveEndingSlashes(dir, '/');
#if defined(_MSC_VER)
	dir = getRemoveEndingSlashes(dir, '\\');
#endif

	unsigned int dirCount = 0;
	std::vector<std::string> entries;
	std::string prevDir = dir;
	do {
		if (!scanDirectory(dir, dirCount, entries,
				false /* addSubDirs */, true /* addRegularFiles */)) {
			return "";
		}
		for (const auto &f : entries) {
			//LOGI("file %s\n", f.c_str());
			if (f == filename) {
				// found file --> return current directory
				return dir;
			}
		}
		prevDir = dir;
		dir = getDirnameFromPath(dir);
		dir = getRemoveEndingSlashes(dir, '/');
#if defined(_MSC_VER)
		dir = getRemoveEndingSlashes(dir, '\\');
#endif
		//LOGI("next search dir %s\n", dir.c_str());
	} while (dir != prevDir);
	return "";
}

std::string gs::fs::getCwd()
{
	char cwd[10240];
	if (!getcwd(cwd, 10240)) {
		LOGE("Can't get current working directory (cwd > 10240 characters?)\n");
		return "";
	}
	size_t len = strlen(cwd);
	if (len > 1 && cwd[len - 1] == '/') {
		cwd[len - 1] = '\0';
		--len;
	}
	return cwd;
}

bool gs::fs::changeCwd(const std::string& dirname)
{
	return chdir(dirname.c_str()) != -1;
}

std::string gs::fs::readFileAsString(const std::string& filename)
{
	std::ifstream f(filename);
	if (!f.is_open()) {
		LOGE("Can't open '%s'\n", filename.c_str());
		return "";
	}
	std::stringstream buffer;
	buffer << f.rdbuf();
	return buffer.str();
}

std::string gs::fs::getRemoveEndingSlashes(const std::string& name, char slash)
{
	if (name.empty() || name.back() != slash) {
		return name;
	}
	int len = name.length();
	int i = len - 1;
	for (; i >= 0 && name[i] == slash; --i)
		;
	if (i < 0) {
		// the full string has always the same character '/'.
		char str[2] = {slash, '\0'};
		return str;
	}
	return name.substr(0, i + 1);
}

std::string gs::fs::getReduceAndRemoveEndingSlashes(const std::string& name, char slash)
{
	std::string tmp = name;
	size_t len = name.length();
	size_t ii = 0, oi = 0; // ii ... input index, oi ... output index
	bool prevWasCh = false;
	bool fullStringWithCh = true;
	for (; ii < len; ++ii) {
		if (name[ii] != slash) {
			if (prevWasCh) {
				tmp[oi] = slash;
				++oi;
			}
			tmp[oi] = name[ii];
			++oi;
			prevWasCh = false;
			fullStringWithCh = false;
		}
		else {
			prevWasCh = true;
		}
	}
	// also works for an empty string!
	return fullStringWithCh ? name.substr(0, 1) : tmp.substr(0, oi);
}

void gs::fs::getDirAndBaseForPath(const std::string& path,
		std::string& dirname, std::string& basename)
{
	getDirnameAndFilenameFromPath(path, &dirname, &basename);
}

std::string gs::fs::getDirnameFromPath(const std::string& path)
{
	std::string dirname;
	getDirnameAndFilenameFromPath(path, &dirname, nullptr);
	return dirname;
}

std::string gs::fs::getBasenameFromPath(const std::string& path)
{
	std::string basename;
	getDirnameAndFilenameFromPath(path, nullptr, &basename);
	return basename;
}


