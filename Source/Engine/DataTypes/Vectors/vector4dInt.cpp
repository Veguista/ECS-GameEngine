#include "vector4dInt.h"
#include "vector3dInt.h"
#include "vector2dInt.h"
#include "vector4d.h"
#include "Color.h"
#include <cmath>

#pragma region Operators

vec4Int vec4Int::operator+(const vec4Int& _other) const { return vec4Int(x + _other.x, y + _other.y, z + _other.z, w + _other.w); }
vec4Int vec4Int::operator-(const vec4Int& _other) const { return vec4Int(x - _other.x, y - _other.y, z - _other.z, w - _other.w); }
vec4Int vec4Int::operator*(const vec4Int& _other) const { return vec4Int(x * _other.x, y * _other.y, z * _other.z, w * _other.w); }
vec4Int vec4Int::operator/(const vec4Int& _other) const { return vec4Int(x / _other.x, y / _other.y, z / _other.z, w / _other.w); }

void vec4Int::operator=(const vec4Int& _other)
{
	x = _other.x;
	y = _other.y;
	z = _other.z;
	w = _other.w;
}

#pragma endregion

#pragma region Constructors

vec4Int::vec4Int(const vec3Int& _other) : x{ _other.x }, y{ _other.y }, z{ _other.z }, w{ 0 } {};
vec4Int::vec4Int(const vec2Int& _other) : x{ _other.x }, y{ _other.y }, z{ 0 }, w{ 0 } {};
vec4Int::vec4Int(const Color& _other) : x{ _other.r }, y{ _other.g }, z{ _other.b }, w{ _other.a } {};

#pragma endregion

#pragma region Public Methods

vec4Int vec4Int::Absolute() const { return vec4Int(abs(x), abs(y), abs(z), abs(w)); }
float vec4Int::Length() const { return sqrt(x * x + y * y + z * z + w * w); }
vec4 vec4Int::Normal() const { return vec4(x, y, z, w) / Length(); }
float vec4Int::Dot(const vec4Int& _other) const { return x * _other.x + y * _other.y + z * _other.z + w * _other.w; }

float vec4Int::Angle(const vec4Int& _other) const // Clockwise
{
	return acos(Dot(_other) / Length() * _other.Length());
}

float vec4Int::Distance(const vec4Int& _other) const { return (*this - _other).Length(); }

#pragma endregion
