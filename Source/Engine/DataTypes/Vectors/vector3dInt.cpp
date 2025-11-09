#include "vector3dInt.h"
#include "vector2dInt.h"
#include "vector3d.h"
#include <cmath>

#pragma region Operators

vec3Int vec3Int::operator+(const vec3Int& _other) const { return vec3Int(x + _other.x, y + _other.y, z + _other.z); }
vec3Int vec3Int::operator-(const vec3Int& _other) const { return vec3Int(x - _other.x, y - _other.y, z - _other.z); }
vec3Int vec3Int::operator*(const vec3Int& _other) const { return vec3Int(x * _other.x, y * _other.y, z * _other.z); }
vec3Int vec3Int::operator/(const vec3Int& _other) const { return vec3Int(x / _other.x, y / _other.y, z / _other.z); }

void vec3Int::operator=(const vec3Int& _other)
{
	x = _other.x;
	y = _other.y;
	z = _other.z;
}

#pragma endregion

#pragma region Constructors

vec3Int::vec3Int(const vec2Int& _other) : x{ _other.x }, y{ _other.y }, z{ 0 } {};

#pragma endregion

#pragma region Public Methods

vec3Int vec3Int::Absolute() const { return vec3Int(abs(x), abs(y), abs(z)); }
float vec3Int::Length() const { return sqrt(x * x + y * y + z * z); }
vec3 vec3Int::Normal() const { return vec3(x, y, z) / Length(); }
float vec3Int::Dot(const vec3Int& _other) const { return x * _other.x + y * _other.y + z * _other.z; }

float vec3Int::Angle(const vec3Int& _other) const // Clockwise
{
	return acos(Dot(_other) / Length() * _other.Length());
}

float vec3Int::Distance(const vec3Int& _other) const { return (*this - _other).Length(); }

#pragma endregion