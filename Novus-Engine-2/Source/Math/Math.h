#pragma once

#include <cmath>
#include <random>

namespace novus
{
class Math
{
public:
	//0..1
	static float RandF()
	{
		return (float)(rand()) / (float)RAND_MAX;
	}

	//a..b
	static float RandF(float a, float b)
	{
		return a + RandF()*(b - a);
	}

	template<typename T>
	static T Min(const T& a, const T& b)
	{
		return a < b ? a : b;
	}

	template<typename T>
	static T Max(const T& a, const T& b)
	{
		return a > b ? a : b;
	}

	template<typename T>
	static T Lerp(const T& a, const T& b, float t)
	{
		return a + (b - a)*t;
	}

	template<typename T>
	static T Clamp(const T& x, const T& low, const T& high)
	{
		return x < low ? low : (x > high ? high : x);
	}

	static int MipMapCount(int width);
	static int MipMapCount(int width, int height);
	static int MipMapCount(int width, int height, int depth);


	static const float Pi;
	static const float TwoPi;
	static const float PiOver2;
	static const float PiOver4;
	static const float Infinity;

	static float WrapAngle(float angle)
	{
		if (angle < 0.0f || angle > Math::TwoPi)
			return angle - Math::TwoPi * floorf(angle / Math::TwoPi);
		else
			return angle;
	}

	static float DegToRad(float degrees)
	{
		static const float toRad = Pi / 180.0f;

		return degrees * toRad;
	}

	static float RadToDeg(float radians)
	{
		static const float toDeg = 180.0f / Pi;

		return radians * toDeg;
	}
};
}