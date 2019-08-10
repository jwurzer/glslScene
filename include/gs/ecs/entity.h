#ifndef GLSLSCENE_OUTPUT_ENTITY_H
#define GLSLSCENE_OUTPUT_ENTITY_H

#include <stdint.h>
#include <memory>
#include <glm/detail/type_mat4x4.hpp>

#include <gs/common/rect_fwd.h>

namespace gs
{
	class LogicComponent;
	class ShaderComponent;
	class TextureComponent;
	class MeshComponent;
	class TransformComponent;
	class ChildEntities;

	class Entity
	{
		// is necessary because only ChildEntities should access at
		// register id (and parent pointer)
		friend class ChildEntities;
		// is necessary to allow Component to call the private methode
		// setChanged(). setChanged() is not public because this methode
		// should only be called by Component
		friend class Component;
		Entity();
		Entity(const Entity&) = delete;
		Entity& operator=(const Entity&) = delete;
	public:
		enum EName
		{
			MAX_NAME_LENGTH = 16, // incl. \0 termination --> 15 for chars
		};
		static std::shared_ptr<Entity> create();
		~Entity();

		inline bool isChanged() const { return mChanged; }

		inline const char* getName() const { return mName; }
		/*
		 * The last byte with index MAX_NAME_LENGTH - 1 is always set
		 * to '\0' (0-termination).
		 */
		void setName(const char* name);
		/*
		 * @note Only MAX_NAME_LENGTH - 1 are copied if length is higher or
		 *       equal MAX_NAME_LENGTH. If length is less than
		 *       MAX_NAME_LENGTH - 1 then the other bytes are set with zero.
		 *       The last byte with index MAX_NAME_LENGTH - 1 is always set
		 *       to zero ('\0').
		 */
		void copyBufAsName(const void* src, size_t length);

		inline bool isActive() const { return mIsActive; }
		void setActive(bool active);

		// default is 0
		void setTimeOffset(int64_t offsetTd);
		inline int64_t getTimeOffset() const { return mOffsetTd; }

		/**
		 * If the LogicComponent doesn't exist then it will be created.
		 */
		LogicComponent& logic();
		const LogicComponent* getConstLogic() const;

		/**
		 * If the ShaderComponent doesn't exist then it will be created.
		 */
		ShaderComponent& shader();
		const ShaderComponent* getConstShader() const;
		/**
		 * If the TextureComponent doesn't exist then it will be created.
		 */
		TextureComponent& texture();
		const TextureComponent* getConstTexture() const;

		/**
		 * If the MeshComponent doesn't exist then it will be created.
		 * If graphic2d() and text2d() is used then text2d() is not rendered.
		 */
		MeshComponent& mesh();
		/**
		 * @note Used by the rendering engine.
		 * @return Return the address or NULL if no Graphic2dComponent exist.
		 */
		const MeshComponent* getConstMesh() const;

		// transform2d() and transform2dEx() can be used also for audio
		TransformComponent& transform2d();
		const TransformComponent* getConstTransform() const;

		ChildEntities& childEntities();
		const ChildEntities* getConstChildEntities() const;

		void removeLogic();
		void removeShader();
		void removeTexture();
		void removeMesh();
		void removeTransform2d();
		void removeChildEntities();
		void removeAllComponents();

		/**
		 * This member function calls the member function removeChild() from
		 * its parent (if a parent exist).
		 * @return true for removing otherwise false.
		 */
		bool removeMeFromParent();

		/**
		 * reset (recursive all) changes flags. if the output entity has childs
		 * with changes and this childs will be also reset.
         */
		void resetMutableChanged() const;

		/**
		 * The transform of this entity is applied to parameter out.
		 * Parameter out will be NOT reset to a identity matrix.
         * @param ts
         * @param scrPos
         * @param out
		 * @return Return false if no transform is used --> out unchanged
         */
		bool getTransform2d(int64_t ts, const RectFloat& scrSize, glm::mat4& out) const;

		unsigned int getTransformInclParrents(int64_t ts,
				const RectFloat& scrSize, glm::mat4& out) const;

		inline bool isRegistered() const { return mRegisterId ? true : false; }

		// only for testing the ChildEntities implementation
		uint32_t getRegisterId() const { return mRegisterId; }

		unsigned int count() const;
	private:
		void setChanged();

		std::weak_ptr<Entity> mThis;
		/**
		 * register id stores different informations for ChildEntities
		 */
		uint32_t mRegisterId;
		std::weak_ptr<ChildEntities> mParent;
		// one clone source is enougth!!! because if a entity is cloned to
		// multiple entities than all the different destination entities has
		// the same source entity = clone source
		std::weak_ptr<const Entity> mCloneSource;
		mutable bool mChanged;
		char mName[MAX_NAME_LENGTH];
		bool mIsActive;
		int64_t mOffsetTd; // td for time difference (ts would be timestamp), default 0

		std::unique_ptr<LogicComponent> mLogic;
		std::unique_ptr<ShaderComponent> mShader;
		std::unique_ptr<TextureComponent> mTexture;
		std::unique_ptr<MeshComponent> mMesh;
		std::unique_ptr<TransformComponent> mTransform;
		std::shared_ptr<ChildEntities> mChildren;
	};
}

#endif
