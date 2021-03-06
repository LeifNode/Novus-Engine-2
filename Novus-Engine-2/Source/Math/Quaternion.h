/*****************************************************************
 * Copyright (c) 2015 Leif Erkenbrach
 * Distributed under the terms of the MIT License.
 * (See accompanying file LICENSE or copy at
 * http://opensource.org/licenses/MIT)
 *****************************************************************/

#pragma once

namespace novus
{
	template <typename T> struct Vector2_t;
	template <typename T> struct Vector3_t;
	template <typename T> struct Vector4_t;
	template <typename T> struct Matrix3x3_t;
	template <typename T> struct Matrix4x4_t;

	template <typename T> struct Quaternion_t;

	typedef Quaternion_t<float>     Quaternionf;
	typedef Quaternion_t<double>    Quaterniond;
	typedef Quaternionf              Quaternion;

	template <typename T>
	struct Quaternion_t
	{
		T x, y, z, w;

		Quaternion_t();

		Quaternion_t(const T& a, const T& b, const T& c, const T& d);

		//Quaternion_t(const Matrix3x3_t<T>& rotationMat);

		template <typename B>
		Quaternion_t(const Quaternion_t<B>& q);

		template <typename B>
		Quaternion_t(const Vector4_t<B>& v);

		Quaternion_t<T>& operator= (const Quaternion_t<T>& q);

		Quaternion_t<T>& operator*= (const Quaternion_t<T>& q);

		//Add rotation around the axis v where the amount is the length of v
		Quaternion_t<T>& operator+= (const Vector3_t<T>& v);

		static T Magnitude(const Quaternion_t<T>& q);
		static Quaternion_t<T> Normalize(const Quaternion_t<T>& q);

		static T Dot(const Quaternion_t<T>& q1, const Quaternion_t<T>& q2);

		static Quaternion_t<T> AxisAngle(const Vector3_t<T>& axis, const T& radians);
		static Matrix4x4_t<T> ToMatrix(const Quaternion_t<T>& q);
		static Quaternion_t<T> FromMatrix(const Matrix3x3_t<T>& mat);
		static Quaternion_t<T> FromMatrix(const Matrix4x4_t<T>& mat);

		static Quaternion_t<T> Slerp(const Quaternion_t<T>& q1, const Quaternion_t<T>& q2, const T& t);
	};

	template <typename T>
	Quaternion_t<T> operator- (const Quaternion_t<T>& q);

	template <typename T>
	Quaternion_t<T> operator* (const Quaternion_t<T>& q1, const Quaternion_t<T>& q2);

	template <typename T>
	Quaternion_t<T> operator* (const Quaternion_t<T>& q, const T& s);

	template <typename T>
	Quaternion_t<T> operator* (const T& s, const Quaternion_t<T>& q);

	template <typename T>
	Quaternion_t<T> operator/ (const Quaternion_t<T>& q, const T& s);

	template <typename T>
	bool operator== (const Quaternion_t<T>& q1, const Quaternion_t<T>& q2);

	template <typename T>
	bool operator!= (const Quaternion_t<T>& q1, const Quaternion_t<T>& q2);

}

namespace novus
{
	template <typename T>
	Quaternion_t<T>::Quaternion_t()
		: x(0), y(0), z(0), w(1)
	{}

	template <typename T>
	Quaternion_t<T>::Quaternion_t(const T& a, const T& b, const T& c, const T& d)
		: x(a), y(b), z(c), w(d)
	{}

	template <typename T>
	template <typename B>
	Quaternion_t<T>::Quaternion_t(const Quaternion_t<B>& q)
		: x(static_cast<T>(q.x)), y(static_cast<T>(q.y)), z(static_cast<T>(q.z)), w(static_cast<T>(q.w))
	{}

	template <typename T>
	template <typename B>
	Quaternion_t<T>::Quaternion_t(const Vector4_t<B>& v)
		: x(static_cast<T>(v.x)), y(static_cast<T>(v.y)), z(static_cast<T>(v.z)), w(static_cast<T>(v.w))
	{}

	template <typename T>
	Quaternion_t<T>& Quaternion_t<T>::operator= (const Quaternion_t<T>& q)
	{
		this->x = q.x;
		this->y = q.y;
		this->z = q.z;
		this->w = q.w;
		return *this;
	}

	template <typename T>
	Quaternion_t<T>& Quaternion_t<T>::operator*= (const Quaternion_t<T>& q2)
	{
		const Quaternion_t<T>& q1 = *this;

		this->x = q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y;
		this->y = q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x;
		this->z = q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w;
		this->w = q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z;

		return *this;
	}

	template <typename T>
	Quaternion_t<T>& Quaternion_t<T>::operator+= (const Vector3_t<T>& v)
	{
		Quaternion_t<T> q(v.x, v.y, v.z, static_cast<T>(0));
		q *= *this;
		x += q.x * (static_cast<T>(0.5));
		y += q.y * (static_cast<T>(0.5));
		z += q.z * (static_cast<T>(0.5));
		w += q.w * (static_cast<T>(0.5));

		/*float length = Length(v);

		Quaternion_t<T> q = Quaternion_t<T>::AxisAngle(v / length, length);

		*this *= q;*/

		return *this;
	}

	template <typename T>
	Quaternion_t<T> operator- (const Quaternion_t<T>& q)
	{
		const T q2 = q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;

		return Quaternion_t<T>(-q.x / q2, -q.y / q2, -q.z / q2, q.w / q2);
	}

	template <typename T>
	Quaternion_t<T> operator* (const Quaternion_t<T>& q1, const Quaternion_t<T>& q2)
	{
		return Quaternion_t<T>(
			q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y,
			q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x,
			q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w,
			q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z);
	}

	template <typename T>
	Quaternion_t<T> operator* (const Quaternion_t<T>& q, const T& s)
	{
		return Quaternion_t<T>(q.x * s, q.y * s, q.z * s, q.w * s);
	}

	template <typename T>
	Quaternion_t<T> operator* (const T& s, const Quaternion_t<T>& q)
	{
		return Quaternion_t<T>(q.x * s, q.y * s, q.z * s, q.w * s);
	}

	template <typename T>
	Quaternion_t<T> operator/ (const Quaternion_t<T>& q, const T& s)
	{
		return Quaternion_t<T>(q.x / s, q.y / s, q.z / s, q.w / s);
	}

	template <typename T>
	T Quaternion_t<T>::Magnitude(const Quaternion_t<T>& q)
	{
		return sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
	}

	template <typename T>
	Quaternion_t<T> Quaternion_t<T>::Normalize(const Quaternion_t<T>& q)
	{
		return q / sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
	}

	template <typename T>
	T Quaternion_t<T>::Dot(const Quaternion_t<T>& q1, const Quaternion_t<T>& q2)
	{
		return q1.x * q2.x + q1.y *q2.y + q1.z * q2.z + q1.w * q2.w;
	}

	template <typename T>
	Quaternion_t<T> Quaternion_t<T>::AxisAngle(const Vector3_t<T>& axis, const T& radians)
	{
		T radOver2 = radians / 2;
		T radSin = sin(radOver2);

		return Quaternion_t<T>(
			axis.x * radSin,
			axis.y * radSin,
			axis.z * radSin,
			cos(radOver2));
	}

	template <typename T>
	Matrix4x4_t<T> Quaternion_t<T>::ToMatrix(const Quaternion_t<T>& q)
	{
		return Matrix4x4_t<T>(
			1 - 2 * q.y*q.y - 2 * q.z*q.z, 2 * q.x*q.y - 2 * q.w*q.z, 2 * q.x*q.z + 2 * q.w*q.y, 0,
			2 * q.x*q.y + 2 * q.w*q.z, 1 - 2 * q.x*q.x - 2 * q.z*q.z, 2 * q.y*q.z - 2 * q.w*q.x, 0,
			2 * q.x*q.z - 2 * q.w*q.y, 2 * q.y*q.z + 2 * q.w*q.x, 1 - 2 * q.x*q.x - 2 * q.y*q.y, 0,
			0, 0, 0, 1);
	}

	//http://www.gamasutra.com/view/feature/131686/rotating_objects_using_quaternions.php
	template <typename T>
	Quaternion_t<T> Quaternion_t<T>::FromMatrix(const Matrix3x3_t<T>& m)
	{
		Quaternion_t<T> result;

		T  tr, s, q[4];
		int    i, j, k;
		int nxt[3] = { 1, 2, 0 };
		tr = m[0][0] + m[1][1] + m[2][2];
		// check the diagonal
		if (tr > static_cast<T>(0.0)) {
			s = sqrt(tr + static_cast<T>(1.0));
			result.w = s / static_cast<T>(2.0);
			s = static_cast<T>(0.5) / s;
			result.x = (m[1][2] - m[2][1]) * s;
			result.y = (m[2][0] - m[0][2]) * s;
			result.z = (m[0][1] - m[1][0]) * s;
		}
		else {
			// diagonal is negative
			i = 0;
			if (m[1][1] > m[0][0]) i = 1;
			if (m[2][2] > m[i][i]) i = 2;
			j = nxt[i];
			k = nxt[j];
			s = sqrt((m[i][i] - (m[j][j] + m[k][k])) + static_cast<T>(1.0));
			q[i] = s * static_cast<T>(0.5);
			if (s != static_cast<T>(0.0)) s = static_cast<T>(0.5) / s;
			q[3] = (m[j][k] - m[k][j]) * s;
			q[j] = (m[i][j] + m[j][i]) * s;
			q[k] = (m[i][k] + m[k][i]) * s;
			result.x = q[0];
			result.y = q[1];
			result.z = q[2];
			result.w = q[3];
		}

		return result;
	}

	template <typename T>
	Quaternion_t<T> Quaternion_t<T>::FromMatrix(const Matrix4x4_t<T>& m)
	{
		Quaternion_t<T> result;

		T  tr, s, q[4];
		int    i, j, k;
		int nxt[3] = { 1, 2, 0 };
		tr = m[0][0] + m[1][1] + m[2][2];
		// check the diagonal
		if (tr > static_cast<T>(0.0)) {
			s = sqrt(tr + static_cast<T>(1.0));
			result.w = s / static_cast<T>(2.0);
			s = static_cast<T>(0.5) / s;
			result.x = (m[1][2] - m[2][1]) * s;
			result.y = (m[2][0] - m[0][2]) * s;
			result.z = (m[0][1] - m[1][0]) * s;
		}
		else {
			// diagonal is negative
			i = 0;
			if (m[1][1] > m[0][0]) i = 1;
			if (m[2][2] > m[i][i]) i = 2;
			j = nxt[i];
			k = nxt[j];
			s = sqrt((m[i][i] - (m[j][j] + m[k][k])) + static_cast<T>(1.0));
			q[i] = s * static_cast<T>(0.5);
			if (s != static_cast<T>(0.0)) s = static_cast<T>(0.5) / s;
			q[3] = (m[j][k] - m[k][j]) * s;
			q[j] = (m[i][j] + m[j][i]) * s;
			q[k] = (m[i][k] + m[k][i]) * s;
			result.x = q[0];
			result.y = q[1];
			result.z = q[2];
			result.w = q[3];
		}

		return result;
	}

	template <typename T>
	Quaternion_t<T> Quaternion_t<T>::Slerp(const Quaternion_t<T>& q1, const Quaternion_t<T>& q2, const T& t)
	{
		T cosAngle = Dot(q1, q2);
		T sinAngle = sqrt(1 - cosAngle * cosAngle);

		return (sinAngle * (1 - t) / sinAngle) * q1 + (sinAngle * t / sinAngle) * q2;
	}

	template <typename T>
	bool operator== (const Quaternion_t<T>& q1, const Quaternion_t<T>& q2)
	{
		return !(q1.x != q2.x || q1.y != q2.y || q1.z != q2.z || q1.w != q2.w);
	}

	template <typename T>
	bool operator!= (const Quaternion_t<T>& q1, const Quaternion_t<T>& q2)
	{
		return q1.x != q2.x || q1.y != q2.y || q1.z != q2.z || q1.w != q2.w;
	}
}