#include "Color.h"
#include "vector3d.h"
#include "vector3dInt.h"
#include "vector4d.h"
#include "vector4dInt.h"

#pragma region Constructors

Color::Color(const vec3 _other) : r{ static_cast<unsigned char>(_other.x) }, g{ static_cast<unsigned char>(_other.y) }, b{ static_cast<unsigned char>(_other.z) } {};
Color::Color(const vec3Int _other) : r{ static_cast<unsigned char>(_other.x) }, g{ static_cast<unsigned char>(_other.y) }, b{ static_cast<unsigned char>(_other.z) } {};
Color::Color(const vec4 _other) : r{ static_cast<unsigned char>(_other.x) }, g{ static_cast<unsigned char>(_other.y) }, b{ static_cast<unsigned char>(_other.z) }, a{ static_cast<unsigned char>(_other.w) } {};
Color::Color(const vec4Int _other) : r{ static_cast<unsigned char>(_other.x) }, g{ static_cast<unsigned char>(_other.y) }, b{ static_cast<unsigned char>(_other.z) }, a{ static_cast<unsigned char>(_other.w) } {};

#pragma endregion

#pragma region Operators

Color Color::operator+(const Color& _other) const { return Color(static_cast<unsigned char>(r + _other.r), g + _other.g, b + _other.b, a + _other.a); }
Color Color::operator-(const Color& _other) const { return Color(static_cast<unsigned char>(r - _other.r), g - _other.g, b - _other.b, a - _other.a); }
Color Color::operator*(const Color& _other) const { return Color(static_cast<unsigned char>(r * _other.r), g * _other.g, b * _other.b, a * _other.a); }
Color Color::operator/(const Color& _other) const { return Color(static_cast<unsigned char>(r / _other.r), g / _other.g, b / _other.b, a / _other.a); }

bool Color::operator==(const Color& _other) const { return r == _other.r && g == _other.g && b == _other.b && a == _other.a; }
bool Color::operator!=(const Color& _other) const { return r != _other.r || g != _other.g || b != _other.b || a != _other.a; }

void Color::operator=(const Color& _other) 
{ 
	r = _other.r;
	g = _other.g;
	b = _other.b;
	a = _other.a;
};

#pragma endregion
