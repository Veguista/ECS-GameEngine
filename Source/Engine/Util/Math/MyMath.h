#pragma once

#include "Engine/DataTypes/Vectors/vector2d.h"
#include <cmath>
#include <numbers>

struct vec2;

namespace MyMath
{
	constexpr double PI = std::numbers::pi;
	constexpr double RadToDegrees = 180 / PI;
	constexpr double DegreesToRad = PI / 180;

	static float AreaOfTriangle(const vec2& _p1, const vec2& _p2, const vec2& _p3)
	{
		return abs(_p1.x * (_p2.y - _p3.y) + _p2.x * (_p3.y - _p1.y) + _p3.x * (_p1.y - _p2.y)) / 2;
	}
}
