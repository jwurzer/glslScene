#include <gs/common/str.h>

std::string gs::str::getStringAndRemoveEndingWithCh(const std::string& str, char ch,
		bool alsoAllowFirstCharToRemove, unsigned int* removeCount)
{
	if (str.empty() || str.back() != ch) {
		if (removeCount) {
			removeCount = 0;
		}
		return str;
	}
	int len = str.length();
	int i = len - 1;
	for (; i >= 0 && str[i] == ch; --i)
		;
	if (i < 0) {
		// the full string has always the same character
		if (alsoAllowFirstCharToRemove) {
			if (removeCount) {
				*removeCount = len;
			}
			return "";
		}
		if (removeCount) {
			*removeCount = len - 1;
		}
		char tmp[2] = {ch, '\0'};
		return tmp;
	}
	if (removeCount) {
		*removeCount = len - (i + 1);
	}
	return str.substr(0, i + 1);
}


