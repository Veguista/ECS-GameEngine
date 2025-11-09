#include "vector4d.h"
#include "vector3d.h"
#include "vector2d.h"
#include "vector4dInt.h"
#include "vector3dInt.h"
#include "vector2dInt.h"
#include "Color.h"

#pragma region Operators

vec4 vec4::operator+(const vec4& other) const { return vec4(x + other.x, y + other.y, z + other.z, w + other.w); }
vec4 vec4::operator-(const vec4& other) const { return vec4(x - other.x, y - other.y, z - other.z, w - other.w); }
vec4 vec4::operator*(const vec4& other) const { return vec4(x * other.x, y * other.y, z * other.z, w * other.w); }
vec4 vec4::operator/(const vec4& other) const { return vec4(x / other.x, y / other.y, z / other.z, w / other.w); }

vec4 vec4::operator+(const vec3& other) const { return vec4(x + other.x, y + other.y, z + other.y, w); }
vec4 vec4::operator-(const vec3& other) const { return vec4(x - other.x, y - other.y, z - other.y, w); }
vec4 vec4::operator*(const vec3& other) const { return vec4(x * other.x, y * other.y, z * other.y, 0); }
vec4 vec4::operator/(const vec3& other) const { return vec4(x / other.x, y / other.y, z / other.y, 0); }

vec4 vec4::operator+(const vec2& other) const { return vec4(x + other.x, y + other.y, z, w); }
vec4 vec4::operator-(const vec2& other) const { return vec4(x - other.x, y - other.y, z, w); }
vec4 vec4::operator*(const vec2& other) const { return vec4(x * other.x, y * other.y, 0, 0); }
vec4 vec4::operator/(const vec2& other) const { return vec4(x / other.x, y / other.y, 0, 0); }

vec4 vec4::operator+(const vec4Int& other) const { return vec4(x + other.x, y + other.y, z + other.z, w + other.w); }
vec4 vec4::operator-(const vec4Int& other) const { return vec4(x - other.x, y - other.y, z - other.z, w - other.w); }
vec4 vec4::operator*(const vec4Int& other) const { return vec4(x * other.x, y * other.y, z * other.z, w * other.w); }
vec4 vec4::operator/(const vec4Int& other) const { return vec4(x / other.x, y / other.y, z / other.z, w / other.w); }
										 
vec4 vec4::operator+(const vec3Int& other) const { return vec4(x + other.x, y + other.y, z + other.y, w); }
vec4 vec4::operator-(const vec3Int& other) const { return vec4(x - other.x, y - other.y, z - other.y, w); }
vec4 vec4::operator*(const vec3Int& other) const { return vec4(x * other.x, y * other.y, z * other.y, 0); }
vec4 vec4::operator/(const vec3Int& other) const { return vec4(x / other.x, y / other.y, z / other.y, 0); }
										 
vec4 vec4::operator+(const vec2Int& other) const { return vec4(x + other.x, y + other.y, z, w); }
vec4 vec4::operator-(const vec2Int& other) const { return vec4(x - other.x, y - other.y, z, w); }
vec4 vec4::operator*(const vec2Int& other) const { return vec4(x * other.x, y * other.y, 0, 0); }
vec4 vec4::operator/(const vec2Int& other) const { return vec4(x / other.x, y / other.y, 0, 0); }

vec4 vec4::operator+(const Color& other) const { return vec4(x + other.r, y + other.g, z + other.b, w + other.a); }
vec4 vec4::operator-(const Color& other) const { return vec4(x - other.r, y - other.g, z - other.b, w - other.a); }
vec4 vec4::operator*(const Color& other) const { return vec4(x * other.r, y * other.g, z * other.b, w * other.a); }
vec4 vec4::operator/(const Color& other) const { return vec4(x / other.r, y / other.g, z / other.b, w / other.a); }

void vec4::operator=(const vec4& _other)
{
	x = _other.x;
	y = _other.y;
	z = _other.z;
	w = _other.w;
}

#pragma endregion

#pragma region Constructors

vec4::vec4(const vec3& _other) : x{ _other.x }, y{ _other.y }, z{ _other.z }, w{ 0 } {};
vec4::vec4(const vec2& _other) : x{ _other.x }, y{ _other.y }, z{ 0 }, w{ 0 } {};
vec4::vec4(const vec4Int& _other) : x{ static_cast<float>(_other.x) }, y{ static_cast<float>(_other.y) }, z{ static_cast<float>(_other.z) }, w{ static_cast<float>(_other.w) } {};
vec4::vec4(const vec3Int& _other) : x{ static_cast<float>(_other.x) }, y{ static_cast<float>(_other.y) }, z{ static_cast<float>(_other.z) }, w{ 0 } {};
vec4::vec4(const vec2Int& _other) : x{ static_cast<float>(_other.x) }, y{ static_cast<float>(_other.y) }, z{ 0 }, w{ 0 } {};
vec4::vec4(const Color& _other) : x{ static_cast<float>(_other.r) }, y{ static_cast<float>(_other.g) }, z{ static_cast<float>(_other.b) }, w{ static_cast<float>(_other.a) } {};


#pragma endregion

#pragma region Public Methods

vec4 vec4::Absolute() const { return vec4(abs(x), abs(y), abs(z), abs(w)); }
float vec4::Length() const { return sqrt(x * x + y * y + z * z + w * w); }
vec4 vec4::Normal() const { return (*this) / Length(); }
float vec4::Dot(const vec4& _other) const { return x * _other.x + y * _other.y + z * _other.z + w * _other.w; }

float vec4::Angle(const vec4& _other) const { return acos(Dot(_other) / Length() * _other.Length()); }
float vec4::Distance(const vec4& _other) const { return (*this - _other).Length(); }

#pragma endregion