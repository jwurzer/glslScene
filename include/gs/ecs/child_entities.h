#ifndef GLSLSCENE_CHILD_ENTITIES_H
#define GLSLSCENE_CHILD_ENTITIES_H

#include <gs/ecs/component.h>
#include <stdint.h>
#include <vector>
#include <map>
#include <string>
#include <memory>

namespace gs
{
	class Entity;

	// ok, it inherit also because it has a mChanged and a mChanges vector
	class ChildEntities: public Component
	{
		friend class Entity;
		ChildEntities(std::weak_ptr<Entity> entity);
		ChildEntities(const ChildEntities&) = delete;
		ChildEntities& operator=(const ChildEntities&) = delete;
	public:
		static std::shared_ptr<ChildEntities> create(const std::weak_ptr<Entity>& entity);
		virtual ~ChildEntities();

		virtual EComponent getType() const { return COMPONENT_CHILD_ENTITIES; }

		// remove all childs and reset the register id of the childs to 0
		// and there parrent pointer from the child to NULL. If the childs has subchilds they
		// are not removed from the childs
		void clear();

		bool addChild(const std::shared_ptr<Entity>& entity);

		// getChild() only works for "non layered" childs
		// --> which are added by addChild(x) and not addChild(layer, x).
		const std::shared_ptr<Entity>& getChild(unsigned int index) const;
		
		// support for "normal" entities and sorted entities.
		bool replaceChild(const std::shared_ptr<Entity>& oldEntity,
				const std::shared_ptr<Entity>& newEntity);
		// TODO also support addChild by moveChild if entity is not added????
		bool removeChild(const std::shared_ptr<Entity>& entity);
		const std::vector<std::shared_ptr<Entity> >& getEntities() const;

		unsigned int count() const;
	private:
		// reset mChanged, mCleared and clear mChanges
		virtual void resetMutableChanged() const;

		std::weak_ptr<ChildEntities> mThis;
		/**
		 * a stored entity in this vector has the index:
		 * index == (mEntities[index]->mRegisterId & REG_INDEX_MASK) - 1;
		 * -1 because the register index has a +1 offset.
		 */
		std::vector<std::shared_ptr<Entity> > mEntities;

		mutable bool mCleared;
		/**
		 * register id stores different informations for ChildEntities
		 * flags/bits:
		 * 32 bits:
		 * MSB 32: 1 bit for sorted map or vector
		 *     31: 1 bit for adding
		 *     30: 1 bit for removing
		 *     29-22: 8 bit for sorting layer as signed char (only at sorted map)
		 *     21-1: 21 bit for index (or 29-1 --> 29 bit vor index at unsorted vector)
		 *
		 * If bit 31 (ADD) and bit 30 (REMOVE) is set then this means MOVE.
		 * A MOVE alone is not allowed. Always to MOVEs must be in series.
		 * The first is the MOVE entry is the "from" and the second MOVE the "to".
		 */
		// a change can be a adding, a update or a removing
		mutable std::vector<uint32_t /* register id */> mChanges;

		/**
		 * Is called by OutputEntity
		 */
		void updateChild(const Entity& entity);

		bool removeChildFromEntities(const std::shared_ptr<Entity>& entity);
		bool replaceChildAtEntities(const std::shared_ptr<Entity>& oldEntity,
				const std::shared_ptr<Entity>& newEntity);

		void registerChild(Entity& entity, uint32_t regId);
		/**
		 * The regId can include the _ADD and/or _REMOVE flags. This flags
		 * will be ignored
		 */
		std::shared_ptr<Entity> getChildPerRegisterId(uint32_t regId) const;

		static std::string getRegIdAsString(uint32_t regId);
	};
}
#endif
