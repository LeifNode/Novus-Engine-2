/*****************************************************************
 * Copyright (c) 2015 Leif Erkenbrach
 * Distributed under the terms of the MIT License.
 * (See accompanying file LICENSE or copy at
 * http://opensource.org/licenses/MIT)
 *****************************************************************/

#pragma once

//Based off GLM's architecture

#include <cassert>

namespace novus
{
	template <typename T> struct Vector2_t;
	template <typename T> struct Vector3_t;
	template <typename T> struct Vector4_t;

	typedef Vector2_t<float>        Vector2f;
	typedef Vector2_t<double>       Vector2d;
	typedef Vector2_t<int>          Vector2i;
	typedef Vector2_t<unsigned int> Vector2u;
	typedef Vector2f                 Vector2;

	/**
	 *	Vector2 class declaration
	 */
	template <typename T>
	struct Vector2_t
	{
		//union { T x, r; };
		//union { T y, g; };

		union 
		{
			struct { T x, y; };
			struct { T r, g; };
		};

		//T x, y;

		Vector2_t();


		Vector2_t(const Vector2_t<T>& v);

		explicit Vector2_t(const T& s);

		Vector2_t(const T& a, const T& b);

		//Conversion constructors
		template <typename B>
		explicit Vector2_t(const Vector2_t<B>& v);

		template <typename B>
		explicit Vector2_t(const Vector3_t<B>& v);

		template <typename B>
		explicit Vector2_t(const Vector4_t<B>& v);

		template <typename A, typename B>
		Vector2_t(const A& a, const B& b);

		size_t size() const;

		T& operator[] (size_t i);
		const T& operator[] (size_t i) const;

		//Arithmetic operators
		template <typename U>
		Vector2_t<T>& operator= (const Vector2_t<U>& v);

		template <typename U>
		Vector2_t<T>& operator+= (const Vector2_t<U>& v);

		template <typename U>
		Vector2_t<T>& operator-= (const Vector2_t<U>& v);

		template <typename U>
		Vector2_t<T>& operator*= (const Vector2_t<U>& v);

		template <typename U>
		Vector2_t<T>& operator/= (const Vector2_t<U>& v);


		template <typename U>
		Vector2_t<T>& operator+= (const U& s);

		template <typename U>
		Vector2_t<T>& operator-= (const U& s);

		template <typename U>
		Vector2_t<T>& operator*= (const U& s);

		template <typename U>
		Vector2_t<T>& operator/= (const U& s);
	};

	template <typename T>
	Vector2_t<T> operator+ (const Vector2_t<T>& a, const Vector2_t<T>& b);
	template <typename T>
	Vector2_t<T> operator- (const Vector2_t<T>& a, const Vector2_t<T>& b);
	template <typename T>
	Vector2_t<T> operator* (const Vector2_t<T>& a, const Vector2_t<T>& b);
	template <typename T>
	Vector2_t<T> operator/ (const Vector2_t<T>& a, const Vector2_t<T>& b);

	template <typename T>
	Vector2_t<T> operator+ (const Vector2_t<T>& v, const T& s);
	template <typename T>
	Vector2_t<T> operator- (const Vector2_t<T>& v, const T& s);
	template <typename T>
	Vector2_t<T> operator* (const Vector2_t<T>& v, const T& s);
	template <typename T>
	Vector2_t<T> operator/ (const Vector2_t<T>& v, const T& s);

	template <typename T>
	Vector2_t<T> operator+ (const T& s, const Vector2_t<T>& v);
	template <typename T>
	Vector2_t<T> operator- (const T& s, const Vector2_t<T>& v);
	template <typename T>
	Vector2_t<T> operator* (const T& s, const Vector2_t<T>& v);
	template <typename T>
	Vector2_t<T> operator/ (const T& s, const Vector2_t<T>& v);

	template <typename T>
	Vector2_t<T> operator- (const Vector2_t<T>& v);

	template <typename T>
	bool operator== (const Vector2_t<T>& v1, const Vector2_t<T>& v2);
	template <typename T>
	bool operator!= (const Vector2_t<T>& v1, const Vector2_t<T>& v2);

	template <typename T>
	T Length(const Vector2_t<T>& v);

	template <typename T>
	T LengthSq(const Vector2_t<T>& v);

	template <typename T>
	Vector2_t<T> Normalize(const Vector2_t<T>& v);

	template <typename T>
	T Dot(const Vector2_t<T>& v1, const Vector2_t<T>& v2);

	template <typename T>
	Vector2_t<T> Project(const Vector2_t<T>& v, const Vector2_t<T>& n);

	template <typename T>
	Vector2_t<T> Reflect(const Vector2_t<T>& v, const Vector2_t<T>& n);
}


namespace novus
{
	/** Vector2 class definition */

	template <typename T>
	Vector2_t<T>::Vector2_t()
		: x(0), y(0)
	{}

	template <typename T>
	Vector2_t<T>::Vector2_t(const Vector2_t<T>& v)
		: x(v.x), y(v.y)
	{}

	template <typename T>
	Vector2_t<T>::Vector2_t(const T& s)
		: x(s), y(s)
	{}

	template <typename T>
	Vector2_t<T>::Vector2_t(const T& a, const T& b)
		: x(a), y(b)
	{}

	template <typename T>
	template <typename B>
	Vector2_t<T>::Vector2_t(const Vector2_t<B>& v)
		: x(static_cast<T>(v.x)), y(static_cast<T>(v.y))
	{}

	template <typename T>
	template <typename B>
	Vector2_t<T>::Vector2_t(const Vector3_t<B>& v)
		: x(static_cast<T>(v.x)), y(static_cast<T>(v.y))
	{}

	template <typename T>
	template <typename B>
	Vector2_t<T>::Vector2_t(const Vector4_t<B>& v)
		: x(static_cast<T>(v.x)), y(static_cast<T>(v.y))
	{}

	template <typename T>
	template <typename A, typename B>
	Vector2_t<T>::Vector2_t(const A& a, const B& b)
		: x(static_cast<T>(a)), y(static_cast<T>(b))
	{}

	//Arithmetic operators
	template <typename T>
	template <typename U>
	Vector2_t<T>& Vector2_t<T>::operator= (const Vector2_t<U>& v)
	{
		this->x = static_cast<T>(v.x);
		this->y = static_cast<T>(v.y);
		return *this;
	}

	template <typename T>
	template <typename U>
	Vector2_t<T>& Vector2_t<T>::operator+= (const Vector2_t<U>& v)
	{
		this->x += static_cast<T>(v.x);
		this->y += static_cast<T>(v.y);
		return *this;
	}

	template <typename T>
	template <typename U>
	Vector2_t<T>& Vector2_t<T>::operator-= (const Vector2_t<U>& v)
	{
		this->x -= static_cast<T>(v.x);
		this->y -= static_cast<T>(v.y);
		return *this;
	}

	template <typename T>
	template <typename U>
	Vector2_t<T>& Vector2_t<T>::operator*= (const Vector2_t<U>& v)
	{
		this->x *= static_cast<T>(v.x);
		this->y *= static_cast<T>(v.y);
		return *this;
	}

	template <typename T>
	template <typename U>
	Vector2_t<T>& Vector2_t<T>::operator/= (const Vector2_t<U>& v)
	{
		this->x /= static_cast<T>(v.x);
		this->y /= static_cast<T>(v.y);
		return *this;
	}

	template <typename T>
	template <typename U>
	Vector2_t<T>& Vector2_t<T>::operator+= (const U& s)
	{
		this->x += static_cast<T>(s);
		this->y += static_cast<T>(s);
		return *this;
	}

	template <typename T>
	template <typename U>
	Vector2_t<T>& Vector2_t<T>::operator-= (const U& s)
	{
		this->x -= static_cast<T>(s);
		this->y -= static_cast<T>(s);
		return *this;
	}

	template <typename T>
	template <typename U>
	Vector2_t<T>& Vector2_t<T>::operator*= (const U& s)
	{
		this->x *= static_cast<T>(s);
		this->y *= static_cast<T>(s);
		return *this;
	}

	template <typename T>
	template <typename U>
	Vector2_t<T>& Vector2_t<T>::operator/= (const U& s)
	{
		this->x /= static_cast<T>(s);
		this->y /= static_cast<T>(s);
		return *this;
	}

	template <typename T>
	size_t Vector2_t<T>::size() const
	{
		return 2;
	}

	template <typename T>
	T& Vector2_t<T>::operator[] (size_t i)
	{
		assert(i < size());

		return (&x)[i];
	}

	template <typename T>
	const T& Vector2_t<T>::operator[] (size_t i) const
	{
		assert(i < size());

		return (&x)[i];
	}

	template <typename T>
	Vector2_t<T> operator+ (const Vector2_t<T>& a, const Vector2_t<T>& b)
	{
		return Vector2_t<T>(a.x + b.x, a.y + b.y);
	}

	template <typename T>
	Vector2_t<T> operator- (const Vector2_t<T>& a, const Vector2_t<T>& b)
	{
		return Vector2_t<T>(a.x - b.x, a.y - b.y);
	}

	template <typename T>
	Vector2_t<T> operator* (const Vector2_t<T>& a, const Vector2_t<T>& b)
	{
		return Vector2_t<T>(a.x * b.x, a.y * b.y);
	}

	template <typename T>
	Vector2_t<T> operator/ (const Vector2_t<T>& a, const Vector2_t<T>& b)
	{
		return Vector2_t<T>(a.x / b.x, a.y / b.y);
	}

	template <typename T>
	Vector2_t<T> operator+ (const Vector2_t<T>& v, const T& s)
	{
		return Vector2_t<T>(v.x + s, v.y + s);
	}

	template <typename T>
	Vector2_t<T> operator- (const Vector2_t<T>& v, const T& s)
	{
		return Vector2_t<T>(v.x - s, v.y - s);
	}

	template <typename T>
	Vector2_t<T> operator* (const Vector2_t<T>& v, const T& s)
	{
		return Vector2_t<T>(v.x * s, v.y * s);
	}

	template <typename T>
	Vector2_t<T> operator/ (const Vector2_t<T>& v, const T& s)
	{
		return Vector2_t<T>(v.x / s, v.y / s);
	}


	template <typename T>
	Vector2_t<T> operator+ (const T& s, const Vector2_t<T>& v)
	{
		return Vector2_t<T>(s + v.x, s + v.y);
	}

	template <typename T>
	Vector2_t<T> operator- (const T& s, const Vector2_t<T>& v)
	{
		return Vector2_t<T>(s - v.x, s - v.y);
	}

	template <typename T>
	Vector2_t<T> operator* (const T& s, const Vector2_t<T>& v)
	{
		return Vector2_t<T>(s * v.x, s * v.y);
	}

	template <typename T>
	Vector2_t<T> operator/ (const T& s, const Vector2_t<T>& v)
	{
		return Vector2_t<T>(s / v.x, s / v.y);
	}

	template <typename T>
	Vector2_t<T> operator- (const Vector2_t<T>& v)
	{
		return Vector2_t<T>(-v.x, -v.y);
	}

	template <typename T>
	bool operator== (const Vector2_t<T>& v1, const Vector2_t<T>& v2)
	{
		return (v1.x == v2.x && v1.y == v2.y);
	}

	template <typename T>
	bool operator!= (const Vector2_t<T>& v1, const Vector2_t<T>& v2)
	{
		return (v1.x != v2.x || v1.y != v2.y);
	}

	template <typename T>
	T Length(const Vector2_t<T>& v)
	{
		return sqrt(v.x * v.x + v.y * v.y);
	}

	template <typename T>
	T LengthSq(const Vector2_t<T>& v)
	{
		return v.x * v.x + v.y * v.y;
	}

	template <typename T>
	Vector2_t<T> Normalize(const Vector2_t<T>& v)
	{
		float length = sqrt(v.x * v.x + v.y * v.y);

		return v / length;
	}

	template <typename T>
	T Dot(const Vector2_t<T>& v1, const Vector2_t<T>& v2)
	{
		return Vector2_t<T>(v1.x * v2.x + v1.y * v2.y);
	}

	template <typename T>
	Vector2_t<T> Project(const Vector2_t<T>& v, const Vector2_t<T>& n)
	{
		return Dot(v, n) * n;
	}

	template <typename T>
	Vector2_t<T> Reflect(const Vector2_t<T>& v, const Vector2_t<T>& n)
	{
		return v - 2.0f * Dot(v, n) * n;
	}
}