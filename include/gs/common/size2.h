////////////////////////////////////////////////////////////
// from SFML vector2
////////////////////////////////////////////////////////////

#ifndef GLSLSCENE_SIZE2_H
#define GLSLSCENE_SIZE2_H

#include <stdint.h>
#include <math.h> // for magnitude

// Define the most common types
#include <gs/common/size2_fwd.h>

namespace gs
{
	////////////////////////////////////////////////////////////
	/// \brief Utility template class for manipulating
	///        2-dimensional vectors
	///
	////////////////////////////////////////////////////////////
	template <typename T>
	class Size2
	{
	public :

		////////////////////////////////////////////////////////////
		/// \brief Default constructor
		///
		/// Creates a Size2(0, 0).
		///
		////////////////////////////////////////////////////////////
		Size2();

		////////////////////////////////////////////////////////////
		/// \brief Construct the vector from its coordinates
		///
		/// \param width width length
		/// \param height height length
		///
		////////////////////////////////////////////////////////////
		Size2(T width, T height);

		////////////////////////////////////////////////////////////
		/// \brief Construct the vector from another type of vector
		///
		/// This constructor doesn't replace the copy constructor,
		/// it's called only when U != T.
		/// A call to this constructor will fail to compile if U
		/// is not convertible to T.
		///
		/// \param vector Vector to convert
		///
		////////////////////////////////////////////////////////////
		template <typename U>
		explicit Size2(const Size2<U>& vector);

		////////////////////////////////////////////////////////////
		// Member data
		////////////////////////////////////////////////////////////
		T mWidth; ///< Width length of the Size2
		T mHeight; ///< Height length of the Size2

		float magnitude() const;
		// Calculating the squared magnitude instead of using the magnitude
		// property is much faster - the calculation is basically the same
		// only without the slow Sqrt call.
		float sgrMagnitude() const;
	};

	////////////////////////////////////////////////////////////
	/// \relates Size2
	/// \brief Overload of unary operator -
	///
	/// \param right Vector to negate
	///
	/// \return Memberwise opposite of the vector
	///
	////////////////////////////////////////////////////////////
	template <typename T>
	Size2<T> operator -(const Size2<T>& right);

	////////////////////////////////////////////////////////////
	/// \relates Size2
	/// \brief Overload of binary operator +=
	///
	/// This operator performs a memberwise addition of both vectors,
	/// and assigns the result to \a left.
	///
	/// \param left  Left operand (a vector)
	/// \param right Right operand (a vector)
	///
	/// \return Reference to \a left
	///
	////////////////////////////////////////////////////////////
	template <typename T>
	Size2<T>& operator +=(Size2<T>& left, const Size2<T>& right);

	////////////////////////////////////////////////////////////
	/// \relates Size2
	/// \brief Overload of binary operator -=
	///
	/// This operator performs a memberwise subtraction of both vectors,
	/// and assigns the result to \a left.
	///
	/// \param left  Left operand (a vector)
	/// \param right Right operand (a vector)
	///
	/// \return Reference to \a left
	///
	////////////////////////////////////////////////////////////
	template <typename T>
	Size2<T>& operator -=(Size2<T>& left, const Size2<T>& right);

	////////////////////////////////////////////////////////////
	/// \relates Size2
	/// \brief Overload of binary operator +
	///
	/// \param left  Left operand (a vector)
	/// \param right Right operand (a vector)
	///
	/// \return Memberwise addition of both vectors
	///
	////////////////////////////////////////////////////////////
	template <typename T>
	Size2<T> operator +(const Size2<T>& left, const Size2<T>& right);

	////////////////////////////////////////////////////////////
	/// \relates Size2
	/// \brief Overload of binary operator -
	///
	/// \param left  Left operand (a vector)
	/// \param right Right operand (a vector)
	///
	/// \return Memberwise subtraction of both vectors
	///
	////////////////////////////////////////////////////////////
	template <typename T>
	Size2<T> operator -(const Size2<T>& left, const Size2<T>& right);

	////////////////////////////////////////////////////////////
	/// \relates Size2
	/// \brief Overload of binary operator *
	///
	/// \param left  Left operand (a vector)
	/// \param right Right operand (a scalar value)
	///
	/// \return Memberwise multiplication by \a right
	///
	////////////////////////////////////////////////////////////
	template <typename T>
	Size2<T> operator *(const Size2<T>& left, T right);

	////////////////////////////////////////////////////////////
	/// \relates Size2
	/// \brief Overload of binary operator *
	///
	/// \param left  Left operand (a scalar value)
	/// \param right Right operand (a vector)
	///
	/// \return Memberwise multiplication by \a left
	///
	////////////////////////////////////////////////////////////
	template <typename T>
	Size2<T> operator *(T left, const Size2<T>& right);

	////////////////////////////////////////////////////////////
	/// \relates Size2
	/// \brief Overload of binary operator *=
	///
	/// This operator performs a memberwise multiplication by \a right,
	/// and assigns the result to \a left.
	///
	/// \param left  Left operand (a vector)
	/// \param right Right operand (a scalar value)
	///
	/// \return Reference to \a left
	///
	////////////////////////////////////////////////////////////
	template <typename T>
	Size2<T>& operator *=(Size2<T>& left, T right);

	////////////////////////////////////////////////////////////
	/// \relates Size2
	/// \brief Overload of binary operator /
	///
	/// \param left  Left operand (a vector)
	/// \param right Right operand (a scalar value)
	///
	/// \return Memberwise division by \a right
	///
	////////////////////////////////////////////////////////////
	template <typename T>
	Size2<T> operator /(const Size2<T>& left, T right);

	////////////////////////////////////////////////////////////
	/// \relates Size2
	/// \brief Overload of binary operator /=
	///
	/// This operator performs a memberwise division by \a right,
	/// and assigns the result to \a left.
	///
	/// \param left  Left operand (a vector)
	/// \param right Right operand (a scalar value)
	///
	/// \return Reference to \a left
	///
	////////////////////////////////////////////////////////////
	template <typename T>
	Size2<T>& operator /=(Size2<T>& left, T right);

	////////////////////////////////////////////////////////////
	/// \relates Size2
	/// \brief Overload of binary operator ==
	///
	/// This operator compares strict equality between two vectors.
	///
	/// \param left  Left operand (a vector)
	/// \param right Right operand (a vector)
	///
	/// \return True if \a left is equal to \a right
	///
	////////////////////////////////////////////////////////////
	template <typename T>
	bool operator ==(const Size2<T>& left, const Size2<T>& right);

	////////////////////////////////////////////////////////////
	/// \relates Size2
	/// \brief Overload of binary operator !=
	///
	/// This operator compares strict difference between two vectors.
	///
	/// \param left  Left operand (a vector)
	/// \param right Right operand (a vector)
	///
	/// \return True if \a left is not equal to \a right
	///
	////////////////////////////////////////////////////////////
	template <typename T>
	bool operator !=(const Size2<T>& left, const Size2<T>& right);

	#include <gs/common/size2.inl>
} // namespace gs

#endif

////////////////////////////////////////////////////////////
/// \class gs::Size2
/// \ingroup system
///
/// gs::Size2 is a simple class that defines a mathematical
/// vector with two coordinates (x and y). It can be used to
/// represent anything that has two dimensions: a size, a point,
/// a velocity, etc.
///
/// The template parameter T is the type of the coordinates. It
/// can be any type that supports arithmetic operations (+, -, /, *)
/// and comparisons (==, !=), for example int or float.
///
/// You generally don't have to care about the templated form (gs::Size2<T>),
/// the most common specializations have special typedefs:
/// \li gs::Size2<float> is gs::Size2f
/// \li gs::Size2<int> is gs::Size2i
/// \li gs::Size2<unsigned int> is gs::Size2u
///
/// The gs::Size2 class has a small and simple interface, its x and y members
/// can be accessed directly (there's no accessor like setX(), getX()) and it
/// contains no mathematical function like dot product, cross product, length, etc.
///
/// Usage example:
/// \code
/// gs::Size2f v1(16.5f, 24.f);
/// v1.x = 18.2f;
/// float y = v1.y;
///
/// gs::Size2f v2 = v1 * 5.f;
/// gs::Size2f v3;
/// v3 = v1 + v2;
///
/// bool different = (v2 != v3);
/// \endcode
///
/// Note: for 3-dimensional vectors, see gs::Vector3.
///
////////////////////////////////////////////////////////////
