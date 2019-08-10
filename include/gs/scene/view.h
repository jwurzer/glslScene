#ifndef GLSLSCENE_VIEW_H
#define GLSLSCENE_VIEW_H

namespace gs
{
	enum class ViewMode
	{
		WINDOW_SIZE,
	};

	class View
	{
	public:
		View(ViewMode mode);
		ViewMode getViewMode() const { return mViewMode; }
	private:
		ViewMode mViewMode;
	};
}

#endif //GLSLSCENE_VIEW_H
