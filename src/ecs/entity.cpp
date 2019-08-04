
#include <ecs/entity.h>

#include <ecs/logic_component.h>
#include <ecs/shader_component.h>
#include <ecs/texture_component.h>
#include <ecs/mesh_component.h>
#include <ecs/transform_component.h>
#include <ecs/child_entities.h>

#include <system/log.h>
#include <string.h>

std::shared_ptr<gs::Entity> gs::Entity::create()
{
#if 1
	std::shared_ptr<Entity> oe(new Entity());
#else
	// see https://stackoverflow.com/questions/8147027/how-do-i-call-stdmake-shared-on-a-class-with-only-protected-or-private-const
	// see https://groups.google.com/a/isocpp.org/forum/#!topic/std-proposals/YXyGFUq7Wa4
	//std::shared_ptr<Entity> oe = std::make_shared<Entity>();
	// TODO: struct A should maybe moved to header file.
	struct A : std::allocator<Entity> {
		void construct(void* p) { ::new(p) Entity(); }
		void destroy(Entity* p) { p->Entity	};
	std::shared_ptr<Entity> oe = std::allocate_shared<Entity>(A());
#endif
	oe->mThis = oe;
	return oe;
}

gs::Entity::Entity()
		:mThis(), mRegisterId(0), mParent(), mCloneSource(), mChanged(false),
		mIsActive(true), mOffsetTd(0),
		mLogic(), mShader(), mTexture(), mMesh(), mTransform(), mChildren()
{
	memset(mName, 0, MAX_NAME_LENGTH);
}

gs::Entity::~Entity()
{
	//removeAll();
}

void gs::Entity::setName(const char* name)
{
	copyBufAsName(name, strlen(name));
}

void gs::Entity::copyBufAsName(const void* src, size_t length)
{
	char name[MAX_NAME_LENGTH];
	memset(name, 0, MAX_NAME_LENGTH);
	// limit length to MAX_NAME_LENGTH - 1 --> last element is always 0
	length = (length >= MAX_NAME_LENGTH) ? (MAX_NAME_LENGTH - 1) : length;
	memcpy(name, src, length);

	if (!memcmp(mName, name, MAX_NAME_LENGTH)) {
		return; // nothing to do
	}
	memcpy(mName, name, MAX_NAME_LENGTH); // length instead of MAX_NAME_LENGTH would be also ok
	setChanged();
}

void gs::Entity::setActive(bool active)
{
	if (mIsActive == active) {
		return; // nothing to do
	}
	mIsActive = active;
	setChanged();
}

void gs::Entity::setTimeOffset(int64_t offsetTd)
{
	if (mOffsetTd == offsetTd) {
		return; // nothing to do
	}
	mOffsetTd = offsetTd;
	setChanged();
}

gs::LogicComponent& gs::Entity::logic()
{
	if (!mLogic) {
		mLogic.reset(new LogicComponent(mThis));
		setChanged();
	}
	return *mLogic;
}

const gs::LogicComponent* gs::Entity::getConstLogic() const
{
	return mLogic.get();
}

gs::ShaderComponent& gs::Entity::shader()
{
	if (!mShader) {
		mShader.reset(new ShaderComponent(mThis));
		setChanged();
	}
	return *mShader;
}

const gs::ShaderComponent* gs::Entity::getConstShader() const
{
	return mShader.get();
}

gs::TextureComponent& gs::Entity::texture()
{
	if (!mTexture) {
		mTexture.reset(new TextureComponent(mThis));
		setChanged();
	}
	return *mTexture;
}

const gs::TextureComponent* gs::Entity::getConstTexture() const
{
	return mTexture.get();
}

gs::MeshComponent& gs::Entity::mesh()
{
	if (!mMesh) {
		mMesh.reset(new MeshComponent(mThis));
		setChanged();
	}
	return *mMesh;
}

const gs::MeshComponent* gs::Entity::getConstMesh() const
{
	return mMesh.get();
}

gs::TransformComponent& gs::Entity::transform2d()
{
	if (!mTransform) {
		mTransform.reset(new TransformComponent(mThis));
		setChanged();
	}
	return *mTransform;
}

const gs::TransformComponent* gs::Entity::getConstTransform() const
{
	return mTransform.get();
}

gs::ChildEntities& gs::Entity::childEntities()
{
	if (!mChildren) {
		mChildren = ChildEntities::create(mThis);
		setChanged();
	}
	return *mChildren;
}

const gs::ChildEntities* gs::Entity::getConstChildEntities() const
{
	return mChildren.get();
}

void gs::Entity::removeLogic()
{
	if (!mLogic) {
		return; // nothing to do
	}
	mLogic.reset();
	setChanged();
}

void gs::Entity::removeShader()
{
	if (!mShader) {
		return; // nothing to do
	}
	mShader.reset();
	setChanged();
}

void gs::Entity::removeTexture()
{
	if (!mTexture) {
		return; // nothing to do
	}
	mTexture.reset();
	setChanged();
}

void gs::Entity::removeMesh()
{
	if (!mMesh) {
		return; // nothing to do
	}
	mMesh.reset();
	setChanged();
}

void gs::Entity::removeTransform2d()
{
	if (!mTransform) {
		return; // nothing to do
	}
	mTransform.reset();
	setChanged();
}

void gs::Entity::removeChildEntities()
{
	if (!mChildren) {
		return; // nothing to do
	}
	mChildren.reset();
	setChanged();
}

void gs::Entity::removeAllComponents()
{
	removeLogic();
	removeShader();
	removeTexture();
	removeMesh();
	removeTransform2d();
	removeChildEntities();
}

bool gs::Entity::removeMeFromParent()
{
	if (!mRegisterId) {
		// output entity is not a child --> no parent --> no removing
		return false;
	}
	std::shared_ptr<ChildEntities> parent(mParent.lock());
	std::shared_ptr<Entity> thisPtr(mThis.lock());
	if (!parent || !thisPtr) {
		return false; // no parent or no shared this pointer
	}
	return parent->removeChild(thisPtr);
}

void gs::Entity::resetMutableChanged() const
{
	if (!mChanged) {
		return;
	}
	mChanged = false;
	if (mMesh) {
		mMesh->resetMutableChanged();
	}
	if (mTransform) {
		mTransform->resetMutableChanged();
	}
	if (mChildren) {
		// reset all childs, sub childs, sub sub childs, ...
		mChildren->resetMutableChanged();
	}
}

bool gs::Entity::getTransform2d(int64_t ts, const RectFloat& scrSize, glm::mat4& out) const
{
	bool rv = false;
	if (mTransform) {
		mTransform->getTransform2d(out);
		rv = true;
	}
	return rv;
}

unsigned int gs::Entity::getTransformInclParrents(int64_t ts,
		const RectFloat& scrSize, glm::mat4& out) const
{
	std::shared_ptr<ChildEntities> parent;
	std::shared_ptr<Entity> entity;
	if (!(parent = mParent.lock()) || !(entity = parent->mEntity.lock())) {
		// no parent --> the root output entity --> reset transform
		out = glm::mat4(1.0f);
		getTransform2d(ts, scrSize, out);
		return 1;
	}
	unsigned int count =
			entity->getTransformInclParrents(ts, scrSize, out);
	getTransform2d(ts, scrSize, out);
	return count + 1;
}

unsigned int gs::Entity::count() const
{
	if (!mChildren) {
		return 1;
	}
	return 1 + mChildren->count();
}

void gs::Entity::setChanged()
{
	if (mChanged) {
		// if output entity is used before it is added then
		// mChanged will be set but no called. thats ok because if its added
		// then addChild adds the register id to the changes vector
		return; // nothing to do
	}
	mChanged = true;
	std::shared_ptr<ChildEntities> parent = mParent.lock();
	if (!parent) {
		return;
	}
	parent->updateChild(*this);
	//LOGSI << "update child " << mChanged << " " << this << std::endl;
}
