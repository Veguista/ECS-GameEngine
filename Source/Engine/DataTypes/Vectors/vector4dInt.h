#pragma once

struct vec4;
struct vec2Int;
struct vec3Int;
struct Color;

struct vec4Int
{
	int x{ 0 };
	int y{ 0 };
	int z{ 0 };
	int w{ 0 };

#pragma region Operators

	template<typename T>
	vec4Int operator+(const T& _other) const { return vec4Int(x + _other, y + _other, z + _other, w + _other); }
	template<typename T>
	vec4Int operator-(const T& _other) const { return vec4Int(x - _other, y - _other, z - _other, w - _other); }
	template<typename T>
	vec4Int operator*(const T& _other) const { return vec4Int(x * _other, y * _other, z * _other, w * _other); }
	template<typename T>
	vec4Int operator/(const T& _other) const { return vec4Int(x / _other, y / _other, z / _other, w / _other); }

	vec4Int operator+(const vec4Int& _other) const;
	vec4Int operator-(const vec4Int& _other) const;
	vec4Int operator*(const vec4Int& _other) const;
	vec4Int operator/(const vec4Int& _other) const;

	void operator=(const vec4Int& _other);

#pragma endregion

#pragma region Constructors

	constexpr vec4Int() {}
	vec4Int(int _x, int _y, int _z, int _w) : x{ _x }, y{ _y }, z{ _z }, w{ _w } {}
	vec4Int(const vec4Int& _other) : x{ _other.x }, y{ _other.y }, z{ _other.z }, w{ _other.w } {}
	vec4Int(const vec3Int& _other);
	vec4Int(const vec2Int& _other);
	vec4Int(const Color& _other);

#pragma endregion

#pragma region Public Methods

	vec4Int Absolute() const;
	float Length() const;
	vec4 Normal() const;
	float Dot(const vec4Int& _other) const;

	float Angle(const vec4Int& _other) const;
	float Distance(const vec4Int& _other) const;

	static inline vec4Int Zero() { return vec4Int(0, 0, 0, 0); }
	static inline vec4Int One() { return vec4Int(1, 1, 1, 1); }

#pragma endregion

};