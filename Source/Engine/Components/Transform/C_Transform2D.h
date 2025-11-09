#pragma once

#include "Engine/DataTypes/Vectors/vector2d.h"
#include "Engine/DataTypes/Vectors/vector3d.h"
#include "Engine/ECS/ECS_Interfaces.h"
#include "Engine/ExternalLibraries/Pugixml/pugixml.hpp"

#define ENTITY_BASE_POSITION vec2{0, 0}
#define ENTITY_ORIENTATION_IDENTITY vec2{0, 1}
#define ENTITY_BASE_SCALE vec2{1, 1}

struct C_Transform2D : IECS_Transform, IECS_Serializable
{
	vec2 m_pos{ ENTITY_BASE_POSITION };
	float m_rotation{ 0 };
	vec2 m_scale{ ENTITY_BASE_SCALE };

	// Constructors
	C_Transform2D() {}
	C_Transform2D(vec2 _position) : m_pos{ _position } {}
	C_Transform2D(vec2 _position, float _rotation);
	C_Transform2D(vec2 _position, vec2 _scale) : m_pos{ _position }, m_scale{ _scale } {}
	C_Transform2D(vec2 _position, float _rotation, vec2 _scale);
	C_Transform2D(vec2 _position, vec2 _orientation, vec2 _scale);

	void SetTransformValues(vec2 _position, vec2 _orientation, vec2 _scale);
	void SetTransformValues(vec2 _position, float _rotation, vec2 _scale);
	void SetPosition(vec2 _position);
	void SetRotation(float _rotation);
	void SetRotation(vec2 _orientation);
	inline void AddDisplacement(const vec2& _position) { m_pos = m_pos + _position; };

	bool Serialize(pugi::xml_node* _ComponentNode) const;
	bool Load(const pugi::xml_node* _ComponentNode);
};