#pragma once

#include "Engine/DataTypes/Vectors/vector2d.h"
#include "Engine/ECS/ECS_Interfaces.h"
#include "Engine/ExternalLibraries/Pugixml/pugixml.hpp"

struct C_Transform2D;

struct C_Collider2D : IECS_Serializable
{
	enum CollisionType_2D
	{
		no_collision = 1,
		rectangle = 2,
		circle = 3,
	};

private:
	CollisionType_2D m_collisionType { rectangle };

public:
	vec2 m_Dimensions{ 1,1 };
	vec2 m_Offset{ 0,0 };

	C_Collider2D() {};
	C_Collider2D(CollisionType_2D _collisionType, vec2 _dimensions = vec2(1, 1), vec2 _offset = vec2(0, 0))
		: m_collisionType {_collisionType}, m_Dimensions{ _dimensions }, m_Offset{ _offset }
	{}

	// Public Methods
	inline CollisionType_2D GetCollisionType() const { return m_collisionType; };
	inline void SetCollisionType(CollisionType_2D _newCollisionType) { m_collisionType = _newCollisionType; };

	bool CheckOverlap(const C_Transform2D& _thisTransform, const C_Collider2D& _otherCollider, const C_Transform2D& _otherTransform) const;

	bool Serialize(pugi::xml_node* _ComponentNode);
	bool Load(const pugi::xml_node* _ComponentNode);

private:
	// Collision functions.
	static bool Check_AABBRectangle_AABBRectangle(const vec2& _leftCornerPos1, const vec2& _dimensions1, const vec2& _leftCornerPos2, const vec2& _dimensions2);
	static bool Check_AABBRectangle_RotatingRectangle(const C_Collider2D& _firstCollider, const C_Transform2D& _firstTransform, const C_Collider2D& _rotatingCollider, const C_Transform2D& _rotatingTransform);
	static bool Check_AABBRectangle_Circle(const vec2& _leftCornerPos, const vec2& _squareDimensions, const vec2& _circleCentre, const vec2& _circleDimensions);

	static bool Check_RotatingRectangle_RotatingRectangle(const C_Collider2D& _firstCollider, const C_Transform2D& _firstTransform, const C_Collider2D& _otherCollider, const C_Transform2D& _otherTransform);
	static bool Check_RotatingRectangle_Circle(const C_Collider2D& _squareCollider, const C_Transform2D& _squareTransform, const vec2& _circleCentre, const vec2& _circleRadius);

	static bool Check_Circle_Circle(const vec2& _circleCentre1, const vec2& _circleDimensions1, const vec2& _circleCentre2, const vec2& _circleDimensions2);

	static void INTERNAL_Calculate_MinMax_Values(float* _minX, float* _maxX, float* _minY, float* _maxY,
		const vec2& _point1, const vec2& _point2, const vec2& _point3, const vec2& _point4);

	static bool INTERNAL_Check_SAT_Collision_AlongAxis(vec2 _axisDirection, 
		const vec2* _aPoints, unsigned int _countAPoints, const vec2* _bPoints, unsigned int _countBPoints);
};

bool operator==(const char& _collisionType1, const C_Collider2D::CollisionType_2D& _collisionType2);
bool operator==(const C_Collider2D::CollisionType_2D& _collisionType1, const char& _collisionType2);

bool operator!=(const char& _collisionType1, const C_Collider2D::CollisionType_2D& _collisionType2);
bool operator!=(const C_Collider2D::CollisionType_2D& _collisionType1, const char& _collisionType2);