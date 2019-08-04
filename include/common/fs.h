#ifndef GLSLSCENE_FS_H
#define GLSLSCENE_FS_H

#include <string>
#include <vector>

namespace gs
{
	namespace fs
	{
		/**
		 * scan a directory
		 *
		 * @note directories . and .. are not included at entries!!!
		 *
		 * @param dirname Directory for scan
		 * @param dirCount (OUT) Count of sub directories
		 * @param entries (OUT) File and folder entries of the directory
		 * @return true if directory (dirname) successfully read. It also return
		 *         true if the directory is empty. False if directory doesn't exist
		 *         or no permissions etc.
		 */
		bool scanDirectory(const std::string& dirname, unsigned int& dirCount,
				std::vector<std::string>& entries,
				bool addSubDirs = true, bool addRegularFiles = true);

		/**
		 * Remove directories from entries.
		 * @return True if one or more files are left in entries.
		 */
		bool removeDirEntries(unsigned int dirCount,
				std::vector<std::string>& entries);

		std::string findDirectoryOfFile(const std::string& dirname, const std::string& filename);

		std::string getCwd();
		bool changeCwd(const std::string& dirname);

		std::string readFileAsString(const std::string& filename);

		/**
		 * Remove all ending slashes but never remove a slash at the beginning.
		 * Which means, if the first character is a '/' then it will not be removed.
		 * See special cases.
		 *
		 * name           return
		 * "."            "."
		 * "/foo"         "/foo"
		 * "foo/"         "foo"
		 * "foo///"       "foo"
		 * "///foo//a//"  "///foo//a"
		 * "/"            "/"          - special case!!!
		 * "/////"        "/"          - special case!!!
		 * ""             ""
		 */
		std::string getRemoveEndingSlashes(const std::string& name, char slash);

		/**
		 * Remove all ending slashes but never remove a slash at the beginning.
		 * Which means, if the first character is a '/' then it will not be removed.
		 * See special cases.
		 * And if more then one slashes to the next directory then these slashes
		 * will be reduced to one slash
		 *
		 * name           return
		 * "."            "."
		 * "/foo"         "/foo"
		 * "foo/"         "foo"
		 * "foo///"       "foo"
		 * "///foo//a//"  "/foo/a" - This result is different to getRemoveEndingSlashes()
		 * "/"            "/"      - special case!!!
		 * "/////"        "/"      - special case!!!
		 * ""             ""
		 */
		std::string getReduceAndRemoveEndingSlashes(const std::string& name, char slash);

		/**
		 * path                dirname     basename        dirname + basename
		 * "/foo/test.txt" --> "/foo/" --> "test.txt"  --> "/foo/test.txt"
		 * "test2.txt"     --> "./"    --> "test2.txt" --> "./test2.txt"
		 * "./test3.txt"   --> "./"    --> "test3.txt" --> "./test3.txt"
		 * "/test4.txt"    --> "/"     --> "test4.txt" --> "/test4.txt"
		 * "."             --> "./"    --> "."         --> "./."
		 * ".."            --> "./"    --> ".."        --> "./.."
		 * ""              --> "./"    --> ""          --> "./"
		 * "/"             --> "/"     --> ""          --> "/"
		 * "/foo/"         --> "/foo/" --> ""          --> "/foo/"
		 * "foo/"          --> "foo/"  --> ""          --> "foo/"
		 */
		void getDirAndBaseForPath(const std::string& path,
				std::string& dirname, std::string& basename);

		std::string getDirnameFromPath(const std::string& path);
		std::string getBasenameFromPath(const std::string& path);
	}
}

#endif //GLSLSCENE_FS_H
