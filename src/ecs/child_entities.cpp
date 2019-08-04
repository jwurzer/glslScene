
#include <ecs/child_entities.h>

#include <ecs/entity.h>
#include <system/log.h>
#include <sstream>

//#define REG_FLAG_SORTED (1 << 31) // child of sorted entities (map used, not vector)
#define REG_FLAG_ADD    (1 << 30)
#define REG_FLAG_REMOVE (1 << 29)
#define REG_MOVE (REG_FLAG_ADD | REG_FLAG_REMOVE)

#define REG_UPDATE_MASK REG_MOVE // same as REG_MOVE because this has set both flags

#define REG_INDEX_MASK 0x1FFFFFFF // mask for index of "non layer" entities

//#define REG_LAYER_MASK 0x1FE00000 // for sorted entities (map used)
//#define REG_LAYER_SHIFT 21 // for sorted entities (map used)

//#define REG_LAYER_INDEX_MASK 0x1FFFFF // mask for index inside one layer (map used)

using namespace std;

namespace gs {
	namespace {
		// is used to be able to return a null pointer reference (see getChild())
		std::shared_ptr<Entity> nullOePtr;
	}
}

std::shared_ptr<gs::ChildEntities> gs::ChildEntities::create(
		const std::weak_ptr<Entity>& entity)
{
	std::shared_ptr<ChildEntities> ce(new ChildEntities(entity));
	ce->mThis = ce;
	return ce;
}

gs::ChildEntities::ChildEntities(std::weak_ptr<Entity> entity)
		:Component(entity), mThis(), mEntities(),
		mCleared(false), mChanges()
{
}

gs::ChildEntities::~ChildEntities()
{
	clear();
}

void gs::ChildEntities::clear()
{
	for (unsigned int i = 0; i < mEntities.size(); i++) {
		mEntities[i]->mRegisterId = 0;
		mEntities[i]->mParent = std::weak_ptr<ChildEntities>();
	}
	mEntities.clear();

	mChanges.clear();
	//std::vector<std::shared_ptr<Entity> >().swap(mEntities);

	mCleared = true;
	setChanged();
}

bool gs::ChildEntities::addChild(const std::shared_ptr<Entity>& entity)
{
	if (entity->mRegisterId) {
		LOGSE << "child entities: output entity already added" << endl;
		return false;
	}
	if (mEntities.size() >= 0x1FFFFFFF) { // >= instead of > because of +1 index (because 0 is not allowed)
		LOGSE << "child entities: no free index" << endl;
		return false;
	}
	mEntities.push_back(entity);
	// must be done after adding. because the index should have +1 offset --> 0 index not possible
	registerChild(*entity, mEntities.size()); // also add to mChanges
	setChanged();

	if (entity->isChanged()) {
		updateChild(*entity);
	}
	return true;
}

const std::shared_ptr<gs::Entity>& gs::ChildEntities::getChild(unsigned int index) const
{
	if (index >= mEntities.size()) {
		return nullOePtr;
	}
	return mEntities[index];
}

bool gs::ChildEntities::replaceChild(const std::shared_ptr<Entity>& oldEntity,
		const std::shared_ptr<Entity>& newEntity)
{
	uint32_t oldRegId = oldEntity->mRegisterId;
	if (!oldRegId) {
		LOGSE << "child entities: old output entity is not a child --> replacing failed" << endl;
		return false;
	}
	if (newEntity->mRegisterId) {
		LOGSE << "child entities: new output entity is already a child --> replacing failed" << endl;
		return false;
	}

	bool replaced = replaceChildAtEntities(oldEntity, newEntity);
	if (!replaced) {
		LOGSE << "child entities: replacing output entities failed" << endl;
		return false;
	}

	// search UPDATES from the last element of mChanges to until ADD, MOVE or
	// start. If a update is found --> no new update must be added
	bool foundUpdate = false;
	for (unsigned int i = mChanges.size(); i > 0; ) {
		i--;
		if (mChanges[i] == (oldRegId | REG_FLAG_ADD) ||
				mChanges[i] == (oldRegId | REG_MOVE)) {
			break;
		}

		if ((mChanges[i] & REG_UPDATE_MASK) == REG_MOVE) {
			// if there is a MOVE (to) --> then the first MOVE (from) can/should be skipped
			i--;
			continue; // its a MOVE --> can't be a UPDATE --> jump to next
		}
		
		if (mChanges[i] == oldRegId /* --> UPDATE */) {
			foundUpdate = true;
			break;
		}
	}

	if (!foundUpdate) {
		// no UPDATE was found --> add a UPDATE
		// for a replace, a UPDATE is enough because the address of the
		// pointer has changed --> clone will force a "full clone"
		mChanges.push_back(oldRegId); // for update no additional flag is necessary
	}
	newEntity->mRegisterId = oldRegId;
	newEntity->mParent = mThis; // is the same as oldEntity->mParrent
	oldEntity->mRegisterId = 0;
	oldEntity->mParent = std::weak_ptr<ChildEntities>();
	setChanged();
	return true;
}

bool gs::ChildEntities::removeChild(const std::shared_ptr<Entity>& entity)
{
	if (!entity->mRegisterId) {
		LOGSE << "child entities: output entity is not a child --> removing failed" << endl;
		return false;
	}

	bool removed = removeChildFromEntities(entity);
	if (!removed) {
		LOGSE << "child entities: removing of output entity failed" << endl;
		return false;
	}

#if 1 // Code does handle special cases!!! see explanation below the loop
	// implemenation.
	// Maybe this code should be tested with unit tests ;-) ...

	// remove all UPDATES from REMOVE (which will be added after this loop)
	// to until ADD, MOVE or start.
	// Its a reverse loop. This is important because the rid must be adapted
	// if REMOVEs from other childs between (see explanation below).
	uint32_t rid = entity->mRegisterId;
	int count = 0;
	for (unsigned int i = mChanges.size(); i > 0; ) {
		i--;
		if (mChanges[i] == (rid | REG_FLAG_ADD) ||
				mChanges[i] == (rid | REG_MOVE)) {
			break;
		}

		if ((mChanges[i] & REG_UPDATE_MASK) == REG_MOVE) {
			// MOVEs are always two in series (a from and to)
			// this one is the "to" MOVE (second) because of the reverse loop.
			// if there is a MOVE (to) --> then the first MOVE (from) can/should be skipped

			i--; // increase --> next interation is skipped --> "from" MOVE is skipped

			// its a "to"-MOVE --> can't be a UPDATE --> jump to next
			// MOVEs are no problem for the register id (rid) because MOVEs
			// are only allowed for "sorted/layered" entities (from map)
			continue;
		}

		// Now check the special case (if another one is removed with a low id)
		// --> in this case the rid from the child which should be removed
		// must be increased
		if ((mChanges[i] & REG_UPDATE_MASK) == REG_FLAG_REMOVE && // found a REMOVE from another child
				(mChanges[i] & REG_INDEX_MASK) <= (rid & REG_INDEX_MASK) // other child with lower id
				) {
			// <= and not < for rid check because before increase the other
			// can have the same rid.
			++rid;
			//LOGI("Found special case ;-)");
			continue;
		}
		
		if (mChanges[i] == rid /* --> UPDATE */) {
			mChanges.erase(mChanges.begin() + i); // erase one element
			// now mChanges[i] is the next higher element (previous accessed by loop)
			// --> no access should be taken which is currently the case
			count++;
		}
	}

	if (count > 0) {
		if (count != 1) {
			LOGSI << "child entities: (re)moveChild: warning: "
					"erase more than one update (" << count << ")" << endl;
		}
#ifdef CHILD_ENTITIES_DEBUG
		else {
			LOGI("child entities: (re)moveChild: info: erase 1 update\n");
		}
#endif
	}
	/* The code for removing UPDATES between ADD, MOVE, start and REMOVE
	 * is tricky with changing the register id (rid) for checking UPDATEs and
	 * against ADD, MOVE, start.
	 *
	 * A problem can exist if more removeChild() for different childs applied.
	 *
	 * If a child is removed with a lower id then all the childrens ids with higher
	 * ids from the vector (not layered) are decreased by 1. If one of this children also
	 * removed then this child has different ids for ADD and REMOVE entry.
	 * And also the UPDATES from this child can be diffent.
	 *
	 * Solution: Also check REMOVEs from other childs and correct the reg id
	 * temporary in the loop.
	 */
#endif

	mChanges.push_back(entity->mRegisterId | REG_FLAG_REMOVE);
	entity->mRegisterId = 0;
	entity->mParent = std::weak_ptr<ChildEntities>();
	setChanged();
	return true;
}

const std::vector<std::shared_ptr<gs::Entity> >& gs::ChildEntities::getEntities() const
{
	return mEntities;
}

void gs::ChildEntities::resetMutableChanged() const
{
	if (!isChanged()) {
		return;
	}
#if 1 // mChanges version - the smart version ;-)
	// this version use the mChanges vector to check only the childs which
	// has really a update.
	size_t cnt = mChanges.size();
	for (unsigned int i = 0; i < cnt; ++i) {
		switch (mChanges[i] & REG_UPDATE_MASK) {
			case 0: // UPDATE
			{
				std::shared_ptr<const Entity> entity =
					getChildPerRegisterId(mChanges[i] & ~REG_UPDATE_MASK);
				if (!entity) {
					LOGSE << "child entities: reset flags: entity not found" << endl;
					break;
				}
				if (!entity->isChanged()) {
					LOGSE << "child entities: reset flags: UPDATE flag but no changes" << endl;
					break;
				}
				entity->resetMutableChanged();
				break;
			}
			// for ADD and MOVE (and for REMOVE anyway) there is NO recursive
			// call necessary because if a entity has changes then an additional
			// UPDATE must exist!
			// If a entity is changed and added or added and changed there
			// must exist both.
			// Only a ADD without a UPDATE is also possible but in this case
			// the child is NOT changed and therefore
			// no entity->resetMutableChanged(); is necessary.
#if 0 // only for bug checking (normal disabled)
			case REG_FLAG_ADD:
			case REG_MOVE:
			case REG_FLAG_REMOVE:
			{
				uint32_t regId = mChanges[i] & ~REG_UPDATE_MASK;
				bool updateFound = false;
				for (size_t ii = 0; ii < cnt; ++ii) {
					if (mChanges[ii] & REG_UPDATE_MASK) {
						// no update
						continue;
					}
					if (regId == (mChanges[ii] & ~REG_UPDATE_MASK)) {
						updateFound = true;
						break;
					}
				}
				LOGI("No update found for %u (that's ok, only for info)",
						static_cast<unsigned int>((mChanges[i] & REG_UPDATE_MASK) >> 29));
			}
				break;
#endif
			default:
				break;
		}
	}
#else
	// the brute-force version - not so smart but very short code ;-)
	size_t cnt = mEntities.size();
	for (size_t i = 0; i < cnt; ++i) {
		// a extra check if entity is changed is not necessary because that's
		// the first thing which is done by resetMutableChanged().
		mEntities[i]->resetMutableChanged();
	}
	for (TSortedEntities::const_iterator it = mSortedEntities.begin();
			it != mSortedEntities.end(); ++it) {
		it->second->resetMutableChanged();
	}
#endif
	Component::resetMutableChanged();
	mCleared = false; // this is mCleared and not mChanged --> = false is necessary
	mChanges.clear();
}

unsigned int gs::ChildEntities::count() const
{
	unsigned int size = mEntities.size();
	unsigned int c = 0;
	for (unsigned int i = 0; i < size; i++) {
		c += mEntities[i]->count();
	}
	return c;
}

void gs::ChildEntities::updateChild(const Entity& entity)
{
#if 0
	if (entity.mParent.lock() != mThis.lock()) { // its not the correct parrent
		LOGSE << "child entities: update child failed. entity is not a child." << endl;
		return;
	}
#endif
	mChanges.push_back(entity.mRegisterId); // for update no additional flag is necessary
	setChanged();
}

bool gs::ChildEntities::removeChildFromEntities(const std::shared_ptr<Entity>& entity)
{
	// it'a a non sorted output entity
	uint32_t index = (entity->mRegisterId & REG_INDEX_MASK) - 1; // -1 because the index has a +1 offset
	if (index >= mEntities.size()) {
		LOGSE << "child entities: index of output entity is out of range" << endl;
		return false;
	}

	if (entity != mEntities[index]) {
		LOGSE << "child entities: wrong entity for register id "
				"(maybe you use the wrong parent for removing)" << endl;
		return false;
	}

	size_t cnt = mEntities.size();
	for (uint32_t i = index; i < cnt - 1; i++) {
		mEntities[i] = mEntities[i + 1];
		// no index mask is necessary for reducing because the lowest value of
		// register id can be HERE only 1 (normally 0 is also possible but not here)
		mEntities[i]->mRegisterId--;

		/* The register id is changed from a entity. (see above)
		 *
		 * But is it not a problem if the id already used by mChanges???
		 * No, because mChanges is used by the other ChildEntities which clones
		 * this one and the other ChildEntities has not removed the child now!
		 *
		 * If a entity with this changed id makes a update and is written to
		 * mChanges then it should also be no problem. Because at the time
		 * wenn the other ChildEntities (which makes the clone() call) came to
		 * this entry then at this point the child before is also removed and
		 * the id's now correct for this entry.
		 */
	}
	mEntities.pop_back();
	return true;
}

bool gs::ChildEntities::replaceChildAtEntities(const std::shared_ptr<Entity>& oldEntity,
		const std::shared_ptr<Entity>& newEntity)
{
	// it'a a non sorted output entity
	uint32_t index = (oldEntity->mRegisterId & REG_INDEX_MASK) - 1; // -1 because the reg index has a +1 offset
	if (index >= mEntities.size()) {
		LOGSE << "child entities: index of old output entity is out of range" << endl;
		return false;
	}

	if (oldEntity != mEntities[index]) {
		LOGSE << "child entities: wrong old entity for register id (maybe you use the wrong parent for removing)" << endl;
		return false;
	}

	mEntities[index] = newEntity;
	return true;
}

void gs::ChildEntities::registerChild(Entity& entity, uint32_t regId)
{
	entity.mRegisterId = regId;
	entity.mParent = mThis;
	if (mThis.expired()) {
		LOGW("Register child with expired weak pointer\n");
	}
	// the add flag is only allowed at mChanges vector and not at the entity
	mChanges.push_back(entity.mRegisterId | REG_FLAG_ADD);
}

std::shared_ptr<gs::Entity> gs::ChildEntities::getChildPerRegisterId(uint32_t regId) const
{
	regId &= ~REG_UPDATE_MASK; // remove the _ADD and/or _REMOVE flag

	uint32_t index = (regId & REG_INDEX_MASK) - 1; // -1 because the index has a +1 offset
	if (index >= mEntities.size()) {
		return shared_ptr<Entity>();
	}
	return mEntities[index];
}

std::string gs::ChildEntities::getRegIdAsString(uint32_t regId)
{
	std::stringstream ss;
	ss << "u "; // u for unsorted (vector), s for sorted (map) is was removed (doesn't exist)
	switch (regId & REG_UPDATE_MASK) {
		case 0:
			ss << "U";
			break;
		case REG_FLAG_ADD:
			ss << "A";
			break;
		case REG_MOVE:
			ss << "M";
			break;
		case REG_FLAG_REMOVE:
			ss << "R";
			break;
		default:
			ss << "?";
			break;
	}
	ss << " " << (regId & REG_INDEX_MASK);
	return ss.str();
}
