#ifndef GLSLSCENE_OUTPUT_COMPONENT_H
#define GLSLSCENE_OUTPUT_COMPONENT_H

#include <memory>

namespace gs
{
	class Entity;

	class Component
	{
		friend class Entity;
	public:
		enum EComponent
		{
			COMPONENT_BASE = 0, // (should not be used)
			COMPONENT_LOGIC,
			COMPONENT_SHADER,
			COMPONENT_TEXTURE,
			COMPONENT_MESH,
			COMPONENT_TRANSFORM,
			COMPONENT_CHILD_ENTITIES,

			COMPONENT_OUTPUT_ENTITY_EXTENSION,
			COMPONENT_SCREEN2D,
			COMPONENT_AUDIOSYSTEM,
		};
		Component(const std::weak_ptr<Entity>& entity);
		Component(const Component&) = delete;
		Component& operator=(const Component&) = delete;
		virtual ~Component();

		virtual EComponent getType() const = 0;
		bool isChanged() const { return mChanged; }

	protected:
		void setChanged();
		virtual void resetMutableChanged() const;
		std::shared_ptr<Entity> getEntity() const { return mEntity.lock(); }
	private:
		std::weak_ptr<Entity> const mEntity;
		mutable bool mChanged;
	};
}
#endif
