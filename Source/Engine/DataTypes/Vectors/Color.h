#pragma once

struct vec3;
struct vec4;
struct vec3Int;
struct vec4Int;

struct Color
{
	unsigned char r{ 0 };
	unsigned char g{ 0 };
	unsigned char b{ 0 };
	unsigned char a{ 255 };

#pragma region Constructor

	Color() = default;
	Color(unsigned char _r, unsigned char _g, unsigned char _b) : r{ _r }, g{ _g }, b{ _b } {}
	Color(unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a) : r{ _r }, g{ _g }, b{ _b }, a{ _a } {}

	Color(unsigned int _r, unsigned int _g, unsigned int _b) : r{ static_cast<unsigned char>(_r) }, g{ static_cast<unsigned char>(_g) }, b{ static_cast<unsigned char>(_b) } {}
	Color(unsigned int _r, unsigned int _g, unsigned int _b, unsigned int _a) : r{ static_cast<unsigned char>(_r) }, g{ static_cast<unsigned char>(_g) }, b{ static_cast<unsigned char>(_b) }, a{ static_cast<unsigned char>(_a) } {}
	
	Color(float _r, float _g, float _b) : r{ static_cast<unsigned char>(_r * 255u)}, g{ static_cast<unsigned char>(_g * 255u)}, b{ static_cast<unsigned char>(_b * 255u)} {}
	Color(float _r, float _g, float _b, float _a) : r{ static_cast<unsigned char>(_r * 255u)}, g{ static_cast<unsigned char>(_g * 255u)}, b{ static_cast<unsigned char>(_b * 255u)}, a{ static_cast<unsigned char>(_a * 255u)} {}

	Color(const vec3 _other);
	Color(const vec4 _other);
	Color(const vec3Int _other);
	Color(const vec4Int _other);

#pragma endregion

#pragma region Operators

	Color operator+(const Color& _other) const;
	Color operator-(const Color& _other) const;
	Color operator*(const Color& _other) const;
	Color operator/(const Color& _other) const;

	bool operator==(const Color& _other) const;
	bool operator!=(const Color& _other) const;

	void operator=(const Color& _other);

#pragma endregion

#pragma region Public Methods

	static inline Color White() { return Color(255u, 255u, 255u); }
	static inline Color Black() { return Color(0u, 0u, 0u); }
	static inline Color Red() { return Color(255u, 0u, 0u); }
	static inline Color Green() { return Color(0u, 255u, 0u); }
	static inline Color Blue() { return Color(0u, 0u, 255u); }
	static inline Color Clear() { return Color(0u, 0u, 0u, 0u); }
	static inline Color Grey() { return Color(122u, 122u, 122u); }
	static inline Color Cyan() { return Color(0u, 255u, 255u); }
	static inline Color Magenta() { return Color(255u, 0u, 255u); }
	static inline Color Yellow() { return Color(1.f, 0.92f, 0.016f); }

#pragma endregion

};