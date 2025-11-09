#pragma once

struct vec3;
struct vec2Int;

struct vec3Int
{
	int x{ 0 };
	int y{ 0 };
	int z{ 0 };

#pragma region Operators

	template<typename T>
	vec3Int operator+(const T& _other) const { return vec3Int(x + _other, y + _other, z + _other); }
	template<typename T>
	vec3Int operator-(const T& _other) const { return vec3Int(x - _other, y - _other, z - _other); }
	template<typename T>
	vec3Int operator*(const T& _other) const { return vec3Int(x * _other, y * _other, z * _other); }
	template<typename T>
	vec3Int operator/(const T& _other) const { return vec3Int(x / _other, y / _other, z / _other); }

	vec3Int operator+(const vec3Int& _other) const;
	vec3Int operator-(const vec3Int& _other) const;
	vec3Int operator*(const vec3Int& _other) const;
	vec3Int operator/(const vec3Int& _other) const;

	void operator=(const vec3Int& _other);

#pragma endregion

#pragma region Constructors

	constexpr vec3Int() {}
	vec3Int(int _x, int _y, int _z) : x{ _x }, y{ _y }, z{ _z } {}
	vec3Int(const vec3Int& _other) : x{ _other.x }, y{ _other.y }, z{ _other.z } {}
	vec3Int(const vec2Int& _other);

#pragma endregion

#pragma region Public Methods

	vec3Int Absolute() const;
	float Length() const;
	vec3 Normal() const;
	float Dot(const vec3Int& _other) const;

	float Angle(const vec3Int& _other) const;
	float Distance(const vec3Int& _other) const;

	static inline vec3Int Zero() { return vec3Int(0, 0, 0); }
	static inline vec3Int Up() { return vec3Int(0, 0, 1); }
	static inline vec3Int Down() { return vec3Int(0, 0, -1); }
	static inline vec3Int Forward() { return vec3Int(1, 0, 0); }
	static inline vec3Int Backward() { return vec3Int(-1, 0, 0); }
	static inline vec3Int Right() { return vec3Int(1, 0, 0); }
	static inline vec3Int Left() { return vec3Int(-1, 0, 0); }

#pragma endregion

};