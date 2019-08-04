////////////////////////////////////////////////////////////
// from SFML vector
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
template <typename T>
inline Size2<T>::Size2() :
mWidth(0),
mHeight(0)
{

}


////////////////////////////////////////////////////////////
template <typename T>
inline Size2<T>::Size2(T width, T height) :
mWidth(width),
mHeight(height)
{

}


////////////////////////////////////////////////////////////
template <typename T>
template <typename U>
inline Size2<T>::Size2(const Size2<U>& vector) :
mWidth(static_cast<T>(vector.mWidth)),
mHeight(static_cast<T>(vector.mHeight))
{
}

template <typename T>
inline float Size2<T>::magnitude() const
{
	return sqrt(sgrMagnitude());
}

template <typename T>
inline float Size2<T>::sgrMagnitude() const
{
	return mWidth * mWidth + mHeight * mHeight;
}

////////////////////////////////////////////////////////////
template <typename T>
inline Size2<T> operator -(const Size2<T>& right)
{
    return Size2<T>(-right.mWidth, -right.mHeight);
}


////////////////////////////////////////////////////////////
template <typename T>
inline Size2<T>& operator +=(Size2<T>& left, const Size2<T>& right)
{
    left.mWidth += right.mWidth;
    left.mHeight += right.mHeight;

    return left;
}


////////////////////////////////////////////////////////////
template <typename T>
inline Size2<T>& operator -=(Size2<T>& left, const Size2<T>& right)
{
    left.mWidth -= right.mWidth;
    left.mHeight -= right.mHeight;

    return left;
}


////////////////////////////////////////////////////////////
template <typename T>
inline Size2<T> operator +(const Size2<T>& left, const Size2<T>& right)
{
    return Size2<T>(left.mWidth + right.mWidth, left.mHeight + right.mHeight);
}


////////////////////////////////////////////////////////////
template <typename T>
inline Size2<T> operator -(const Size2<T>& left, const Size2<T>& right)
{
    return Size2<T>(left.mWidth - right.mWidth, left.mHeight - right.mHeight);
}


////////////////////////////////////////////////////////////
template <typename T>
inline Size2<T> operator *(const Size2<T>& left, T right)
{
    return Size2<T>(left.mWidth * right, left.mHeight * right);
}


////////////////////////////////////////////////////////////
template <typename T>
inline Size2<T> operator *(T left, const Size2<T>& right)
{
    return Size2<T>(right.mWidth * left, right.mHeight * left);
}


////////////////////////////////////////////////////////////
template <typename T>
inline Size2<T>& operator *=(Size2<T>& left, T right)
{
    left.mWidth *= right;
    left.mHeight *= right;

    return left;
}


////////////////////////////////////////////////////////////
template <typename T>
inline Size2<T> operator /(const Size2<T>& left, T right)
{
    return Size2<T>(left.mWidth / right, left.mHeight / right);
}


////////////////////////////////////////////////////////////
template <typename T>
inline Size2<T>& operator /=(Size2<T>& left, T right)
{
    left.mWidth /= right;
    left.mHeight /= right;

    return left;
}


////////////////////////////////////////////////////////////
template <typename T>
inline bool operator ==(const Size2<T>& left, const Size2<T>& right)
{
    return (left.mWidth == right.mWidth) && (left.mHeight == right.mHeight);
}


////////////////////////////////////////////////////////////
template <typename T>
inline bool operator !=(const Size2<T>& left, const Size2<T>& right)
{
    return (left.mWidth != right.mWidth) || (left.mHeight != right.mHeight);
}
