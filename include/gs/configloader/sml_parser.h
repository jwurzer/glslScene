#ifndef GLSLSCENE_SML_PARSER_H
#define GLSLSCENE_SML_PARSER_H

#include <fstream>
#include <string>

namespace gs
{
	class CfgValuePair;

	/**
	 * sml - smart markup language
	 * See more infos at the end of this file.
	 */
	class SmlParser
	{
	public:
		SmlParser();
		SmlParser(const std::string& filename);
		~SmlParser();
		bool setFilename(const std::string& filename);
		bool begin();
		// return -1 for error, -2 for end of file or deep count for success
		int getNextSmlEntry(CfgValuePair& entry);
		// return -1 for error or deep count for success
		int getNextSmlEntry(std::string& utf8Line, CfgValuePair& entry,
				int lineNumber);
		bool getAsValuePairTree(CfgValuePair& root,
				bool inclEmptyLines = false, bool inclComments = false);
		const std::string& getErrorMsg() const { return mErrorMsg; }
		unsigned int getLineNumber() const { return mLineNumber; }
		// return filename with linenumber and error message
		std::string getExtendedErrorMsg() const;
	private:
		std::string mFilename;
		std::ifstream mIfs;
		std::string mLine;
		int mErrorCode;
		std::string mErrorMsg;
		unsigned int mLineNumber;

		char mIndentChar;
		unsigned int mIndentCharCount;
	};
}

#endif /* GLSLSCENE_SML_PARSER_H */

/*
 * SML - Smart Markup Language
 *
 * The first indent defines the indent character and the count of this character.
 * The indent can be one or more tabs or one or more spaces.
 * Mixing is not allowed!
 *
 * Other (similar) markup language file formats
 * https://en.wikipedia.org/wiki/JSON
 * https://en.wikipedia.org/wiki/YAML
 * https://en.wikipedia.org/wiki/INI_file
 * https://en.wikipedia.org/wiki/TOML
 * https://de.wikipedia.org/wiki/Extensible_Markup_Language
 */