#include <configloader/sml_parser.h>
#include <common/cfg.h>
#include <system/log.h>
#include <string.h>
#include <stdlib.h>
#include <iostream> // only for testing

gs::SmlParser::SmlParser()
		:mFilename(), mIfs(), mLine(), mErrorCode(0), mErrorMsg(),
		mLineNumber(0),
		mIndentChar(0),
		mIndentCharCount(1)
{
}

gs::SmlParser::SmlParser(const std::string& filename)
	:SmlParser()
{
	setFilename(filename);
}

gs::SmlParser::~SmlParser()
{
	if (mIfs.is_open()) {
		mIfs.close();
	}
}

bool gs::SmlParser::setFilename(const std::string& filename)
{
	mFilename = filename;
	return true;
}

bool gs::SmlParser::begin()
{
	mLineNumber = 0;
	if (mIfs.is_open()) {
		mIfs.clear(); // clear error flags
		mIfs.seekg(0, mIfs.beg);
		if (mIfs.fail()) {
			mErrorCode = -1;
			return false;
		}
		return true;
	}
	mIfs.open(mFilename.c_str(), std::ifstream::in);
	if (mIfs.fail()) {
		mErrorCode = -2;
		return false;
	}
	return true;
}

int gs::SmlParser::getNextSmlEntry(CfgValuePair& entry)
{
	getline(mIfs, mLine);
	if (mIfs.fail()) {
		mErrorCode = -3;
		return -2;
	}
	++mLineNumber;
	return getNextSmlEntry(mLine, entry, mLineNumber);
}

int gs::SmlParser::getNextSmlEntry(std::string& utf8Line, CfgValuePair& entry,
		int lineNumber)
{
	entry.clear();
	entry.mName.mLineNumber = lineNumber;
	entry.mValue.mLineNumber = lineNumber;

	size_t len = utf8Line.length();
	if (!mIndentChar && len > 0 && (utf8Line[0] == ' ' || utf8Line[0] == '\t')) {
		mIndentChar = utf8Line[0];
		// mIndentCharCount already has the value 1 from init
		for (; mIndentCharCount < len && utf8Line[mIndentCharCount] == mIndentChar; ++mIndentCharCount)
			;
	}
	unsigned int deep = 0;
	unsigned int i = 0;
	if (mIndentChar) {
		for (; i < len && utf8Line[i] == mIndentChar; ++i)
			;
		deep = i / mIndentCharCount;
		if (i % mIndentCharCount) {
			mErrorMsg = "Wrong indention. Is not a multiple of the indention count";
			return -1;
		}
	}
	if (i >= len) {
		if (deep > 0) {
#if 0
			mErrorMsg = "Empty line with indention is not allowed.";
			return -1;
#else
			mErrorMsg = "Empty line with indentions.";
			LOGW("error: %s\n", getExtendedErrorMsg().c_str());
			// reset deep to 0 like there is no indention :-P
			deep = 0;
#endif
		}
		// a empty line with or without indention
		entry.mName.mOffset = i;
		return deep;
	}
	char ch = utf8Line[i];
	if (ch == ' ' || ch == '\t') {
		mErrorMsg = "Space/blank or tab is not allowed at the beginning after the indention.";
		return -1;
	}
	if (ch == '=') {
		mErrorMsg = "= is not allowed at the beginning after the indention.";
		return -1;
	}
	else if (ch == '#') {
		++i;
		entry.mName.setComment(utf8Line.c_str() + i);
		entry.mName.mOffset = i;
		return deep;
	}

	//unsigned int lineStartIndex = i; // start of line without indention

	unsigned int valueCount = 1;
	unsigned int wordCountPerValue = 0;
	CfgValue* array = &entry.mName;
	CfgValue* value = &entry.mName;
	while (i < len) {
		ch = utf8Line[i];
		if (ch == '=') {
			++i;
			// forward to next word beginning
			for (; i < len && (utf8Line[i] == ' ' || utf8Line[i] == '\t'); ++i)
				;
			if (i >= len) {
				mErrorMsg = "= is not allowed at the end of a line.";
				return -1;
			}
			ch = utf8Line[i];
			++valueCount;
			if (valueCount > 2) {
				mErrorMsg = "Only two values are allowed for a value pair (only one = is allowed).";
				return -1;
			}
			// now valueCount can only be 2
			array = &entry.mValue;
			value = &entry.mValue;
			wordCountPerValue = 0;
		}

		unsigned int wordStartIndex = i;
		unsigned int dotCount = 0;
		unsigned int digitCount = 0;
		bool isNumber = true;
		bool isOnlyText = false;
		if (ch == '"') {
			unsigned int di = i; // destination index / for copy
			++i;
			char prevCh = ch;
			int moveDiff = 1;
			bool isEscSeq = false;
			for (; i < len; ++i) {
				ch = utf8Line[i];
				if (ch == '"') {
					if (isEscSeq) {
						isEscSeq = false;
					}
					else {
						++i;
						isOnlyText = true;
						break;
					}
				}
				else if (ch == '\\') {
					if (!isEscSeq) {
						isEscSeq = true;
						continue;
					}
					isEscSeq = false;
				}
				else if (isEscSeq) {
					switch (ch) {
						case 't':
							ch = '\t';
							isEscSeq = false;
							break;
						case 'n':
							ch = '\n';
							isEscSeq = false;
							break;
					}
				}
				if (isEscSeq) {
					mErrorMsg = "Start escape sequence with \\ but a wrong character follows.";
					return -1;
				}
				utf8Line[di] = ch;
				prevCh = ch;
				++di;
			}
			if (!isOnlyText) {
				mErrorMsg = "No closing \" for the end of the text.";
				return -1;
			}
			// ii < len doesn't need to be checked because i can be only len but not greater.
			for (unsigned int ii = di; ii < i; ++ii) {
				utf8Line[ii] = ' ';
			}
			i = di; // set to the first blank ;-)
		}
		else {
			if (ch == '+' || ch == '-') {
				++i;
				if (i < len) {
					ch = utf8Line[i];
				}
			}
			for (; i < len && utf8Line[i] != ' ' && utf8Line[i] != '\t' &&
				   utf8Line[i] != '='; ++i) {
				ch = utf8Line[i];
				if (ch >= '0' && ch <= '9') {
					++digitCount;
				}
				else if (ch == '.') {
					++dotCount;
				}
				else {
					isNumber = false;
				}
			}
		}
		const char* word = utf8Line.c_str() + wordStartIndex;
		if (i < len) {
			ch = utf8Line[i];
			utf8Line[i] = '\0';
		}
		++wordCountPerValue;
		if (wordCountPerValue == 2) {
			CfgValue cv = *value;
			value->setArray();
			value->mArray.push_back(CfgValuePair());
			value->mArray.back().mValue = cv;
		}
		if (wordCountPerValue >= 2) {
			array->mArray.push_back(CfgValuePair());
			value = &array->mArray.back().mValue;
		}

		if (isOnlyText) {
			// it can be also a text if isOnlyText is false.
			// If isOnlyText is false it only can be a text if no other
			// cases (number, boolean, etc.) accepted.
			value->setText(word);
		}
		else if (isNumber && digitCount && dotCount == 0) {
			//std::cout << "type: int" << std::endl;
			value->setInteger(atoi(word), 10);
		}
		else if (isNumber && digitCount && dotCount == 1) {
			//std::cout << "type: float" << std::endl;
			value->setFloatingPoint(static_cast<float>(atof(word)));
		}
		else if (!strcmp(word, "true")) {
			//std::cout << "type: bool" << std::endl;
			value->setBool(true);
		}
		else if (!strcmp(word, "false")) {
			//std::cout << "type: bool" << std::endl;
			value->setBool(false);
		}
		else if (!strcmp(word, "null")) {
			//std::cout << "type: null" << std::endl;
			value->setNull();
		}
		else {
			//std::cout << "type: text" << std::endl;
			value->setText(word);
		}
		value->mLineNumber = lineNumber;
		value->mOffset = wordStartIndex;

		//std::cout << "word: '" << word << "', i: " << i << ", len: " << len << std::endl;
		if (i < len) {
			utf8Line[i] = ch;
		}

		// forward to next word beginning
		for (; i < len && (utf8Line[i] == ' ' || utf8Line[i] == '\t'); ++i)
			;
	}
	//std::cout << "TODO: " << deep << " " << utf8Line << std::endl;
	return deep;
}

bool gs::SmlParser::getAsValuePairTree(CfgValuePair& root,
		bool inclEmptyLines, bool inclComments)
{
	root.clear();
	if (!begin()) {
		mErrorMsg = "Jump to begin of file failed.";
		root.clear();
		return false;
	}
	CfgValuePair cfgPair;
	std::vector<CfgValuePair*> stack;
	stack.reserve(10);
	root.mValue.setArray();
	stack.push_back(&root);
	int deep = 0;
	int prevDeep = 0;
	while ((deep = getNextSmlEntry(cfgPair)) >= 0) {
#if 0
		if (cfgPair.isEmpty()) {
			std::cout << "is empty " << deep << std::endl;
		}
#endif
		if (deep > prevDeep) {
			if (cfgPair.isEmpty()) {
				mErrorMsg = "Increase the deep with an empty line is not allowed.";
				root.clear();
				return false;
			}
			if (deep > prevDeep + 1) {
				mErrorMsg = "Can't increase the deep more than one per entry.";
				root.clear();
				return false;
			}
			if (stack.back()->mValue.mArray.empty()) {
				mErrorMsg = "No parent entry exist (should not be possible).";
				root.clear();
				return false;
			}
			if (stack.back()->mValue.mArray.back().mName.isEmpty()) {
				mErrorMsg = "The name of the parent is empty.";
				root.clear();
				return false;
			}
			if (stack.back()->mValue.mArray.back().mName.isComment()) {
				mErrorMsg = "The name of the parent is a comment which is not allowed.";
				root.clear();
				return false;
			}
			if (!stack.back()->mValue.mArray.back().mValue.isEmpty()) {
				mErrorMsg = "The value of the parent is not empty (no = is allowed at parent).";
				root.clear();
				return false;
			}
			stack.push_back(&stack.back()->mValue.mArray.back());
			stack.back()->mValue.setArray();
			stack.back()->mValue.mLineNumber = mLineNumber;
			prevDeep = deep;
		}
		else if (deep < prevDeep && !cfgPair.isEmpty()) {
			for (int i = prevDeep; i > deep; --i) {
				stack.pop_back();
			}
			prevDeep = deep;
		}
		if (!cfgPair.isEmptyOrComment() ||
				(inclEmptyLines && cfgPair.isEmpty()) ||
				(inclComments && cfgPair.isComment())) {
			stack.back()->mValue.mArray.push_back(cfgPair);
		}
	}
	if (deep == -1) {
		root.clear();
		LOGE("parse error at line %u\n", mLineNumber);
		LOGE("error: %s\n", getExtendedErrorMsg().c_str());
		return false;
	}
	// deep should be -2 for end of file and not -1 which is a error
	return true;
}

std::string gs::SmlParser::getExtendedErrorMsg() const
{
	return mFilename + ":" + std::to_string(mLineNumber) + ": " + mErrorMsg;
}
