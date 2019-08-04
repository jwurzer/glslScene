
#include <ecs/mesh_component.h>
//#include <common/value/value.h>
//#include <common/value/value.h>
//#include <common/value/const.h>
//#include <common/color_ex.h>
//#include <out/graphic/vg_element.h>
//#include <out/ecs/update_info.h>
#include <system/log.h>

gs::MeshComponent::MeshComponent(std::weak_ptr<Entity> entity)
		:Component(entity), mSharedCustomData(),
		mSharedPermanentData(), mGraphicId(0), mGraphic(),
		mGraphicChanged(false), mIsSmooth(false),
		mAnimationTs(), mAnimationRefTs(0), mColor()
{
	reset();
}

gs::MeshComponent::~MeshComponent()
{
}

void gs::MeshComponent::reset()
{
	setChanged();
	mGraphicId = 0;
	mGraphic.reset();
	mGraphicChanged = false;
	mIsSmooth = false;
	setFreezeTs(0);
	mColor = Color::white();
}

void gs::MeshComponent::setGraphicId(TMeshId graphicId)
{
	setChanged();
	mGraphicId = graphicId;
	mGraphic.reset();
	mGraphicChanged = false;
}

void gs::MeshComponent::setGraphic(const std::shared_ptr<VectorGraphics>& graphic)
{
	if (mGraphic == graphic && !mGraphicId) {
		return; // nothing to do --> no "changed" update (setGraphicChanged() should be used)
	}
	mGraphicId = 0;
	mGraphic = graphic;
	setGraphicChanged();
}

void gs::MeshComponent::setGraphicChanged()
{
	if (!mGraphic) {
		return;
	}
	mGraphicChanged = true;
	setChanged();
}

void gs::MeshComponent::setSmooth(bool smooth)
{
	if (mIsSmooth == smooth) {
		return; // nothing to do
	}
	setChanged();
	mIsSmooth = smooth;
}
void gs::MeshComponent::setAnimationTs(const Value& ts, int64_t referenceTs)
{
	setChanged();
	//mAnimationTs = ts.getCopy();
	mAnimationRefTs = referenceTs;
}

void gs::MeshComponent::setFreezeTs(int64_t freezeTs)
{
	setChanged();
	mAnimationTs = std::shared_ptr<const Value>();
	mAnimationRefTs = freezeTs;
}

void gs::MeshComponent::setColor(float r, float g, float b)
{
	setColor(r, g, b, 1.0f);
}

void gs::MeshComponent::setColor(float r, float g, float b, float a)
{
	setChanged();
	mColor.set(r, g, b, a);
}

gs::TMeshId gs::MeshComponent::getGraphicId() const
{
	return mGraphicId;
}

int64_t gs::MeshComponent::getAnimationTs(int64_t ts,
		const RectFloat& scrSize) const
{
	if (!mAnimationTs) {
		// a freeze value is set (no animation but a freeze-frame time)
		return mAnimationRefTs;
	}
	//const ValueParam aniVp(float(ts - mAnimationRefTs) / 1000000.0f, scrSize);
	//return mAnimationTs->getResult(aniVp) * 1000000.0f;
	return 0;
}

const float* gs::MeshComponent::getColor(int64_t ts,
		const RectFloat& scrSize) const
{
	return mColor.rgba;
}

void gs::MeshComponent::resetMutableChanged() const
{
	Component::resetMutableChanged();
	// BUG???
	//mGraphicChanged = false; // set to false makes here a problem!

	/*
	 * The problem is that the sfml rendering engine not (or only sometimes)
	 * recognized a changed/updated vertex array (and maybe other updated graphics).
	 *
	 * e.g. Problem can be seen by background rects at line menu (bg for each)
	 * at playano score menu (score trainer)
	 *
	 * Maybe the "try lock" for loading at rendering engine can be the reason?
	 * If lock is not possible the mGraphicChanged is reset in any case ...
	 */
}
