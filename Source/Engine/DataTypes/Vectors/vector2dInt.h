#pragma once

struct vec2;

struct vec2Int
{
	int x{0};
	int y{0};

#pragma region Operators

	template<typename T>
	vec2Int operator+(const T& _other) const { return vec2Int(x + _other, y + _other); }
	template<typename T>
	vec2Int operator-(const T& _other) const { return vec2Int(x - _other, y - _other); }
	template<typename T>
	vec2Int operator*(const T& _other) const { return vec2Int(x * _other, y * _other); }
	template<typename T>
	vec2Int operator/(const T& _other) const { return vec2Int(x / _other, y / _other); }

	vec2Int operator+(const vec2Int& _other) const;
	vec2Int operator-(const vec2Int& _other) const;
	vec2Int operator*(const vec2Int& _other) const;
	vec2Int operator/(const vec2Int& _other) const;

	void operator=(const vec2Int& _other);

#pragma endregion

#pragma region Constructors

	constexpr vec2Int() {}
	vec2Int(int _x, int _y) : x{ _x }, y{ _y } {}
	vec2Int(const vec2Int& _other) : x{ _other.x }, y{ _other.y } {}

#pragma endregion

#pragma region Public Methods

	vec2Int Absolute() const;
	float Length() const;
	vec2 Normal() const;
  float Dot(const vec2Int& _other) const;

	float Angle(const vec2Int& _other) const;
	float Distance(const vec2Int& _other) const;

	static inline vec2Int Zero() { return vec2Int(0, 0); }
	static inline vec2Int Up() { return vec2Int(0, 1); }
	static inline vec2Int Down() { return vec2Int(0, -1); }
	static inline vec2Int Right() { return vec2Int(1, 0); }
	static inline vec2Int Left() { return vec2Int(-1, 0); }

#pragma endregion

};