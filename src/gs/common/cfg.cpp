#include <gs/common/cfg.h>
#include <gs/system/log.h>
#include <cmath>
#include <string.h>

namespace gs
{
	namespace
	{
		// return -1 for not found or not allowed
		ssize_t getRuleIndex(const std::string& ruleName, size_t curRuleIndex,
				const CfgReadRule* rules, size_t rulesSize,
				bool allowRandomSequence)
		{
			if (curRuleIndex < rulesSize && ruleName == rules[curRuleIndex].mName) {
				return curRuleIndex;
			}
			// first search after the current rule index is also allowed
			// if random sequence is false because the sequence is unchanged.
			// Only some rules can be skipped.
			// If a rule should not be skipped then CfgReadRule::RULE_MUST_EXIST
			// should be used.
			for (size_t i = curRuleIndex + 1; i < rulesSize; i++) {
				if (ruleName == rules[i].mName) {
					return i;
				}
			}
			if (!allowRandomSequence) {
				return -1; // no other rule allowed
			}
			// not found --> search before the current rule index
			for (size_t i = 0; i < curRuleIndex; i++) {
				if (ruleName == rules[i].mName) {
					return i;
				}
			}
			return -1; // no rule found
		}
	}
}

gs::CfgValue::CfgValue()
		:mLineNumber(-1), mOffset(-1),
		mType(TYPE_NONE),
		mParseBase(0),
		mBool(false),
		mFloatingPoint(0.0f),
		mInteger(0),
		mText(""),
		mArray()
{
}

gs::CfgValue::~CfgValue()
{
}

std::string gs::CfgValue::getFilePosition() const
{
	char str[30] = "";
	if (mLineNumber >= 0 && mOffset >= 0) {
		snprintf(str, 30, ":%d:%d", mLineNumber, mOffset);
	}
	else if (mLineNumber >= 0) {
		snprintf(str, 30, ":%d", mLineNumber);
	}
	else if (mOffset >= 0) {
		snprintf(str, 30, "::%d", mOffset);
	}
	// else --> both mLineNumber and mOffset are -1 --> using empty string
	return str;
}

void gs::CfgValue::printFilePositionAsError() const
{
	LOGE("Error happend at %s\n", getFilePosition().c_str());
}

void gs::CfgValue::clear()
{
	mLineNumber = -1;
	mOffset = -1;

	mType = TYPE_NONE;
	mParseBase = 0;
	mBool = false;
	mFloatingPoint = 0.0;
	mInteger = 0;
	mText = "";
	//mArray.clear();
	mArray.clear();
}

void gs::CfgValue::setNull()
{
	clear();
	mType = TYPE_NULL;
}

void gs::CfgValue::setBool(bool value)
{
	clear();
	mType = TYPE_BOOL;
	mParseBase = 1;
	mBool = value;
	mFloatingPoint = static_cast<float>(value);
	mInteger = static_cast<int>(value);
}

void gs::CfgValue::setFloatingPoint(float value)
{
	clear();
	mType = TYPE_FLOAT;
	mParseBase = 10;
	mBool = (std::fpclassify(value) != FP_ZERO);
	mFloatingPoint = value;
	mInteger = static_cast<int>(value);
}

void gs::CfgValue::setInteger(int value, unsigned int parseBase)
{
	clear();
	mType = TYPE_INT;
	mParseBase = parseBase;
	mBool = !!value;
	mFloatingPoint = static_cast<float>(value);
	mInteger = value;
}

void gs::CfgValue::setText(const std::string& text)
{
	clear();
	mType = TYPE_TEXT;
	mText = text;
}

void gs::CfgValue::setComment(const std::string& text)
{
	clear();
	mType = TYPE_COMMENT;
	mText = text;
}

void gs::CfgValue::setArray()
{
	clear();
	mType = TYPE_ARRAY;
	mArray.clear();
}

bool gs::CfgValue::equalText(const std::string& text) const
{
	return (mType == TYPE_TEXT && mText == text) ? true : false;
}

const gs::CfgValuePair* gs::CfgValue::sectionGetValuePair(const std::string& attrName) const
{
	unsigned int pairCount = mArray.size();
	unsigned int foundCount = 0;
	const CfgValuePair* valuePair = NULL;

	for (unsigned int i = 0; i < pairCount; i++) {
		if (mArray[i].mName.mType != CfgValue::TYPE_TEXT) {
			continue;
		}
		if (mArray[i].mName.mText != attrName) {
			continue;
		}
		foundCount++;
		valuePair = &mArray[i];
	}
	if (foundCount != 1) {
		return NULL;
	}
	return valuePair;
}

const gs::CfgValue* gs::CfgValue::sectionGetValue(const std::string& attrName) const
{
	const CfgValuePair* valuePair = sectionGetValuePair(attrName);
	if (!valuePair) {
		return NULL;
	}
	return &valuePair->mValue;
}

bool gs::CfgValue::sectionGetText(const std::string& attrName, std::string& attrValue) const
{
	const CfgValue* value = sectionGetValue(attrName);
	if (!value) {
		return false;
	}
	if (value->mType != CfgValue::TYPE_TEXT) {
		return false;
	}
	attrValue = value->mText;
	return true;
}

std::string gs::CfgValue::sectionGetText(const std::string& attrName) const
{
	std::string attrValue;
	if (!sectionGetText(attrName, attrValue)) {
		return "";
	}
	return attrValue;
}

bool gs::CfgValue::sectionGetInteger(const std::string& attrName, int& attrValue) const
{
	const CfgValue* value = sectionGetValue(attrName);
	if (!value) {
		return false;
	}
	if (value->mType != CfgValue::TYPE_INT) {
		return false;
	}
	attrValue = value->mInteger;
	return true;
}

int gs::CfgValue::sectionGetInteger(const std::string& attrName) const
{
	int attrValue = 0;
	if (!sectionGetInteger(attrName, attrValue)) {
		return 0;
	}
	return attrValue;
}

bool gs::CfgValue::sectionGetBool(const std::string& attrName, bool& attrValue) const
{
	const CfgValue* value = sectionGetValue(attrName);
	if (!value) {
		return false;
	}
	if (value->mType != CfgValue::TYPE_BOOL) {
		return false;
	}
	attrValue = value->mBool;
	return true;
}

bool gs::CfgValue::sectionGetBool(const std::string& attrName) const
{
	bool attrValue = 0;
	if (!sectionGetBool(attrName, attrValue)) {
		return false;
	}
	return attrValue;
}

std::vector<const gs::CfgValuePair*> gs::CfgValue::sectionGetValuePairs(
		const std::string& attrName) const
{
	std::vector<const gs::CfgValuePair*> pairs;
	unsigned int pairCount = mArray.size();

	for (unsigned int i = 0; i < pairCount; i++) {
		if (mArray[i].mName.mType != CfgValue::TYPE_TEXT) {
			continue;
		}
		if (mArray[i].mName.mText != attrName) {
			continue;
		}
		pairs.push_back(&mArray[i]);
	}
	return pairs;
}

std::vector<const gs::CfgValue*> gs::CfgValue::sectionGetValues(
		const std::string& attrName) const
{
	std::vector<const gs::CfgValue*> values;
	unsigned int pairCount = mArray.size();

	for (unsigned int i = 0; i < pairCount; i++) {
		if (mArray[i].mName.mType != CfgValue::TYPE_TEXT) {
			continue;
		}
		if (mArray[i].mName.mText != attrName) {
			continue;
		}
		values.push_back(&mArray[i].mValue);
	}
	return values;
}

std::vector<const std::string*> gs::CfgValue::sectionGetTexts(
		const std::string& attrName) const
{
	std::vector<const CfgValue*> values = sectionGetValues(attrName);
	std::vector<const std::string*> texts;
	texts.reserve(values.size());
	for (size_t i = 0; i < values.size(); i++) {
		if (values[i]->mType != CfgValue::TYPE_TEXT) {
			continue;
		}
		texts.push_back(&values[i]->mText);
	}
	return texts;
}

std::vector<int> gs::CfgValue::sectionGetIntegers(const std::string& attrName) const
{
	std::vector<const CfgValue*> values = sectionGetValues(attrName);
	std::vector<int> ints;
	ints.reserve(values.size());
	for (size_t i = 0; i < values.size(); i++) {
		if (values[i]->mType != CfgValue::TYPE_INT) {
			continue;
		}
		ints.push_back(values[i]->mInteger);
	}
	return ints;
}

ssize_t gs::CfgValue::sectionGetIndex(const std::string& attrName) const
{
	unsigned int pairCount = mArray.size();

	for (unsigned int i = 0; i < pairCount; i++) {
		if (mArray[i].mName.mType != CfgValue::TYPE_TEXT) {
			continue;
		}
		if (mArray[i].mName.mText != attrName) {
			continue;
		}
		return i;
	}
	return -1;
}

ssize_t gs::CfgValue::sectionGet(const CfgReadRule* rules, bool allowRandomSequence,
		bool allowUnusedValuePairs, bool allowEarlyReturn, bool allowDuplicatedNames,
		bool allowDuplicatedRuleNamesWithDiffTypes,
		size_t startIndex, size_t* outNextIndex) const
{
	size_t rulesSize = 0;
	unsigned int finalMustExistCount = 0;
	unsigned int currentMustExistCount = 0;
	while (rules[rulesSize].mType != CfgReadRule::TYPE_UNKNOWN) {
		if (rules[rulesSize].mStorePtr.mPtr) {
			switch (rules[rulesSize].mType) {
				case CfgReadRule::TYPE_UNKNOWN:
					break;
				case CfgReadRule::TYPE_NULL:
					*rules[rulesSize].mStorePtr.mNull = false;
					break;
				case CfgReadRule::TYPE_BOOL:
					*rules[rulesSize].mStorePtr.mBool = false;
					break;
				case CfgReadRule::TYPE_FLOAT:
					*rules[rulesSize].mStorePtr.mFloat = 0.0f;
					break;
				case CfgReadRule::TYPE_DOUBLE:
					*rules[rulesSize].mStorePtr.mDouble = 0.0;
					break;
				case CfgReadRule::TYPE_INT:
					*rules[rulesSize].mStorePtr.mInt = 0;
					break;
				case CfgReadRule::TYPE_UINT:
					*rules[rulesSize].mStorePtr.mUInt = 0;
					break;
				case CfgReadRule::TYPE_STRING:
					rules[rulesSize].mStorePtr.mStr->clear();
					break;
				case CfgReadRule::TYPE_ARRAY:
					*rules[rulesSize].mStorePtr.mArray = nullptr;
					break;
				case CfgReadRule::TYPE_VALUE_FOR_ARRAY:
					*rules[rulesSize].mStorePtr.mValueForArray = nullptr;
					break;
				case CfgReadRule::TYPE_VALUE_PAIR:
					*rules[rulesSize].mStorePtr.mValuePair = nullptr;
					break;
			}
		}
		if (rules[rulesSize].mUsedCount) {
			*rules[rulesSize].mUsedCount = 0;
		}
		if (rules[rulesSize].mRule == CfgReadRule::RULE_MUST_EXIST) {
			++finalMustExistCount;
		}
		++rulesSize;
	}

	size_t pairCount = mArray.size();
	size_t curRuleIndex = 0;
	size_t storeCount = 0;

	size_t usedRuleCounts[64];
	memset(usedRuleCounts, 0, sizeof(usedRuleCounts));
	if (rulesSize > 64) {
		LOGE("Too many config rules (%zu > 64)\n", rulesSize);
		return -1;
	}

	size_t i = startIndex;
	bool prevTypeWasWrong = false;
	for (; i < pairCount && storeCount < rulesSize; ++i) {
		if (prevTypeWasWrong) {
			--i; // revert the ++i from loop
		}
		const CfgValuePair& vp = mArray[i];

		if (vp.isEmpty() || vp.isComment()) {
			continue;
		}

		const std::string& vpAttrName =
				(vp.mName.mType == CfgValue::TYPE_TEXT) ?
				vp.mName.mText : "";

		// find the correct rule
		ssize_t ri = getRuleIndex(vpAttrName, curRuleIndex,
				rules, rulesSize, allowRandomSequence && !prevTypeWasWrong);
		// now we can reset prevTypeWasWrong (if it was set). Before its not
		// possible because getRuleIndex() need this variable!
		prevTypeWasWrong = false;
		if (ri < 0) {
			// no rule found for this attr name
			if (allowUnusedValuePairs) {
				continue;
			}
			if (allowEarlyReturn && finalMustExistCount == currentMustExistCount) {
				size_t ii = i;
				for (; i < pairCount && storeCount < rulesSize; ++i) {
					const CfgValuePair& vp = mArray[i];
					const std::string& vpAttrName =
							(vp.mName.mType == CfgValue::TYPE_TEXT) ?
							vp.mName.mText : "";
					if (getRuleIndex(vpAttrName, curRuleIndex,
							rules, rulesSize, allowRandomSequence && !prevTypeWasWrong) >= 0) {
						return -2;
					}
					++curRuleIndex; // no problem if out of range because getRuleIndex() check this
				}
				if (outNextIndex) {
					*outNextIndex = ii;
				}
				return storeCount;
			}
			LOGI("no rule found for attr name '%s'\n", vpAttrName.c_str());
			return -3;
		}
		curRuleIndex = ri;
		++usedRuleCounts[curRuleIndex];
		if (rules[curRuleIndex].mUsedCount) {
			++*rules[curRuleIndex].mUsedCount;
		}
		// check if it was the first time that the rule was used and if the rule must be used
		if (usedRuleCounts[curRuleIndex] == 1 &&
				rules[curRuleIndex].mRule == CfgReadRule::RULE_MUST_EXIST) {
			++currentMustExistCount;
		}
		if (usedRuleCounts[curRuleIndex] > 1) {
			if (allowDuplicatedNames) {
				curRuleIndex++; // no problem if out of range because getRuleIndex() check this
				continue; // ignore duplicates --> jump to next
			}
			return -4;
		}

		const CfgReadRule& rule = rules[curRuleIndex];
		static unsigned int lookup[] = {
			CfgReadRule::ALLOW_NONE,  // for CfgValue::TYPE_NONE  = 0
			CfgReadRule::ALLOW_NULL,  // for CfgValue::TYPE_NULL  = 1
			CfgReadRule::ALLOW_BOOL,  // for CfgValue::TYPE_BOOL  = 2
			CfgReadRule::ALLOW_FLOAT, // for CfgValue::TYPE_FLOAT = 3
			CfgReadRule::ALLOW_INT,   // for CfgValue::TYPE_INT   = 4
			CfgReadRule::ALLOW_TEXT,  // for CfgValue::TYPE_TEXT  = 5
			0, // ALLOW_COMMENT not exist // for CfgValue::TYPE_COMMENT = 6
			CfgReadRule::ALLOW_ARRAY, // for CfgValue::TYPE_ARRAY
		};
		if (!(rule.mAllowedTypes & lookup[vp.mValue.mType])) {
			if (allowDuplicatedRuleNamesWithDiffTypes) {
				prevTypeWasWrong = true;
				// revert rule count because this rule was not used (because of wrong type)

				// check if it was the first time that the rule was used and if the rule must be used
				// then the "current must exist count" must also be reverted
				if (usedRuleCounts[curRuleIndex] == 1 &&
						rules[curRuleIndex].mRule == CfgReadRule::RULE_MUST_EXIST) {
					--currentMustExistCount;
				}
				--usedRuleCounts[curRuleIndex];
				if (rules[curRuleIndex].mUsedCount) {
					--*rules[curRuleIndex].mUsedCount;
				}
				curRuleIndex++; // no problem if out of range because getRuleIndex() check this
				continue;
			}
			LOGE("Type is not allowed. allowed %u, lookup %u, type %u\n",
					rule.mAllowedTypes, lookup[vp.mValue.mType], vp.mValue.mType);
			return -5;
		}
		switch (rule.mType) {
			case CfgReadRule::TYPE_UNKNOWN:
				return -1; // unknown is not allowed
			case CfgReadRule::TYPE_NULL:
				*rule.mStorePtr.mNull = true;
				break;
			case CfgReadRule::TYPE_BOOL:
				*rule.mStorePtr.mBool = vp.mValue.mBool;
				break;
			case CfgReadRule::TYPE_FLOAT:
				*rule.mStorePtr.mFloat = vp.mValue.mFloatingPoint;
				break;
			case CfgReadRule::TYPE_DOUBLE:
				*rule.mStorePtr.mDouble = vp.mValue.mFloatingPoint;
				break;
			case CfgReadRule::TYPE_INT:
				*rule.mStorePtr.mInt = vp.mValue.mInteger;
				break;
			case CfgReadRule::TYPE_UINT:
				*rule.mStorePtr.mUInt = (vp.mValue.mInteger >= 0) ?
						vp.mValue.mInteger : 0;
				break;
			case CfgReadRule::TYPE_STRING:
				*rule.mStorePtr.mStr = vp.mValue.mText;
				break;
			case CfgReadRule::TYPE_ARRAY:
				*rule.mStorePtr.mArray = &vp.mValue.mArray;
				break;
			case CfgReadRule::TYPE_VALUE_FOR_ARRAY:
				*rule.mStorePtr.mValueForArray = &vp.mValue;
				break;
			case CfgReadRule::TYPE_VALUE_PAIR:
				*rule.mStorePtr.mValuePair = &vp;
				break;
		}
		curRuleIndex++; // no problem if out of range because getRuleIndex() check this
		storeCount++;
	}

	// now check if the rules are complied
#if 1
	for (size_t ri = 0; ri < rulesSize; ri++) {
		if (rules[ri].mRule == CfgReadRule::RULE_MUST_EXIST && !usedRuleCounts[ri]) {
			return -6;
		}
	}
#endif
	// this version should be enougth. Loop above is not necessary.
	if (currentMustExistCount < finalMustExistCount) {
		return -7;
	}
	if (outNextIndex) {
		*outNextIndex = i;
	}
	return storeCount;
}

//-----------------------------------------------

gs::CfgValuePair::CfgValuePair()
{
}

gs::CfgValuePair::~CfgValuePair()
{
}

void gs::CfgValuePair::clear()
{
	mName.clear();
	mValue.clear();
}

void gs::CfgValuePair::setTextBool(const std::string& attrName, bool attrValue)
{
	mName.setText(attrName);
	mValue.setBool(attrValue);
}

void gs::CfgValuePair::setTextFloat(const std::string& attrName, float attrValue)
{
	mName.setText(attrName);
	mValue.setFloatingPoint(attrValue);
}

void gs::CfgValuePair::setTextInt(const std::string& attrName, int attrValue)
{
	mName.setText(attrName);
	mValue.setInteger(attrValue);
}

void gs::CfgValuePair::setTextText(const std::string& attrName, const std::string& attrValue)
{
	mName.setText(attrName);
	mValue.setText(attrValue);
}

void gs::CfgValuePair::setSection()
{
	mName.clear();
	mValue.setArray();
}

void gs::CfgValuePair::setSection(const std::string& sectionName)
{
	mName.setText(sectionName);
	mValue.setArray();
}

