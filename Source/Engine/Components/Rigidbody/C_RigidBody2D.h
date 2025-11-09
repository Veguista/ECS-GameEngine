#pragma once

#include "Engine/DataTypes/Vectors/vector2d.h"
#include "Engine/ECS/ECS_Interfaces.h"
#include "Engine/ExternalLibraries/Pugixml/pugixml.hpp"

struct C_Transform2D;

struct C_Rigidbody2D : IECS_Serializable
{
	vec2 m_velocity{ vec2::Zero() };
	float m_dragScale{ 1.5f };
	float m_gravityScale{ 0 };

	void UpdatePhysics(C_Transform2D& _transform, float _deltaTime);

	bool Serialize(pugi::xml_node* _ComponentNode);
	bool Load(const pugi::xml_node* _ComponentNode);

	C_Rigidbody2D(){}
	C_Rigidbody2D(vec2 _initialVelocity);
	C_Rigidbody2D(vec2 _initialVelocity, float _dragScale);
};