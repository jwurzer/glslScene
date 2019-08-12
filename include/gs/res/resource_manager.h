#ifndef GLSLSCENE_RESOURCE_MANAGER_H
#define GLSLSCENE_RESOURCE_MANAGER_H

#include <gs/res/resource_id.h>
#include <gs/res/texture.h>
#include <string>
#include <memory>
#include <map>

namespace gs
{
	class Resource;
	class Mesh;
	class Framebuffer;
	class ShaderProgram;
	class ShaderProgramLoadInfo;
	class FileChangeMonitoring;

	class ResourceManager
	{
	public:
		typedef std::map<std::string /* id name */, std::shared_ptr<Resource> > TResByIdNameMap;
		typedef std::map<TResourceId /* id number */, std::shared_ptr<Resource> > TResByIdNumberMap;

		ResourceManager(const std::weak_ptr<FileChangeMonitoring>& fcm,
				bool useVaoVersionForMesh);

		TTextureId addTexture(const std::string& idName,
				const std::string& filename, TexMipmap mipmap,
				TexFilter minFilter, TexFilter magFilter);
		TShaderId addShaderProgram(const std::string& idName,
				const ShaderProgramLoadInfo& shaderProgramLoadInfo);
		TResourceId addResource(const std::string& idName,
				const std::shared_ptr<Resource>& res);
		std::string toString() const;

		TResourceId getResourceId(const std::string& idName) const;
		std::shared_ptr<Resource> getResourceByIdName(const std::string& idName) const;
		std::shared_ptr<Resource> getResourceByIdNumber(TResourceId idNumber) const;

		std::shared_ptr<Texture> getTextureByIdName(const std::string& idName) const;
		std::shared_ptr<Texture> getTextureByIdNumber(TResourceId idNumber) const;

		std::shared_ptr<ShaderProgram> getShaderProgramByIdName(const std::string& idName) const;
		std::shared_ptr<ShaderProgram> getShaderProgramByIdNumber(TResourceId idNumber) const;

		std::shared_ptr<Mesh> getMeshByIdName(const std::string& idName) const;
		std::shared_ptr<Mesh> getMeshByIdNumber(TResourceId idNumber) const;

		std::shared_ptr<Framebuffer> getFramebufferByIdName(const std::string& idName) const;
		std::shared_ptr<Framebuffer> getFramebufferByIdNumber(TResourceId idNumber) const;

		bool useVaoVersionForMesh() const { return mUseVaoVersionForMesh; }

		unsigned int getResourceCount() const { return mResByIdNumber.size(); }

		const TResByIdNameMap& getResourceMapByIdName() const { return mResByIdName; }
		const TResByIdNumberMap&  getResourceMapByIdNumber() const { return mResByIdNumber; }
	private:
		std::weak_ptr<FileChangeMonitoring> mFileMonitoring;
		bool mUseVaoVersionForMesh;

		TResByIdNameMap mResByIdName;
		TResByIdNumberMap mResByIdNumber;

		TResourceId mNextFreeResourceId;
	};
}

#endif
