#include "vector2d.h"
#include "vector2dInt.h"
#include "Engine/Util/Math/MyMath.h"

#pragma region Operators

vec2 vec2::operator+(const vec2& _other) const { return vec2(x + _other.x, y + _other.y); }
vec2 vec2::operator-(const vec2& _other) const { return vec2(x - _other.x, y - _other.y); }
vec2 vec2::operator*(const vec2& _other) const { return vec2(x * _other.x, y * _other.y); }
vec2 vec2::operator/(const vec2& _other) const { return vec2(x / _other.x, y / _other.y); }

vec2 vec2::operator+(const vec2Int& _other) const { return vec2(x + _other.x, y + _other.y); }
vec2 vec2::operator-(const vec2Int& _other) const { return vec2(x - _other.x, y - _other.y); }
vec2 vec2::operator*(const vec2Int& _other) const { return vec2(x * _other.x, y * _other.y); }
vec2 vec2::operator/(const vec2Int& _other) const { return vec2(x / _other.x, y / _other.y); }

void vec2::operator=(const vec2& _other)
{
	x = _other.x;
	y = _other.y;
}

bool vec2::operator==(const vec2& _other) const
{
	return x == _other.x && y == _other.y;
}

#pragma endregion

#pragma region Constructors

vec2::vec2(const vec2Int& _other) : x{ static_cast<float>(_other.x) }, y{ static_cast<float>(_other.y)} {};

#pragma endregion


#pragma region Public Methods

float vec2::ClockwiseAngle(const vec2& _other) const
{
	float signedAngle = SignedAngle(_other);
	return signedAngle < 0 ? (signedAngle + MyMath::PI) : signedAngle;
}

vec2 vec2::RotateBy(const double _radAngle) const
{
	return vec2(static_cast<float>(cos(-_radAngle) * x - sin(-_radAngle) * y), 
							static_cast<float>(sin(-_radAngle) * x + cos(-_radAngle) * y));
}

#pragma endregion
