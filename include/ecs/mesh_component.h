#ifndef GLSLSCENE_GRAPHIC2D_COMPONENT_H
#define GLSLSCENE_GRAPHIC2D_COMPONENT_H

#include <ecs/component.h>
#include <res/resource_id.h>
#include <common/color.h>
#include <stdint.h>
#include <memory>

#include <common/rect_fwd.h>

namespace gs
{
	class Value;
	class ValueParam;
	class VectorGraphics;

	class MeshComponent: public Component
	{
	public:
		/**
		 * This pointer can be used for anything.
		 * The pointer will be NOT copied at the clone function.
		 * The clone() function leave this pointer at the source unchanged and
		 * set this pointer to NULL at the destination!!!!!
		 *
		 * This pointer is used by the rendering engine for quick access of resources.
		 */
		mutable std::shared_ptr<void> mSharedCustomData;

		/**
		 * This pointer can be used for anything.
		 * The pointer will be NOT copied at the clone function.
		 * The clone() function leave this pointer at the source AND
		 * destination unchanged!
		 */
		mutable std::shared_ptr<void> mSharedPermanentData;

		MeshComponent(std::weak_ptr<Entity> entity);
		virtual ~MeshComponent();

		virtual EComponent getType() const { return COMPONENT_MESH; }

		/**
		 * Reset all to default beside of mCustomData. mCustomData is unchanged!
		 */
		void reset();

		/**
		 * can be a graphic id or texture/bitmap id
		 * default is 0 (no graphic).
		 * This member function also set the page index to zero.
		 */
		void setGraphicId(TMeshId graphicId);
		/**
		 * Can be used instead of setGraphicId.
		 * This is useful if a graphic is changed by the game
		 *
		 * If setGraphic set a new graphic (pointer changed) then
		 * setGraphicChanged() is automatically called.
		 */
		void setGraphic(const std::shared_ptr<VectorGraphics>& graphic);
		/**
		 * If no graphic is set by setGraphic(VectorGraphics graphic) then this call is ignored.
		 * setGraphicChanged() should be called if the game has changed the
		 * elements of the graphic. Otherwise the changes are not recognized.
		 */
		void setGraphicChanged();

		/**
		 * Default value for smooth is false.
		 * This is currently only used for VG_AS_BITMAP (also not for VG_AS_TEX_BITMAP).
		 * This means, only a graphic which is loaded with loadGraphic() or
		 * loadSvgGraphic() and parameter vgAsBitmap is true then using this
		 * member function has an effect.
		 * @param smooth
		 */
		void setSmooth(bool smooth);

		/*
		 * default is Const(0.0f) and 0 --> no animation
		 * Internal mAnimationTs is NULL and mAnimationRefTs is 0.
		 */
		void setAnimationTs(const Value& ts, int64_t referenceTs);
		/*
		 * default is 0
		 *
		 * Using setFreezeTs always deactivate the animation.
		 */
		void setFreezeTs(int64_t freezeTs);

		/**
		 * Coloring sprites, bitmaps and also set the start-value (default) for
		 * currentColor at vector graphics. Alpha value is NOT ignored for
		 * currentColor. This means if setColor() is used and a vector graphic
		 * use this currentColor as stroke or fill then also stroke-opacity or
		 * fill-opacity are overwritten with the alpha value.
         */
		void setColor(float r, float g, float b); // set alpha to 1.0f
		void setColor(float r, float g, float b, float a);
		void setColor(const Color& color);

		TMeshId getGraphicId() const;
		const std::shared_ptr<VectorGraphics>& getGraphic() const { return mGraphic; }
		bool isGraphicChanged() const { return mGraphicChanged; }
		void resetMutableGraphicChanged() const { mGraphicChanged = false; }
		bool isSmooth() const { return mIsSmooth; }
		int64_t getAnimationTs(int64_t ts, const RectFloat& scrSize) const;
		const float* getColor(int64_t ts, const RectFloat& scrSize) const;

		// also reset mGraphicChanged
		virtual void resetMutableChanged() const;
	private:

		TMeshId mGraphicId;
		std::shared_ptr<VectorGraphics> mGraphic;
		mutable bool mGraphicChanged;

		bool mIsSmooth;
		std::shared_ptr<const Value> mAnimationTs;
		int64_t mAnimationRefTs;
		Color mColor;
	};
}
#endif
