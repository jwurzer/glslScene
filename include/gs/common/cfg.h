#ifndef GLSLSCENE_CFG_H
#define GLSLSCENE_CFG_H

#include <gs/common/ssize.h>
#include <string>
#include <fstream>
#include <memory>
#include <vector>

/*
 * This file contains the config classes for storing the parsed configuration
 * from a "conf" file, JSON file, BSON coding, BER/DER coding.
 */
namespace gs
{
	class CfgValuePair;
	class CfgReadRule;

	class CfgValue
	{
	public:
		enum EValueType
		{
			// see: http://www.obj-sys.com/asn1tutorial/node124.html
			// see: http://www.obj-sys.com/asn1tutorial/node11.html

			TYPE_NONE  = 0, // none, nothing, empty (empty line)
			TYPE_NULL,
			TYPE_BOOL,
			TYPE_FLOAT,
			TYPE_INT,
			TYPE_TEXT,
			TYPE_COMMENT, // use mText, e.g. # comment text
			TYPE_ARRAY,
		};

		// -1 for no line number
		int mLineNumber;
		// -1 for no offset. offset is the horizontal offset at the text line from file
		int mOffset;

		/**
		 * for CfgParser::begin() and CfgParser::getNextCfgEntry()
		 * methode one internal object from CfgEntry is used. After
		 * each call of CfgParser::getNextCfgEntry() the methode
		 * CfgEntry::clearWithoutSectionAndNumber() is called.
		 * --> All values are reset (without line number and section) after
		 * each call of getNextCfgEntry(). CfgEntry has the objects mFirst and
		 * mSecond (and also a mSection) of the class CfgValue. mFirst and
		 * mSecond will be cleared (CfgValue::clear()) by
		 * clearWithoutSectionAndNumber().
		 */

		/**
		 * if mType is TYPE_FLOAT then the float value is stored at mFloatingPoint
		 * and also stored as rounded integer (up/down rounding at .5) and if
		 * the floating point value is between +-0.5 then mBool is false and
		 * otherwise its true.
		 *
		 * if mType is TYPE_INT then the integer value is stored at mInteger and
		 * also stored as float to mFloatingPoint and if the integer value is
		 * zero then mBool is set to false otherwise its set true.
		 *
		 * if mType is TYPE_BOOL then mBool stores the value. mFloatingPoint
		 * and mInteger would be also 1.0 and 1 for true or 0.0 and 0 for false.
		 *
		 * if mType is TYPE_BOOL, TYPE_FLOAT or TYPE_INT then mText is always
		 * empty (--> "").
		 */
		EValueType mType;

		/**
		 * only for additional informations for TYPE_INT. if mType is
		 * TYPE_INT then mParseBase give infos if it was a dec, octal or
		 * hex string to parse.
		 * if mType is TYPE_FLOAT then mParseBase is always 10.
		 * if mType is TYPE_BOOL then mParseBase is always 1.
		 */
		unsigned int mParseBase;

		bool mBool;
		float mFloatingPoint;
		int mInteger;
		std::string mText;
		std::vector<CfgValuePair> mArray;

		CfgValue();
		~CfgValue();

		// return line number and offset as :<line-number>:<offset>
		// If only a line number is set then :<line-number> is returned.
		// If only a offset is set then ::<offset> is returned
		// If both line number and offset are -1 then a empty string is returned.
		std::string getFilePosition() const;
		// error happend at
		void printFilePositionAsError() const;
		void clear();
		void setNull();
		void setBool(bool value);
		void setFloatingPoint(float value);
		void setInteger(int value, unsigned int parseBase = 10);
		void setText(const std::string& text);
		void setComment(const std::string& text);
		void setArray();
		bool equalText(const std::string& text) const;

		bool isEmpty() const { return mType == CfgValue::TYPE_NONE; }
		bool isComment() const { return mType == CfgValue::TYPE_COMMENT; }
		bool isNull() const { return mType == CfgValue::TYPE_NULL; }
		bool isBool() const { return mType == CfgValue::TYPE_BOOL; }
		bool isFloat() const { return mType == CfgValue::TYPE_FLOAT; }
		bool isInteger() const { return mType == CfgValue::TYPE_INT; }
		bool isNumber() const { return isInteger() || isFloat(); }
		bool isText() const { return mType == CfgValue::TYPE_TEXT; }
		bool isNoArray() const { return mType == TYPE_NONE || mType == TYPE_NULL ||
				mType == TYPE_BOOL || mType == TYPE_FLOAT ||
				mType == TYPE_INT || mType == TYPE_TEXT ||
				mType == TYPE_COMMENT; }
		bool isArray() const {
			return mType == TYPE_ARRAY;
		}

		const CfgValuePair* sectionGetValuePair(const std::string& attrName) const;
		const CfgValue* sectionGetValue(const std::string& attrName) const;
		bool sectionGetText(const std::string& attrName, std::string& attrValue) const;
		// if not exist it return ""
		std::string sectionGetText(const std::string& attrName) const;
		bool sectionGetInteger(const std::string& attrName, int& attrValue) const;
		// if not exist it return 0
		int sectionGetInteger(const std::string& attrName) const;
		bool sectionGetBool(const std::string& attrName, bool& attrValue) const;
		bool sectionGetBool(const std::string& attrName) const;

		std::vector<const CfgValuePair*> sectionGetValuePairs(const std::string& attrName) const;
		std::vector<const CfgValue*> sectionGetValues(const std::string& attrName) const;
		std::vector<const std::string*> sectionGetTexts(const std::string& attrName) const;
		std::vector<int> sectionGetIntegers(const std::string& attrName) const;
		ssize_t sectionGetIndex(const std::string& attrName) const;
		/**
		 * @param rules The size limit is 64 without the termination rule
		 *              (64 rules + 1 termination rule)
		 * @param allowRandomSequence
		 * @param allowUnusedValuePairs
		 * @param allowEarlyReturn This value is only used wenn allowUnusedValuePairs is false.
		 *        If allowUnusedValuePairs is false and this parameter is true
		 *        then if no rule is found for the current attribute name
		 *        the function can also return successful. It return successful
		 *        if all "must exist" rules already applied and no future
		 *        attribute name has an existing rule at the rule set.
		 * @param allowDuplicatedNames
		 * @param startIndex
		 * @return Return the store count.
		 */
		ssize_t sectionGet(const CfgReadRule* rules, bool allowRandomSequence,
				bool allowUnusedValuePairs, bool allowEarlyReturn,
				bool allowDuplicatedNames,
				bool allowDuplicatedRuleNamesWithDiffTypes,
				size_t startIndex,
				size_t* outNextIndex) const;
	};

	/**
	 * A value pair has always two values. If the first value is a string then
	 * the value pair is compatible with a JSON object which must have a
	 * string and a value.
	 */
	class CfgValuePair
	{
	public:
		CfgValue mName;
		CfgValue mValue;

		CfgValuePair();
		~CfgValuePair();

		void clear();
		void setTextBool(const std::string& attrName, bool attrValue);
		void setTextFloat(const std::string& attrName, float attrValue);
		void setTextInt(const std::string& attrName, int attrValue);
		void setTextText(const std::string& attrName, const std::string& attrValue);
		void setSection();
		void setSection(const std::string& sectionName);
		bool isEmpty() const { return mName.mType == CfgValue::TYPE_NONE &&
				mValue.mType == CfgValue::TYPE_NONE; }
		bool isComment() const { return mName.mType == CfgValue::TYPE_COMMENT &&
				mValue.mType == CfgValue::TYPE_NONE; }
		bool isEmptyOrComment() const { return
				(mName.mType == CfgValue::TYPE_NONE ||
				mName.mType == CfgValue::TYPE_COMMENT) &&
				mValue.mType == CfgValue::TYPE_NONE; }
		bool isSection() const { return mName.isNoArray() &&
				mValue.isArray(); }
		bool isEmptySection() const { return mName.isEmpty() &&
				mValue.isArray() && mValue.mArray.empty(); }
	};

	class CfgReadRule
	{
	public:
		const std::string mName;

		enum EStoreType
		{
			TYPE_UNKNOWN = 0,

			TYPE_NULL,
			TYPE_BOOL,
			TYPE_FLOAT,
			TYPE_DOUBLE,
			TYPE_INT,
			TYPE_UINT,
			TYPE_STRING,
			TYPE_ARRAY,
			TYPE_VALUE_FOR_ARRAY,
			TYPE_VALUE_PAIR,
		} mType;

		union {
			void* mPtr; // used to reset pointer to null
			bool* mNull;
			bool* mBool;
			float* mFloat;
			double* mDouble;
			int* mInt;
			unsigned int* mUInt;
			std::string* mStr;
			const std::vector<CfgValuePair>** mArray;
			const CfgValue** mValueForArray;
			const CfgValuePair** mValuePair;
		} mStorePtr;

		enum ERule
		{
			RULE_OPTIONAL = 0,
			RULE_MUST_EXIST,
		} mRule;

		enum EAllowedTypes
		{
			ALLOW_NONE   =  1,
			ALLOW_NULL   =  2,
			ALLOW_BOOL   =  4,
			ALLOW_FLOAT  =  8,
			ALLOW_INT    = 16,
			ALLOW_TEXT   = 32,
			ALLOW_ARRAY  = 64,

			ALLOW_NUMBER = (ALLOW_FLOAT | ALLOW_INT),
			ALLOW_ALL    = (ALLOW_NONE | ALLOW_NULL | ALLOW_BOOL | ALLOW_FLOAT | ALLOW_INT | ALLOW_TEXT | ALLOW_ARRAY),
		};
		unsigned int mAllowedTypes; // as uint instead of AllowedTypes because its used as flags

		// stores the count how often the rule was used
		unsigned int* mUsedCount;

		CfgReadRule(const char* name) // should be always "" for EOF of rule array
				:mName(name),
				mType(TYPE_UNKNOWN), mRule(RULE_OPTIONAL),
				mAllowedTypes(0),
				mUsedCount(nullptr) {}

		CfgReadRule(const char* name, bool* boolPtr, ERule rule,
				EAllowedTypes typeFlags = ALLOW_BOOL, unsigned int* usedCount = nullptr)
				:mName(name),
				mType(TYPE_BOOL), mRule(rule),
				mAllowedTypes(typeFlags),
				mUsedCount(usedCount)
				{ mStorePtr.mBool = boolPtr; }
		CfgReadRule(const std::string& name, bool* boolPtr, ERule rule,
				EAllowedTypes typeFlags = ALLOW_BOOL, unsigned int* usedCount = nullptr)
				:mName(name),
				mType(TYPE_BOOL), mRule(rule),
				mAllowedTypes(typeFlags),
				mUsedCount(usedCount)
				{ mStorePtr.mBool = boolPtr; }

		CfgReadRule(const char* name, float* floatPtr, ERule rule,
				EAllowedTypes typeFlags = ALLOW_NUMBER, unsigned int* usedCount = nullptr)
				:mName(name),
				mType(TYPE_FLOAT), mRule(rule),
				mAllowedTypes(typeFlags),
				mUsedCount(usedCount)
				{ mStorePtr.mFloat = floatPtr; }
		CfgReadRule(const std::string& name, float* floatPtr, ERule rule,
				EAllowedTypes typeFlags = ALLOW_NUMBER, unsigned int* usedCount = nullptr)
				:mName(name),
				mType(TYPE_FLOAT), mRule(rule),
				mAllowedTypes(typeFlags),
				mUsedCount(usedCount)
				{ mStorePtr.mFloat = floatPtr; }

		CfgReadRule(const char* name, double* doublePtr, ERule rule,
				EAllowedTypes typeFlags = ALLOW_NUMBER, unsigned int* usedCount = nullptr)
				:mName(name),
				mType(TYPE_DOUBLE), mRule(rule),
				mAllowedTypes(typeFlags),
				mUsedCount(usedCount)
				{ mStorePtr.mDouble = doublePtr; }
		CfgReadRule(const std::string& name, double* doublePtr, ERule rule,
				EAllowedTypes typeFlags = ALLOW_NUMBER, unsigned int* usedCount = nullptr)
				:mName(name),
				mType(TYPE_DOUBLE), mRule(rule),
				mAllowedTypes(typeFlags),
				mUsedCount(usedCount)
				{ mStorePtr.mDouble = doublePtr; }

		CfgReadRule(const char* name, int* intPtr, ERule rule,
				EAllowedTypes typeFlags = ALLOW_INT, unsigned int* usedCount = nullptr)
				:mName(name),
				mType(TYPE_INT), mRule(rule),
				mAllowedTypes(typeFlags),
				mUsedCount(usedCount)
				{ mStorePtr.mInt = intPtr; }
		CfgReadRule(const std::string& name, int* intPtr, ERule rule,
				EAllowedTypes typeFlags = ALLOW_INT, unsigned int* usedCount = nullptr)
				:mName(name),
				mType(TYPE_INT), mRule(rule),
				mAllowedTypes(typeFlags),
				mUsedCount(usedCount)
				{ mStorePtr.mInt = intPtr; }

		CfgReadRule(const char* name, unsigned int* uintPtr, ERule rule,
				EAllowedTypes typeFlags = ALLOW_INT, unsigned int* usedCount = nullptr)
				:mName(name),
				mType(TYPE_UINT), mRule(rule),
				mAllowedTypes(typeFlags),
				mUsedCount(usedCount)
				{ mStorePtr.mUInt = uintPtr; }
		CfgReadRule(const std::string& name, unsigned int* uintPtr, ERule rule,
				EAllowedTypes typeFlags = ALLOW_INT, unsigned int* usedCount = nullptr)
				:mName(name),
				mType(TYPE_UINT), mRule(rule),
				mAllowedTypes(typeFlags),
				mUsedCount(usedCount)
				{ mStorePtr.mUInt = uintPtr; }

		CfgReadRule(const char* name, std::string* strPtr, ERule rule,
				EAllowedTypes typeFlags = ALLOW_TEXT, unsigned int* usedCount = nullptr)
				:mName(name),
				mType(TYPE_STRING), mRule(rule),
				mAllowedTypes(typeFlags),
				mUsedCount(usedCount)
				{ mStorePtr.mStr = strPtr; }
		CfgReadRule(const std::string& name, std::string* strPtr, ERule rule,
				EAllowedTypes typeFlags = ALLOW_TEXT, unsigned int* usedCount = nullptr)
				:mName(name),
				mType(TYPE_STRING), mRule(rule),
				mAllowedTypes(typeFlags),
				mUsedCount(usedCount)
				{ mStorePtr.mStr = strPtr; }

		CfgReadRule(const char* name, const std::vector<CfgValuePair>** arrayPtr, ERule rule,
				EAllowedTypes typeFlags = ALLOW_ARRAY, unsigned int* usedCount = nullptr)
				:mName(name),
				mType(TYPE_ARRAY), mRule(rule),
				mAllowedTypes(typeFlags),
				mUsedCount(usedCount)
				{ mStorePtr.mArray = arrayPtr; }
		CfgReadRule(const std::string& name, const std::vector<CfgValuePair>** arrayPtr, ERule rule,
				EAllowedTypes typeFlags = ALLOW_ARRAY, unsigned int* usedCount = nullptr)
				:mName(name),
				mType(TYPE_ARRAY), mRule(rule),
				mAllowedTypes(typeFlags),
				mUsedCount(usedCount)
				{ mStorePtr.mArray = arrayPtr; }

		CfgReadRule(const char* name, const CfgValue** arrayPtr, ERule rule,
				EAllowedTypes typeFlags = ALLOW_ARRAY, unsigned int* usedCount = nullptr)
				:mName(name),
				mType(TYPE_VALUE_FOR_ARRAY), mRule(rule),
				mAllowedTypes(typeFlags),
				mUsedCount(usedCount)
		{ mStorePtr.mValueForArray = arrayPtr; }
		CfgReadRule(const std::string& name, const CfgValue** arrayPtr, ERule rule,
				EAllowedTypes typeFlags = ALLOW_ARRAY, unsigned int* usedCount = nullptr)
				:mName(name),
				mType(TYPE_VALUE_FOR_ARRAY), mRule(rule),
				mAllowedTypes(typeFlags),
				mUsedCount(usedCount)
		{ mStorePtr.mValueForArray = arrayPtr; }

		CfgReadRule(const char* name, const CfgValuePair** valuePairPtr, ERule rule,
					EAllowedTypes typeFlags = ALLOW_ARRAY, unsigned int* usedCount = nullptr)
				:mName(name),
				 mType(TYPE_VALUE_PAIR), mRule(rule),
				 mAllowedTypes(typeFlags),
				 mUsedCount(usedCount)
		{ mStorePtr.mValuePair = valuePairPtr; }
		CfgReadRule(const std::string& name, const CfgValuePair** valuePairPtr, ERule rule,
					EAllowedTypes typeFlags = ALLOW_ARRAY, unsigned int* usedCount = nullptr)
				:mName(name),
				 mType(TYPE_VALUE_PAIR), mRule(rule),
				 mAllowedTypes(typeFlags),
				 mUsedCount(usedCount)
		{ mStorePtr.mValuePair = valuePairPtr; }
	};
}
#endif
