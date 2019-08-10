#include <gs/res/resource_manager.h>
#include <gs/res/shader_program.h>
#include <gs/res/mesh.h>
#include <gs/res/framebuffer.h>
#include <gs/system/log.h>

gs::ResourceManager::ResourceManager(const std::weak_ptr<FileChangeMonitoring>& fcm,
		bool useVaoVersionForMesh)
		:mFileMonitoring(fcm), mUseVaoVersionForMesh(useVaoVersionForMesh),
		mResByIdName(), mResByIdNumber(), mNextFreeResourceId(2)
{
}

gs::TTextureId gs::ResourceManager::addTexture(const std::string &idName,
		const std::string &filename, gs::TexMipmap mipmap,
		gs::TexFilter minFilter, gs::TexFilter magFilter)
{
	std::shared_ptr<Texture> tex = std::make_shared<Texture>(mFileMonitoring, filename, mipmap, minFilter, magFilter);
	if (!tex->load()) {
		LOGE("Load texture failed\n");
		return 0;
	}

	return addResource(idName, tex);
}

gs::TShaderId gs::ResourceManager::addShaderProgram(const std::string& idName,
		const ShaderProgramLoadInfo& shaderProgramLoadInfo)
{
	std::shared_ptr<ShaderProgram> shaderProg = std::make_shared<ShaderProgram>(mFileMonitoring, shaderProgramLoadInfo);
	if (!shaderProg->load()) {
		LOGE("Load and compile shader failed\n");
		return 0;
	}

	return addResource(idName, shaderProg);
}

gs::TResourceId gs::ResourceManager::addResource(const std::string &idName,
		const std::shared_ptr<gs::Resource> &res)
{
	TResByIdNameMap::const_iterator itByName = mResByIdName.find(idName);
	if (itByName != mResByIdName.cend()) {
		LOGE("id name '%s' is already used for a resource.\n", idName.c_str());
		return 0;
	}
	TResourceId idNumber = mNextFreeResourceId;
	++mNextFreeResourceId;

	res->setIdName(idName);
	res->setIdNumber(idNumber);

	mResByIdName[idName] = res;
	mResByIdNumber[idNumber] = res;
	return idNumber;
}

std::string gs::ResourceManager::toString() const
{
	std::stringstream s;
	for (const auto& r : mResByIdNumber) {
		const auto& res = *r.second;
		s << "id: " << res.getIdNumber() <<
				", id name: " << res.getIdName() <<
				", type: ";
		switch (r.second->getType()) {
			case ResType::TEXTURE:
				s << "texture";
				break;
			case ResType::SHADER_PROGRAM:
				s << "shader program";
				break;
			case ResType::MESH:
				s << "mesh";
				break;
			case ResType::FRAMEBUFFER:
				s << "framebuffer";
				break;
		}
		s << "\n";
	}
	return s.str();
}

gs::TResourceId gs::ResourceManager::getResourceId(const std::string& idName) const
{
	TResByIdNameMap::const_iterator it = mResByIdName.find(idName);
	if (it == mResByIdName.cend()) {
		return 0;
	}
	return it->second->getIdNumber();
}

std::shared_ptr<gs::Resource> gs::ResourceManager::getResourceByIdName(const std::string& idName) const
{
	TResByIdNameMap::const_iterator it = mResByIdName.find(idName);
	if (it == mResByIdName.cend()) {
		return std::shared_ptr<gs::Resource>();
	}
	return it->second;
}

std::shared_ptr<gs::Resource> gs::ResourceManager::getResourceByIdNumber(TResourceId idNumber) const
{
	TResByIdNumberMap::const_iterator it = mResByIdNumber.find(idNumber);
	if (it == mResByIdNumber.cend()) {
		return std::shared_ptr<gs::Resource>();
	}
	return it->second;
}

std::shared_ptr<gs::Texture> gs::ResourceManager::getTextureByIdName(const std::string& idName) const
{
	std::shared_ptr<Resource> res = getResourceByIdName(idName);
	if (!res) {
		return std::shared_ptr<Texture>();
	}
	if (res->getType() == ResType::FRAMEBUFFER) {
		return std::static_pointer_cast<Framebuffer>(res)->getTexRes();
	}
	if (res->getType() != ResType::TEXTURE) {
		return std::shared_ptr<Texture>();
	}
	return std::static_pointer_cast<Texture>(res);
}

std::shared_ptr<gs::Texture> gs::ResourceManager::getTextureByIdNumber(TResourceId idNumber) const
{
	std::shared_ptr<Resource> res = getResourceByIdNumber(idNumber);
	if (!res) {
		return std::shared_ptr<Texture>();
	}
	if (res->getType() == ResType::FRAMEBUFFER) {
		return std::static_pointer_cast<Framebuffer>(res)->getTexRes();
	}
	if (res->getType() != ResType::TEXTURE) {
		return std::shared_ptr<Texture>();
	}
	return std::static_pointer_cast<Texture>(res);
}

std::shared_ptr<gs::ShaderProgram> gs::ResourceManager::getShaderProgramByIdName(const std::string& idName) const
{
	std::shared_ptr<Resource> res = getResourceByIdName(idName);
	if (!res || res->getType() != ResType::SHADER_PROGRAM) {
		return std::shared_ptr<ShaderProgram>();
	}
	return std::static_pointer_cast<ShaderProgram>(res);
}

std::shared_ptr<gs::ShaderProgram> gs::ResourceManager::getShaderProgramByIdNumber(TResourceId idNumber) const
{
	std::shared_ptr<Resource> res = getResourceByIdNumber(idNumber);
	if (!res || res->getType() != ResType::SHADER_PROGRAM) {
		return std::shared_ptr<ShaderProgram>();
	}
	return std::static_pointer_cast<ShaderProgram>(res);
}

std::shared_ptr<gs::Mesh> gs::ResourceManager::getMeshByIdName(const std::string& idName) const
{
	std::shared_ptr<Resource> res = getResourceByIdName(idName);
	if (!res || res->getType() != ResType::MESH) {
		return std::shared_ptr<Mesh>();
	}
	return std::static_pointer_cast<Mesh>(res);
}

std::shared_ptr<gs::Mesh> gs::ResourceManager::getMeshByIdNumber(TResourceId idNumber) const
{
	std::shared_ptr<Resource> res = getResourceByIdNumber(idNumber);
	if (!res || res->getType() != ResType::MESH) {
		return std::shared_ptr<Mesh>();
	}
	return std::static_pointer_cast<Mesh>(res);
}

std::shared_ptr<gs::Framebuffer> gs::ResourceManager::getFramebufferByIdName(const std::string& idName) const
{
	std::shared_ptr<Resource> res = getResourceByIdName(idName);
	if (!res || res->getType() != ResType::FRAMEBUFFER) {
		return std::shared_ptr<Framebuffer>();
	}
	return std::static_pointer_cast<Framebuffer>(res);
}

std::shared_ptr<gs::Framebuffer> gs::ResourceManager::getFramebufferByIdNumber(TResourceId idNumber) const
{
	std::shared_ptr<Resource> res = getResourceByIdNumber(idNumber);
	if (!res || res->getType() != ResType::FRAMEBUFFER) {
		return std::shared_ptr<Framebuffer>();
	}
	return std::static_pointer_cast<Framebuffer>(res);
}
