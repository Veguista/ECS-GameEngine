#pragma once

#include "Engine/DataTypes/Vectors/vector4d.h"
#include "Engine/ECS/ECS_Interfaces.h"
#include "Engine/ExternalLibraries/Pugixml/pugixml.hpp"
#include <string>

struct Tigr;
struct C_Transform2D;
struct C_Transform2D_PlusParenting;

struct C_TextureRenderer : IECS_Serializable, IECS_Render
{
private:
	bool m_visible{ true };
	
	std::string TextureFilePath;

	unsigned int m_uWidth{ 0 }, m_uHeight{ 0 };
	unsigned char m_uChannels{ 0 };

	// Animation coordinates.
	float m_u0{ 0 };
	float m_u1{ 1 };
	float m_v0{ 0 };
	float m_v1{ 1 };

	Tigr* m_pImage { nullptr };

	vec4 m_tintColor{ vec4(255.0f, 255.0f, 255.0f, 255.0f)};

public:
	C_TextureRenderer() {};
	C_TextureRenderer(const std::string& _ImagePath);
	~C_TextureRenderer();

	void SetDrawingPositionInTexture(float _u0, float _u1, float _v0, float _v1);

	void SetImageAsset(const std::string& _ImagePath);
	inline Tigr* GetImage() const { return m_pImage; };

	inline void SetVisibility(const bool _Visible) { m_visible = _Visible; };
	inline bool GetVisibility() const { return m_visible; };

	void Render(const C_Transform2D* _transform) const;
	void Render(const C_Transform2D_PlusParenting* _transform) const;

	void SetTintColor(const float _r, const float _g, const float _b, const float _a);

	bool Serialize(pugi::xml_node* _ComponentNode);
	bool Load(const pugi::xml_node* _ComponentNode);
};
