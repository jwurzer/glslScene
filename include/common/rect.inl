////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2015 Laurent Gomila (laurent@sfml-dev.org)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
template <typename T>
Rect<T>::Rect() :
mLeft  (0),
mTop   (0),
mWidth (0),
mHeight(0)
{

}


////////////////////////////////////////////////////////////
template <typename T>
Rect<T>::Rect(T rectLeft, T rectTop, T rectWidth, T rectHeight) :
mLeft  (rectLeft),
mTop   (rectTop),
mWidth (rectWidth),
mHeight(rectHeight)
{

}


////////////////////////////////////////////////////////////
template <typename T>
Rect<T>::Rect(const Vector2<T>& position, const Vector2<T>& size) :
mLeft  (position.x),
mTop   (position.y),
mWidth (size.x),
mHeight(size.y)
{

}


////////////////////////////////////////////////////////////
template <typename T>
Rect<T>::Rect(const Vector2<T>& position, const Size2<T>& size) :
mLeft  (position.x),
mTop   (position.y),
mWidth (size.mWidth),
mHeight(size.mHeight)
{

}


////////////////////////////////////////////////////////////
template <typename T>
template <typename U>
Rect<T>::Rect(const Rect<U>& rectangle) :
mLeft  (static_cast<T>(rectangle.mLeft)),
mTop   (static_cast<T>(rectangle.mTop)),
mWidth (static_cast<T>(rectangle.mWidth)),
mHeight(static_cast<T>(rectangle.mHeight))
{
}


////////////////////////////////////////////////////////////
template <typename T>
bool Rect<T>::contains(T x, T y) const
{
    // Rectangles with negative dimensions are allowed, so we must handle them correctly

    // Compute the real min and max of the rectangle on both axes
    T minX = std::min(mLeft, static_cast<T>(mLeft + mWidth));
    T maxX = std::max(mLeft, static_cast<T>(mLeft + mWidth));
    T minY = std::min(mTop, static_cast<T>(mTop + mHeight));
    T maxY = std::max(mTop, static_cast<T>(mTop + mHeight));

    return (x >= minX) && (x < maxX) && (y >= minY) && (y < maxY);
}


////////////////////////////////////////////////////////////
template <typename T>
bool Rect<T>::contains(const Vector2<T>& point) const
{
    return contains(point.x, point.y);
}


////////////////////////////////////////////////////////////
template <typename T>
bool Rect<T>::intersects(const Rect<T>& rectangle) const
{
    Rect<T> intersection;
    return intersects(rectangle, intersection);
}


////////////////////////////////////////////////////////////
template <typename T>
bool Rect<T>::intersects(const Rect<T>& rectangle, Rect<T>& intersection) const
{
    // Rectangles with negative dimensions are allowed, so we must handle them correctly

    // Compute the min and max of the first rectangle on both axes
    T r1MinX = std::min(mLeft, static_cast<T>(mLeft + mWidth));
    T r1MaxX = std::max(mLeft, static_cast<T>(mLeft + mWidth));
    T r1MinY = std::min(mTop, static_cast<T>(mTop + mHeight));
    T r1MaxY = std::max(mTop, static_cast<T>(mTop + mHeight));

    // Compute the min and max of the second rectangle on both axes
    T r2MinX = std::min(rectangle.mLeft, static_cast<T>(rectangle.mLeft + rectangle.mWidth));
    T r2MaxX = std::max(rectangle.mLeft, static_cast<T>(rectangle.mLeft + rectangle.mWidth));
    T r2MinY = std::min(rectangle.mTop, static_cast<T>(rectangle.mTop + rectangle.mHeight));
    T r2MaxY = std::max(rectangle.mTop, static_cast<T>(rectangle.mTop + rectangle.mHeight));

    // Compute the intersection boundaries
    T interLeft   = std::max(r1MinX, r2MinX);
    T interTop    = std::max(r1MinY, r2MinY);
    T interRight  = std::min(r1MaxX, r2MaxX);
    T interBottom = std::min(r1MaxY, r2MaxY);

    // If the intersection is valid (positive non zero area), then there is an intersection
    if ((interLeft < interRight) && (interTop < interBottom))
    {
        intersection = Rect<T>(interLeft, interTop, interRight - interLeft, interBottom - interTop);
        return true;
    }
    else
    {
        intersection = Rect<T>(0, 0, 0, 0);
        return false;
    }
}

template <typename T>
Rect<T> Rect<T>::boundingBox(const Rect<T>& rectangle) const
{
	// Compute the min and max of the first rectangle on both axes
	T r1MinX = std::min(mLeft, static_cast<T>(mLeft + mWidth));
	T r1MaxX = std::max(mLeft, static_cast<T>(mLeft + mWidth));
	T r1MinY = std::min(mTop, static_cast<T>(mTop + mHeight));
	T r1MaxY = std::max(mTop, static_cast<T>(mTop + mHeight));

	// Compute the min and max of the second rectangle on both axes
	T r2MinX = std::min(rectangle.mLeft, static_cast<T>(rectangle.mLeft + rectangle.mWidth));
	T r2MaxX = std::max(rectangle.mLeft, static_cast<T>(rectangle.mLeft + rectangle.mWidth));
	T r2MinY = std::min(rectangle.mTop, static_cast<T>(rectangle.mTop + rectangle.mHeight));
	T r2MaxY = std::max(rectangle.mTop, static_cast<T>(rectangle.mTop + rectangle.mHeight));

	// Compute the bounding box
	T boundLeft   = std::min(r1MinX, r2MinX);
	T boundTop    = std::min(r1MinY, r2MinY);
	T boundRight  = std::max(r1MaxX, r2MaxX);
	T boundBottom = std::max(r1MaxY, r2MaxY);

	return Rect<T>(boundLeft, boundTop, boundRight - boundLeft, boundBottom - boundTop);
}

template <typename T>
Vector2<T> Rect<T>::position() const
{
	return Vector2<T>(mLeft, mTop);
}
template <typename T>
void Rect<T>::setPosition(const Vector2<T>& pos)
{
	mLeft = pos.x;
	mTop = pos.y;
}

template <typename T>
Size2<T> Rect<T>::size() const
{
	return Size2<T>(mWidth, mHeight);
}

template <typename T>
void Rect<T>::setSize(const Size2<T>& size)
{
	mWidth = size.mWidth;
	mHeight = size.mHeight;
}

template <typename T>
bool Rect<T>::isEqual(const Rect<T>& r, T tolerance) const
{
	// mLeft + tolerance < r.mLeft -- is the same as -- mLeft < r.mLeft - tolerance
	// but this version has no problem with unsigned T
	if (mLeft + tolerance < r.mLeft || mLeft > r.mLeft + tolerance) {
		return false;
	}
	if (mTop + tolerance < r.mTop || mTop > r.mTop + tolerance) {
		return false;
	}
	if (mWidth + tolerance < r.mWidth || mWidth > r.mWidth + tolerance) {
		return false;
	}
	if (mHeight + tolerance < r.mHeight || mHeight > r.mHeight + tolerance) {
		return false;
	}
	return true;
}

template <typename T>
bool Rect<T>::isEqualPosition(const Vector2<T>& position, T tolerance) const
{
	// mLeft + tolerance < position.mLeft -- is the same as -- mLeft < position.mLeft - tolerance
	// but this version has no problem with unsigned T
	if (mLeft + tolerance < position.mLeft || mLeft > position.mLeft + tolerance) {
		return false;
	}
	if (mTop + tolerance < position.mTop || mTop > position.mTop + tolerance) {
		return false;
	}
	return true;
}

template <typename T>
bool Rect<T>::isEqualSize(const Size2<T>& size, T tolerance) const
{
	if (mWidth + tolerance < size.mWidth || mWidth > size.mWidth + tolerance) {
		return false;
	}
	if (mHeight + tolerance < size.mHeight || mHeight > size.mHeight + tolerance) {
		return false;
	}
	return true;
}

template <typename T>
void Rect<T>::regular()
{
	if (mWidth < 0) {
		mLeft += mWidth;
		mWidth = -mWidth;
	}
	if (mHeight < 0) {
		mTop += mHeight;
		mHeight = -mHeight;
	}
}

////////////////////////////////////////////////////////////
template <typename T>
inline bool operator ==(const Rect<T>& left, const Rect<T>& right)
{
    return (left.mLeft == right.mLeft) && (left.mWidth == right.mWidth) &&
           (left.mTop == right.mTop) && (left.mHeight == right.mHeight);
}


////////////////////////////////////////////////////////////
template <typename T>
inline bool operator !=(const Rect<T>& left, const Rect<T>& right)
{
    return !(left == right);
}
