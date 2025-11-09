#pragma once

#include <cmath>

struct vec2Int;

struct vec2
{
	float x{0};
	float y{0};

#pragma region Operators

	template<typename T>
	vec2 operator+(const T& _other) const { return vec2(x + _other, y + _other); }
	template<typename T>
	vec2 operator-(const T& _other) const { return vec2(x - _other, y - _other); }
	template<typename T>
	vec2 operator*(const T& _other) const { return vec2(x * _other, y * _other); }
	template<typename T>
	vec2 operator/(const T& _other) const { return vec2(x / _other, y / _other); }

	vec2 operator+(const vec2& _other) const;
	vec2 operator-(const vec2& _other) const;
	vec2 operator*(const vec2& _other) const;
	vec2 operator/(const vec2& _other) const;

	vec2 operator+(const vec2Int& _other) const;
	vec2 operator-(const vec2Int& _other) const;
	vec2 operator*(const vec2Int& _other) const;
	vec2 operator/(const vec2Int& _other) const;

	void operator=(const vec2& _other);
	bool operator==(const vec2& _other) const;

#pragma endregion

#pragma region Constructors

	constexpr vec2(){}
	vec2(float _x, float _y) : x{ _x }, y{ _y } {}
 	vec2(const vec2& _other) : x{ _other.x }, y{ _other.y } {}
	vec2(const vec2Int& _other);

	template<typename T>
	vec2(const T& _x, const T& _y) : x{ static_cast<float>(_x) }, y{ static_cast<float>(_y) } {}

	template<typename T, typename U>
	vec2(const T& _x, const U& _y) : x{ static_cast<float>(_x) }, y{ static_cast<float>(_y) } {}

#pragma endregion

#pragma region Public Methods

	inline vec2 Absolute() const { return vec2(abs(x), abs(y)); };
	inline float Length() const { return sqrt(x * x + y * y); };
	inline vec2 Normal() const { return (*this) / Length(); };
	inline float Dot(const vec2& _other) const { return x * _other.x + y * _other.y; };

	/// <summary>
	/// Shortest angle in radiants between both vectors.
	/// </summary>
	/// <param name="_other"></param>
	/// <returns></returns>
	inline float Angle(const vec2& _other) const { return acos(Dot(_other) / Length() * _other.Length()); };
	
	/// <summary>
	/// Returns the signed angle (in clockwise orientation, radiants) from original vector to parameter vector.
	/// <para>The result will be inside the [-PI, PI] range.
	/// </para>
	/// </summary>
	/// <param name="_other"></param>
	/// <returns></returns>
	inline float SignedAngle(const vec2& _other) const 
		// U'll notice that the formula is kinda inverted here (we use the _other vector as the main one and viceversa).
		// That is to set the orientation of the Signed Angle clockwise.

									// determinant								 // dotProduct
		{ return atan2(_other.x * y - _other.y * x, _other.Dot(*this)); };

	/// <summary>
	/// Calculates the clockwise angle (in radiants) from owner vector to parameter vector.
	/// </summary>
	/// <param name="_other"></param>
	/// <returns></returns>
	float ClockwiseAngle(const vec2& _other) const;
  vec2 RotateBy(const double _radAngle) const;
	inline float Distance(const vec2& _other) const { return (*this - _other).Length(); };

	static inline vec2 Zero() { return vec2(0, 0); }
	static inline vec2 Up() { return vec2(0, 1); }
	static inline vec2 Down() { return vec2(0, -1); }
	static inline vec2 Right() { return vec2(1, 0); }
	static inline vec2 Left() { return vec2(-1, 0); }

#pragma endregion

};