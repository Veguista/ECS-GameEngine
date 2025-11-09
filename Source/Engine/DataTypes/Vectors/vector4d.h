#pragma once

struct vec2;
struct vec3;
struct vec2Int;
struct vec3Int;
struct vec4Int;
struct Color;

struct vec4
{
	float x{0};
	float y{0};
	float z{0};
	float w{0};

#pragma region Operators

	template<typename T>
	vec4 operator+(const T& _other) const { return vec4(x + _other, y + _other, z + _other, w + _other); }
	template<typename T>
	vec4 operator-(const T& _other) const { return vec4(x - _other, y - _other, z - _other, w - _other); }
	template<typename T>
	vec4 operator*(const T& _other) const { return vec4(x * _other, y * _other, z * _other, w * _other); }
	template<typename T>
	vec4 operator/(const T& _other) const { return vec4(x / _other, y / _other, z / _other, w / _other); }

	vec4 operator+(const vec4& _other) const;
	vec4 operator-(const vec4& _other) const;
	vec4 operator*(const vec4& _other) const;
	vec4 operator/(const vec4& _other) const;

	vec4 operator+(const vec3& _other) const;
	vec4 operator-(const vec3& _other) const;
	vec4 operator*(const vec3& _other) const;
	vec4 operator/(const vec3& _other) const;

	vec4 operator+(const vec2& _other) const;
	vec4 operator-(const vec2& _other) const;
	vec4 operator*(const vec2& _other) const;
	vec4 operator/(const vec2& _other) const;

	vec4 operator+(const vec4Int& _other) const;
	vec4 operator-(const vec4Int& _other) const;
	vec4 operator*(const vec4Int& _other) const;
	vec4 operator/(const vec4Int& _other) const;
																		 
	vec4 operator+(const vec3Int& _other) const;
	vec4 operator-(const vec3Int& _other) const;
	vec4 operator*(const vec3Int& _other) const;
	vec4 operator/(const vec3Int& _other) const;
																		 
	vec4 operator+(const vec2Int& _other) const;
	vec4 operator-(const vec2Int& _other) const;
	vec4 operator*(const vec2Int& _other) const;
	vec4 operator/(const vec2Int& _other) const;

	vec4 operator+(const Color& _other) const;
	vec4 operator-(const Color& _other) const;
	vec4 operator*(const Color& _other) const;
	vec4 operator/(const Color& _other) const;

	void operator=(const vec4& _other);

#pragma endregion

#pragma region Constructors

	constexpr vec4() {}
	vec4(float _x, float _y, float _z, float _w) : x{ _x }, y{ _y }, z{ _z }, w{ _w } {}
	vec4(const vec4& _other) : x{ _other.x }, y{ _other.y }, z{ _other.z }, w{ _other.w } {}
	vec4(const vec3& _other);
	vec4(const vec2& _other);
	vec4(const vec4Int& _other);
	vec4(const vec3Int& _other);
	vec4(const vec2Int& _other);
	vec4(const Color& _other);

#pragma endregion

#pragma region Public Methods

	vec4 Absolute() const;
	float Length() const;
	vec4 Normal() const;
	float Dot(const vec4& _other) const;

	float Angle(const vec4& _other) const;
	float Distance(const vec4& _other) const;

	static inline vec4 Zero() { return vec4(0, 0, 0, 0); }
	static inline vec4 One() { return vec4(1, 1, 1, 1); }

#pragma endregion

};