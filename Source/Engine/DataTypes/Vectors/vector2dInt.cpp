#include "vector2dInt.h"
#include "vector2d.h"
#include <cmath>

#pragma region Operators

vec2Int vec2Int::operator+(const vec2Int& _other) const { return vec2Int(x + _other.x, y + _other.y); }
vec2Int vec2Int::operator-(const vec2Int& _other) const { return vec2Int(x - _other.x, y - _other.y); }
vec2Int vec2Int::operator*(const vec2Int& _other) const { return vec2Int(x * _other.x, y * _other.y); }
vec2Int vec2Int::operator/(const vec2Int& _other) const { return vec2Int(x / _other.x, y / _other.y); }

void vec2Int::operator=(const vec2Int& _other)
{
	x = _other.x;
	y = _other.y;
}

#pragma endregion

#pragma region Public Methods

vec2Int vec2Int::Absolute() const { return vec2Int(abs(x), abs(y)); }
float vec2Int::Length() const { return sqrt(x * x + y * y); }
vec2 vec2Int::Normal() const { return vec2(x, y) / Length(); }
float vec2Int::Dot(const vec2Int& _other) const { return x * _other.x + y * _other.y; }

float vec2Int::Angle(const vec2Int& _other) const // Clockwise
{
	float dotProduct = Dot(_other);
	float determinant = x * _other.y - y * _other.x;
	return atan2(determinant, dotProduct);
}

float vec2Int::Distance(const vec2Int& _other) const { return (*this - _other).Length(); }

#pragma endregion