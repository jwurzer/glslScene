#ifndef GLSLSCENE_ATTRIBUTE_H
#define GLSLSCENE_ATTRIBUTE_H

namespace gs
{
	class Attribute
	{
	public:
		std::string mName;
		unsigned int mCompOffset;
		unsigned int mCompCount;
		int mLocation;

		Attribute()
				:mName(), mCompOffset(0), mCompCount(0), mLocation(-1) {}
		Attribute(const std::string& name, unsigned int compOffset, unsigned int compCount)
				:mName(name), mCompOffset(compOffset), mCompCount(compCount), mLocation(-1) {}
	};
}

#endif //GLSLSCENE_ATTRIBUTE_H
