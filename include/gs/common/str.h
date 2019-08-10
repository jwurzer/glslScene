#ifndef GLSLSCENE_STR_H
#define GLSLSCENE_STR_H

#include <string>

namespace gs
{
	namespace str
	{
		std::string getStringAndRemoveEndingWithCh(const std::string& str, char ch,
				bool alsoAllowFirstCharToRemove, unsigned int* removeCount);
	}
}

#endif //GLSLSCENE_STR_H
