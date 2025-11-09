#include "vector3d.h"
#include "vector2d.h"
#include "vector3dInt.h"
#include "vector2dInt.h"

#pragma region Operators

vec3 vec3::operator+(const vec3& _other) const { return vec3(x + _other.x, y + _other.y, z + _other.z); }
vec3 vec3::operator-(const vec3& _other) const { return vec3(x - _other.x, y - _other.y, z - _other.z); }
vec3 vec3::operator*(const vec3& _other) const { return vec3(x * _other.x, y * _other.y, z * _other.z); }
vec3 vec3::operator/(const vec3& _other) const { return vec3(x / _other.x, y / _other.y, z / _other.z); }

vec3 vec3::operator+(const vec2& _other) const { return vec3(x + _other.x, y + _other.y, z); }
vec3 vec3::operator-(const vec2& _other) const { return vec3(x - _other.x, y - _other.y, z); }
vec3 vec3::operator*(const vec2& _other) const { return vec3(x * _other.x, y * _other.y, 0); }
vec3 vec3::operator/(const vec2& _other) const { return vec3(x / _other.x, y / _other.y, 0); }

vec3 vec3::operator+(const vec3Int& _other) const { return vec3(x + _other.x, y + _other.y, z + _other.z); }
vec3 vec3::operator-(const vec3Int& _other) const { return vec3(x - _other.x, y - _other.y, z - _other.z); }
vec3 vec3::operator*(const vec3Int& _other) const { return vec3(x * _other.x, y * _other.y, z * _other.z); }
vec3 vec3::operator/(const vec3Int& _other) const { return vec3(x / _other.x, y / _other.y, z / _other.z); }
										 
vec3 vec3::operator+(const vec2Int& _other) const { return vec3(x + _other.x, y + _other.y, z); }
vec3 vec3::operator-(const vec2Int& _other) const { return vec3(x - _other.x, y - _other.y, z); }
vec3 vec3::operator*(const vec2Int& _other) const { return vec3(x * _other.x, y * _other.y, 0); }
vec3 vec3::operator/(const vec2Int& _other) const { return vec3(x / _other.x, y / _other.y, 0); }

void vec3::operator=(const vec3& _other)
{
	x = _other.x;
	y = _other.y;
	z = _other.z;
}

#pragma endregion

vec3::vec3(const vec2& _other) : x{ _other.x }, y{ _other.y }, z{ 0 } {};
vec3::vec3(const vec3Int& _other) : x{ static_cast<float>(_other.x) }, y{ static_cast<float>(_other.y) }, z{ static_cast<float>(_other.z) } {};
vec3::vec3(const vec2Int& _other) : x{ static_cast<float>(_other.x) }, y{ static_cast<float>(_other.y) }, z{ 0 } {};

#pragma region Public Methods

vec3 vec3::Absolute() const { return vec3(abs(x), abs(y), abs(z)); }
float vec3::Length() const { return sqrt(x * x + y * y + z * z); }
vec3 vec3::Normal() const { return (*this) / Length(); }
float vec3::Dot(const vec3& _other) const { return x * _other.x + y * _other.y + z * _other.z; }

float vec3::Angle(const vec3& _other) const { return acos(Dot(_other) / Length() * _other.Length()); }
float vec3::Distance(const vec3& _other) const { return (*this - _other).Length(); }

#pragma endregion