
#include <ecs/component.h>
#include <ecs/entity.h>

gs::Component::Component(const std::weak_ptr<Entity>& entity)
		:mEntity(entity), mChanged(false)
{
}

gs::Component::~Component()
{
}

void gs::Component::setChanged()
{
	if (mChanged) {
		return; // nothing to do
	}
	mChanged = true;
	std::shared_ptr<Entity> entity(mEntity.lock());
	if (entity) {
		entity->setChanged();
	}
}

void gs::Component::resetMutableChanged() const
{
	mChanged = false;
}

