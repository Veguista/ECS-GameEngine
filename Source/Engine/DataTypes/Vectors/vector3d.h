#pragma once

struct vec2;
struct vec2Int;
struct vec3Int;

struct vec3
{
	float x{0};
	float y{0};
	float z{0};

#pragma region Operators

	template<typename T>
	vec3 operator+(const T& _other) const { return vec3(x + _other, y + _other, z + _other); }
	template<typename T>
	vec3 operator-(const T& _other) const { return vec3(x - _other, y - _other, z - _other); }
	template<typename T>
	vec3 operator*(const T& _other) const { return vec3(x * _other, y * _other, z * _other); }
	template<typename T>
	vec3 operator/(const T& _other) const { return vec3(x / _other, y / _other, z / _other); }

	vec3 operator+(const vec2& _other) const;
	vec3 operator-(const vec2& _other) const;
	vec3 operator*(const vec2& _other) const;
	vec3 operator/(const vec2& _other) const;

	vec3 operator+(const vec3& _other) const;
	vec3 operator-(const vec3& _other) const;
	vec3 operator*(const vec3& _other) const;
	vec3 operator/(const vec3& _other) const;

	vec3 operator+(const vec2Int& _other) const;
	vec3 operator-(const vec2Int& _other) const;
	vec3 operator*(const vec2Int& _other) const;
	vec3 operator/(const vec2Int& _other) const;

	vec3 operator+(const vec3Int& _other) const;
	vec3 operator-(const vec3Int& _other) const;
	vec3 operator*(const vec3Int& _other) const;
	vec3 operator/(const vec3Int& _other) const;

	void operator=(const vec3& _other);

#pragma endregion

#pragma region Constructors

	vec3() {}
	vec3(float _x, float _y, float _z) : x{ _x }, y{ _y }, z{ _z } {}
	vec3(const vec3& other) : x{ other.x }, y{ other.y }, z{ other.z } {}
	vec3(const vec2& other);
	vec3(const vec3Int& _other);
	vec3(const vec2Int& _other);

#pragma endregion

#pragma region Public Methods

	vec3 Absolute() const;
	float Length() const;
	vec3 Normal() const;
	float Dot(const vec3& other) const;

	float Angle(const vec3& other) const;
	float Distance(const vec3& other) const;

	static inline vec3 Zero() { return vec3(0, 0, 0); }
	static inline vec3 Up() { return vec3(0, 0, 1); }
	static inline vec3 Down() { return vec3(0, 0, -1); }
	static inline vec3 Forward() { return vec3(1, 0, 0); }
	static inline vec3 Backward() { return vec3(-1, 0, 0); }
	static inline vec3 Right() { return vec3(1, 0, 0); }
	static inline vec3 Left() { return vec3(-1, 0, 0); }

#pragma endregion
};