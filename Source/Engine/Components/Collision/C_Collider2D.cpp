#include "C_Collider2D.h"
#include "Engine/EngineConfiguration.h"
#include "Engine/Components/Transform/C_Transform2D.h"
#include "Engine/Util/Math/MyMath.h"
#include "Engine/Util/XML/XML_File_Handler.h"

bool C_Collider2D::CheckOverlap(
	const C_Transform2D& _thisTransform, 
	const C_Collider2D& _otherCollider, 
	const C_Transform2D& _otherTransform) const
{
	CollisionType_2D otherColliderType = _otherCollider.GetCollisionType();

	switch (GetCollisionType())
	{
	case CollisionType_2D::no_collision: // No Collision + ________
		return false;

	case CollisionType_2D::rectangle: // Rectangle + ________
		switch (otherColliderType)
		{
		case CollisionType_2D::no_collision:
			return false;
		case CollisionType_2D::rectangle:
			return Check_RotatingRectangle_RotatingRectangle(*this, _thisTransform, _otherCollider, _otherTransform);
		case CollisionType_2D::circle:
			return Check_RotatingRectangle_Circle(*this, _thisTransform, _otherTransform.m_pos + _otherCollider.m_Offset, (_otherCollider.m_Dimensions * _otherTransform.m_scale / 2).Absolute());
		}

	case CollisionType_2D::circle: // Circle + ________
		switch (otherColliderType)
		{
		case CollisionType_2D::no_collision:
			return false;
		case CollisionType_2D::rectangle:
			return Check_RotatingRectangle_Circle(_otherCollider, _otherTransform, _thisTransform.m_pos + this->m_Offset, (this->m_Dimensions * _thisTransform.m_scale / 2).Absolute());
		case CollisionType_2D::circle:
			return Check_Circle_Circle(_thisTransform.m_pos + this->m_Offset, (this->m_Dimensions * _thisTransform.m_scale / 2).Absolute(), _otherTransform.m_pos + _otherCollider.m_Offset, (_otherCollider.m_Dimensions * _otherTransform.m_scale / 2).Absolute());
		}
	}

#ifdef DEBUG
	printf("%s(%u) WARNING : C_Collider2D tried to calculate a collision using an undefined Collision Type.\n", __FILE__, __LINE__ - 31);
#endif // DEBUG

	return false;
}

bool C_Collider2D::Serialize(pugi::xml_node* _ComponentNode)
{
	if (_ComponentNode == nullptr || _ComponentNode->empty())
	{
		return false;
	}

	pugi::xml_node collisionTypeNode = _ComponentNode->append_child("CollisionType");
	XML_UTIL::SaveToXMLNode(static_cast<int>(m_collisionType), collisionTypeNode);

	pugi::xml_node dimensionsNode = _ComponentNode->append_child("Dimensions");
	XML_UTIL::SaveToXMLNode(m_Dimensions, dimensionsNode);

	pugi::xml_node offsetNode = _ComponentNode->append_child("Offset");
	XML_UTIL::SaveToXMLNode(m_Offset, offsetNode);

	return true;
}

bool C_Collider2D::Load(const pugi::xml_node* _ComponentNode)
{
	if (_ComponentNode == nullptr || _ComponentNode->empty())
	{
		return false;
	}

	bool hadFailedLoads = false;

	pugi::xml_node collisionTypeNode = _ComponentNode->child("CollisionType");
	if (!collisionTypeNode.empty())
	{
		int readCollisionType = -1;
		XML_UTIL::LoadXMLNodeToVariable(readCollisionType, collisionTypeNode);
		m_collisionType = static_cast<CollisionType_2D>(readCollisionType);
	}
	else
	{
		hadFailedLoads = true;
	}

	pugi::xml_node dimensionsNode = _ComponentNode->child("Dimensions");
	if (!dimensionsNode.empty())
	{
		XML_UTIL::LoadXMLNodeToVariable(m_Dimensions, dimensionsNode);
	}
	else
	{
		hadFailedLoads = true;
	}

	pugi::xml_node offsetNode = _ComponentNode->child("Offset");
	if (!offsetNode.empty())
	{
		XML_UTIL::LoadXMLNodeToVariable(m_Offset, offsetNode);
	}
	else
	{
		hadFailedLoads = true;
	}

	return !hadFailedLoads;
}

#pragma region Collision methods

bool C_Collider2D::Check_AABBRectangle_AABBRectangle(
	const vec2& _leftCornerPos1, const vec2& _dimensions1, const vec2& _leftCornerPos2, const vec2& _dimensions2)
{
	return _leftCornerPos1.x < _leftCornerPos2.x + _dimensions2.x
		&& _leftCornerPos1.x + _dimensions1.x > _leftCornerPos2.x
		&& _leftCornerPos1.y < _leftCornerPos2.y + _dimensions2.y
		&& _leftCornerPos1.y + _dimensions1.y > _leftCornerPos2.y;
}

bool C_Collider2D::Check_AABBRectangle_RotatingRectangle(const C_Collider2D& _firstCollider, const C_Transform2D& _firstTransform, const C_Collider2D& _rotatingCollider, const C_Transform2D& _rotatingTransform)
{
	// Without rotation, this becomes a AABB rectangle collision
	if (_rotatingTransform.m_rotation == 0 || _rotatingTransform.m_rotation == 180)
	{
		return Check_AABBRectangle_AABBRectangle(
			(_firstTransform.m_pos + _firstCollider.m_Offset) - (_firstTransform.m_scale * _firstCollider.m_Dimensions).Absolute() / 2,
			(_firstTransform.m_scale * _firstCollider.m_Dimensions).Absolute(),
			(_rotatingTransform.m_pos + _rotatingCollider.m_Offset) - (_rotatingTransform.m_scale * _rotatingCollider.m_Dimensions).Absolute() / 2,
			(_rotatingTransform.m_scale * _rotatingCollider.m_Dimensions).Absolute());
	}

	vec2 firstCollCentre = _firstTransform.m_pos + _firstCollider.m_Offset;
	vec2 rotatingCollCentre = _rotatingTransform.m_pos + _rotatingCollider.m_Offset;

	vec2 firstDimensions = (_firstCollider.m_Dimensions * _firstTransform.m_scale).Absolute();
	vec2 rotatingDimensions = (_rotatingCollider.m_Dimensions * _rotatingTransform.m_scale).Absolute();

	// Point[0] = topLeft,  Point[1] = bottomLeft,  Point[2] = topRight,  Point[3] = bottomRight.
	vec2* firstPoints = new vec2[4];
	/* topLeft */			firstPoints[0] = firstCollCentre - firstDimensions / 2;
	/* bottomLeft */	firstPoints[1] = firstCollCentre + vec2(firstDimensions.x / -2, firstDimensions.y / 2);
	/* topRight */		firstPoints[2] = firstCollCentre * 2 - firstPoints[1]; // == point_centre + (V_Corner->Centre) 
	/* bottomRight */ firstPoints[3] = firstCollCentre * 2 - firstPoints[0];

	vec2* rotatingPoints = new vec2[4];
	/* topLeft */			rotatingPoints[0] = rotatingCollCentre + ( rotatingDimensions / -2).RotateBy(_rotatingTransform.m_rotation * MyMath::DegreesToRad);
	/* bottomLeft */	rotatingPoints[1] = rotatingCollCentre + vec2(rotatingDimensions.x / -2, rotatingDimensions.y / 2).RotateBy(_rotatingTransform.m_rotation * MyMath::DegreesToRad);
	/* topRight */		rotatingPoints[2] = rotatingCollCentre * 2 - rotatingPoints[1]; // == point_centre + (V_Corner->Centre) 
	/* bottomRight */ rotatingPoints[3] = rotatingCollCentre * 2 - rotatingPoints[0];

	// Faster calculation to see if they are close enough.
	float rotatingMinX, rotatingMaxX, rotatingMinY, rotatingMaxY;

	INTERNAL_Calculate_MinMax_Values(&rotatingMinX, &rotatingMaxX, &rotatingMinY, &rotatingMaxY,
		rotatingPoints[0], rotatingPoints[1], rotatingPoints[2], rotatingPoints[3]);

	if (firstPoints[0].x > rotatingMaxX || firstPoints[3].x < rotatingMinX || firstPoints[0].y > rotatingMaxY || firstPoints[3].y < rotatingMinY)
	{
		delete[] firstPoints;
		delete[] rotatingPoints;
		return false;
	}

	// If we are here, there is a chance of collision. We do the full SAT-Collision calculation.

	// Small Optimization : Since we know they are both squares, we only need to evaluate 2 of their sides, as the other two are parallel.
	for (unsigned int a = 1; a < 3; a++)
	{
		// Perpendicular Axis to two points.
		if (!INTERNAL_Check_SAT_Collision_AlongAxis(firstPoints[a] - firstPoints[0], firstPoints, 4, rotatingPoints, 4))
		{
			// If our SAT-Collision breaks at any point, there is no collision between both pieces.
			delete[] firstPoints;
			delete[] rotatingPoints;
			return false;
		}
	}

	for (unsigned int b = 1; b < 3; b++)
	{
		// Perpendicular Axis to two points.
		if (!INTERNAL_Check_SAT_Collision_AlongAxis(rotatingPoints[b] - rotatingPoints[0], firstPoints, 4, rotatingPoints, 4))
		{
			// If our SAT-Collision breaks at any point, there is no collision between both pieces.
			delete[] firstPoints;
			delete[] rotatingPoints;
			return false;
		}
	}

	// If at all points the collision existed, both squares are colliding.
	delete[] firstPoints;
	delete[] rotatingPoints;
	return true;
}

bool C_Collider2D::Check_AABBRectangle_Circle(const vec2& _leftCornerPos, const vec2& _squareDimensions, const vec2& _circleCentre, const vec2& _circleRadius)
{
#ifdef DEBUG
	// Checking if the resulting Circles are deformed.
	if (_circleRadius.x != _circleRadius.y || _circleRadius.x != _circleRadius.y)
	{
		printf("%s(%u) WARNING : Trying to calculate a collision using a deformed Circle. Deformed circles are not currently supported.\nThe X-value of the Circle's radius will be used to calculate the Collision.\n", __FILE__, __LINE__ - 6);
	}
#endif // DEBUG

	// We see if the circle centre is inside the square's Y-projection and or X-projection.
	if (_leftCornerPos.y < _circleCentre.y && _leftCornerPos.y + _squareDimensions.y > _circleCentre.y)
	{
		// If it is also in the X-Projection, the circle is inside the square.
		if (_leftCornerPos.x < _circleCentre.x && _leftCornerPos.x + _squareDimensions.x > _circleCentre.x)
		{
			return true;
		}

		// The Normal from the square's side to the circle is vec2::left / right.
		float distanceToSquare = _circleCentre.Distance(
			vec2(_leftCornerPos.x + (_leftCornerPos.x > _circleCentre.x ? 0 : _squareDimensions.x), // Checking is the circle is left or right of the square.
				_circleCentre.y)); // We use the circle's y-pos to trace the smallest distance with the square.

		if (distanceToSquare < _circleRadius.x)
		{
			return true;
		}

		return false;
	}
	if (_leftCornerPos.x < _circleCentre.x && _leftCornerPos.x + _squareDimensions.x > _circleCentre.x)
	{
		// The Normal from the square's side to the circle is vec2::top / bottom.
		float distanceToSquare = _circleCentre.Distance(vec2(_circleCentre.x, // We use the circle's x-pos to trace the smallest distance with the square.
			_leftCornerPos.y + (_leftCornerPos.y > _circleCentre.y ? 0 : _squareDimensions.y))); // Checking is the circle is top or bottom of the square.
 
		if (distanceToSquare < _circleRadius.x)
		{
			return true;
		}

		return false;
	}

	// Else, the other option is that the square's corners are inside the circle.
	if (_leftCornerPos.Distance(_circleCentre) < _circleRadius.x																		// LeftTop corner
		|| (_leftCornerPos + vec2(_squareDimensions.x, 0)).Distance(_circleCentre) < _circleRadius.x	// RightTop corner
		|| (_leftCornerPos + vec2(0, _squareDimensions.y)).Distance(_circleCentre) < _circleRadius.x	// LeftBottom corner
		|| (_leftCornerPos + _squareDimensions).Distance(_circleCentre) < _circleRadius.x)						// RightBottom corner 
	{
		return true;
	}

	return false;
}

bool C_Collider2D::Check_RotatingRectangle_RotatingRectangle(const C_Collider2D& _firstCollider, const C_Transform2D& _firstTransform, const C_Collider2D& _otherCollider, const C_Transform2D& _otherTransform)
{
	// Without rotation, the problem simplifies a lot.
	if (_firstTransform.m_rotation == 0 || _firstTransform.m_rotation == 180)
	{
		// AABB rectangle collision.
		if (_otherTransform.m_rotation == 0 || _otherTransform.m_rotation == 180)
		{
			return Check_AABBRectangle_AABBRectangle(
				(_firstTransform.m_pos + _firstCollider.m_Offset) - (_firstTransform.m_scale * _firstCollider.m_Dimensions).Absolute() / 2,
				(_firstTransform.m_scale * _firstCollider.m_Dimensions).Absolute(),
				(_otherTransform.m_pos + _otherCollider.m_Offset) - (_otherTransform.m_scale * _otherCollider.m_Dimensions).Absolute() / 2,
				(_otherTransform.m_scale * _otherCollider.m_Dimensions).Absolute());
		}

		return Check_AABBRectangle_RotatingRectangle(_firstCollider, _firstTransform, _otherCollider, _otherTransform);
	}
	if (_otherTransform.m_rotation == 0 || _otherTransform.m_rotation == 180)
	{
		return Check_AABBRectangle_RotatingRectangle(_otherCollider, _otherTransform, _firstCollider, _firstTransform);
	}

	vec2 firstCollCentre = _firstTransform.m_pos + _firstCollider.m_Offset;
	vec2 otherCollCentre = _otherTransform.m_pos + _otherCollider.m_Offset;

	vec2 firstDimensions = (_firstCollider.m_Dimensions * _firstTransform.m_scale).Absolute();
	vec2 otherDimensions = (_otherCollider.m_Dimensions * _otherTransform.m_scale).Absolute();

	// Point[0] = topLeft,  Point[1] = bottomLeft,  Point[2] = topRight,  Point[3] = bottomRight.
	vec2* firstPoints = new vec2[4];
	/* topLeft */			firstPoints[0] = firstCollCentre + (firstDimensions / -2).RotateBy(_firstTransform.m_rotation * MyMath::DegreesToRad);
	/* bottomLeft */	firstPoints[1] = firstCollCentre + vec2(firstDimensions.x / -2, firstDimensions.y / 2).RotateBy(_firstTransform.m_rotation * MyMath::DegreesToRad);
	/* topRight */		firstPoints[2] = firstCollCentre * 2 - firstPoints[1]; // Centre + (centre - corner)
	/* bottomRight */	firstPoints[3] = firstCollCentre * 2 - firstPoints[0];

	vec2* otherPoints = new vec2[4];
	/* topLeft */			otherPoints[0] = otherCollCentre + (otherDimensions / -2).RotateBy(_otherTransform.m_rotation * MyMath::DegreesToRad);
	/* bottomLeft */	otherPoints[1] = otherCollCentre + vec2(otherDimensions.x / -2, otherDimensions.y / 2).RotateBy(_otherTransform.m_rotation * MyMath::DegreesToRad);
	/* topRight */		otherPoints[2] = otherCollCentre * 2 - otherPoints[1]; // Centre + (centre - corner)
	/* bottomRight */ otherPoints[3] = otherCollCentre * 2 - otherPoints[0];

	// Faster calculation to see if they are close enough.
	float firstMinX, firstMaxX, otherMinX, otherMaxX, firstMinY, firstMaxY, otherMinY, otherMaxY;

	INTERNAL_Calculate_MinMax_Values(&firstMinX, &firstMaxX, &firstMinY, &firstMaxY, 
		firstPoints[0], firstPoints[1], firstPoints[2], firstPoints[3]);
	INTERNAL_Calculate_MinMax_Values(&otherMinX, &otherMaxX, &otherMinY, &otherMaxY,
		otherPoints[0], otherPoints[1], otherPoints[2], otherPoints[3]);

	if (firstMinX > otherMaxX || firstMaxX < otherMinX || firstMinY > otherMaxY || firstMaxY < otherMinY)
	{
		delete[] firstPoints;
		delete[] otherPoints;
		return false;
	}

	// If we are here, there is a chance of collision. We do the full SAT-Collision calculation.

	// Small optimization : Since we know they are both squares, we only need to evaluate 2 of their sides, as the other two are parallel.
	for (unsigned int a = 1; a < 3; a++)
	{
																							 // Perpendicular Axis to two points.
		if (!INTERNAL_Check_SAT_Collision_AlongAxis(firstPoints[a] - firstPoints[0], firstPoints, 4, otherPoints, 4))
		{
			// If our SAT-Collision breaks at any point, there is no collision between both pieces.
			delete[] firstPoints;
			delete[] otherPoints;
			return false;
		}
	}

	for (unsigned int b = 1; b < 3; b++)
	{
		// Perpendicular Axis to two points.
		if (!INTERNAL_Check_SAT_Collision_AlongAxis(otherPoints[b] - otherPoints[0], firstPoints, 4, otherPoints, 4))
		{
			// If our SAT-Collision breaks at any point, there is no collision between both pieces.
			delete[] firstPoints;
			delete[] otherPoints;
			return false;
		}
	}

	// If at all points the collision existed, both squares are colliding.

	delete[] firstPoints;
	delete[] otherPoints;
	return true;
}

bool C_Collider2D::Check_RotatingRectangle_Circle(const C_Collider2D& _squareCollider, const C_Transform2D& _squareTransform, const vec2& _circleCentre, const vec2& _circleRadius)
{
#ifdef DEBUG
	// Checking if the resulting Circles are deformed.
	if (_circleRadius.x != _circleRadius.y)
	{
		printf("%s(%u) WARNING : Trying to calculate a collision using a deformed Circle. Deformed circles are not currently supported.\nThe X-value of the Circle's radius will be used to calculate the Collision.\n", __FILE__, __LINE__ - 6);
	}
#endif // DEBUG

	// Simplifying the collision with an unrotated square.
	if (_squareTransform.m_rotation == 0 || _squareTransform.m_rotation == 180)
	{
		return Check_AABBRectangle_Circle(
			(_squareTransform.m_pos + _squareCollider.m_Offset) - (_squareTransform.m_scale * _squareCollider.m_Dimensions).Absolute() / 2,
			(_squareTransform.m_scale * _squareCollider.m_Dimensions).Absolute(),
			_circleCentre, _circleRadius);
	}

	vec2 squareCollCentre = _squareTransform.m_pos + _squareCollider.m_Offset;
	vec2 squareDimensions = (_squareCollider.m_Dimensions * _squareTransform.m_scale).Absolute();

	vec2 squarePoint0 = squareCollCentre + (squareDimensions / -2).RotateBy(_squareTransform.m_rotation * MyMath::DegreesToRad);
	vec2 squarePoint1 = squareCollCentre + vec2(squareDimensions.x / -2, squareDimensions.y / 2).RotateBy(_squareTransform.m_rotation * MyMath::DegreesToRad);
	vec2 squarePoint2 = squareCollCentre * 2 - squarePoint1;
	vec2 squarePoint3 = squareCollCentre * 2 - squarePoint0;

	// If the _circleCentre point is inside the rectangle.
	float area_0C1 = MyMath::AreaOfTriangle(_circleCentre, squarePoint0, squarePoint1);
	float area_1C3 = MyMath::AreaOfTriangle(_circleCentre, squarePoint1, squarePoint3);
	float area_3C2 = MyMath::AreaOfTriangle(_circleCentre, squarePoint3, squarePoint2);
	float area_2C0 = MyMath::AreaOfTriangle(_circleCentre, squarePoint2, squarePoint0);

	if (area_0C1 + area_1C3 + area_3C2 + area_2C0 < squareDimensions.x * squareDimensions.y)
	{ // If the sum of the areas is greater than the rectangle's area, the circle-centre is inside the rectangle.
		return true;
	}

	// If it is not, then we check if the rectangle's corners are inside the circle.
	if (squarePoint0.Distance(_circleCentre) < _circleRadius.x
		|| squarePoint1.Distance(_circleCentre) < _circleRadius.x
		|| squarePoint2.Distance(_circleCentre) < _circleRadius.x
		|| squarePoint3.Distance(_circleCentre) < _circleRadius.x)
	{
		return true;
	}

	return false;
}

bool C_Collider2D::Check_Circle_Circle(const vec2& _circleCentre1, const vec2& _circleRadius1, const vec2& _circleCentre2, const vec2& _circleRadius2)
{
#ifdef DEBUG
	// Checking if the resulting Circles are deformed.
	if (_circleRadius1.x != _circleRadius1.y || _circleRadius2.x != _circleRadius2.y)
	{
		printf("%s(%u) WARNING : Trying to calculate a collision using a deformed Circle. Deformed circles are not currently supported.\nThe X-value of the Circle's radius will be used to calculate the Collision.\n", __FILE__, __LINE__ - 6);
	}
#endif // DEBUG

	return (_circleCentre1 - _circleCentre2).Length() < _circleRadius1.x + _circleRadius2.x;
}

#pragma endregion

void C_Collider2D::INTERNAL_Calculate_MinMax_Values(float* _minX, float* _maxX, float* _minY, float* _maxY, 
	const vec2& _point1, const vec2& _point2, const vec2& _point3, const vec2& _point4)
{
	// X
	if (_point1.x > _point2.x)
	{
		if (_point3.x > _point4.x)
		{
			// Max
			if (_point1.x > _point3.x)
			{
				*_maxX = _point1.x;
			}
			else
			{
				*_maxX = _point3.x;
			}

			// Min
			if (_point2.x < _point4.x)
			{
				*_minX = _point2.x;
			}
			else
			{
				*_minX = _point4.x;
			}
		}
		else
		{
			// Max
			if (_point1.x > _point4.x)
			{
				*_maxX = _point1.x;
			}
			else
			{
				*_maxX = _point4.x;
			}

			// Min
			if (_point2.x < _point3.x)
			{
				*_minX = _point2.x;
			}
			else
			{
				*_minX = _point3.x;
			}
		}
	}
	else
	{
		if (_point3.x > _point4.x)
		{
			// Max
			if (_point2.x > _point3.x)
			{
				*_maxX = _point2.x;
			}
			else
			{
				*_maxX = _point3.x;
			}

			// Min
			if (_point1.x < _point4.x)
			{
				*_minX = _point1.x;
			}
			else
			{
				*_minX = _point4.x;
			}
		}
		else
		{
			// Max
			if (_point2.x > _point4.x)
			{
				*_maxX = _point2.x;
			}
			else
			{
				*_maxX = _point4.x;
			}

			// Min
			if (_point1.x < _point3.x)
			{
				*_minX = _point1.x;
			}
			else
			{
				*_minX = _point3.x;
			}
		}
	}

	// Y
	if (_point1.y > _point2.y)
	{
		if (_point3.y > _point4.y)
		{
			// Max
			if (_point1.y > _point3.y)
			{
				*_maxY = _point1.y;
			}
			else
			{
				*_maxY = _point3.y;
			}

			// Min
			if (_point2.y < _point4.y)
			{
				*_minY = _point2.y;
			}
			else
			{
				*_minY = _point4.y;
			}
		}
		else
		{
			// Max
			if (_point1.y > _point4.y)
			{
				*_maxY = _point1.y;
			}
			else
			{
				*_maxY = _point4.y;
			}

			// Min
			if (_point2.y < _point3.y)
			{
				*_minY = _point2.y;
			}
			else
			{
				*_minY = _point3.y;
			}
		}
	}
	else
	{
		if (_point3.y > _point4.y)
		{
			// Max
			if (_point2.y > _point3.y)
			{
				*_maxY = _point2.y;
			}
			else
			{
				*_maxY = _point3.y;
			}

			// Min
			if (_point1.y < _point4.y)
			{
				*_minY = _point1.y;
			}
			else
			{
				*_minY = _point4.y;
			}
		}
		else
		{
			// Max
			if (_point2.y > _point4.y)
			{
				*_maxY = _point2.y;
			}
			else
			{
				*_maxY = _point4.y;
			}

			// Min
			if (_point1.y < _point3.y)
			{
				*_minY = _point1.y;
			}
			else
			{
				*_minY = _point3.y;
			}
		}
	}
}

bool C_Collider2D::INTERNAL_Check_SAT_Collision_AlongAxis(
	vec2 _axisDirection, 
	const vec2* _aPoints, unsigned int _countAPoints, 
	const vec2* _bPoints, unsigned int _countBPoints)
{
	_axisDirection = _axisDirection.Normal();

	vec2 a_point0 = _aPoints[0];
	vec2 a_point1 = _aPoints[1];
	vec2 a_point2 = _aPoints[2];
	vec2 a_point3 = _aPoints[3];

	vec2 b_point0 = _bPoints[0];
	vec2 b_point1 = _bPoints[1];
	vec2 b_point2 = _bPoints[2];
	vec2 b_point3 = _bPoints[3];

	float minAProjection{ _axisDirection.Dot(_aPoints[0]) };
	float maxAProjection{ minAProjection };

	for (unsigned int a = 1; a < _countAPoints; a++)
	{
		float projectionDistance = _axisDirection.Dot(_aPoints[a]);

		if (projectionDistance > maxAProjection)
		{
			maxAProjection = projectionDistance;
		}
		else if (projectionDistance < minAProjection)
		{
			minAProjection = projectionDistance;
		}
	}

	float minBProjection{ _axisDirection.Dot(_bPoints[0]) };
	float maxBProjection{ minBProjection };

	for (unsigned int b = 1; b < _countBPoints; b++)
	{
		float projectionDistance = _axisDirection.Dot(_bPoints[b]);

		if (projectionDistance > maxBProjection)
		{
			maxBProjection = projectionDistance;
		}
		else if (projectionDistance < minBProjection)
		{
			minBProjection = projectionDistance;
		}
	}

	// Two possible cases for collision. MinA < MinB < MaxA || MinB < MinA < MaxB
	return (minAProjection < minBProjection && minBProjection < maxAProjection)
		|| (minBProjection < minAProjection && minAProjection < maxBProjection);
}

// Operators
bool operator==(const char& _collisionType1, const C_Collider2D::CollisionType_2D& _collisionType2)
{
	return (_collisionType1 < 0 ? -_collisionType1 : _collisionType1 ) == static_cast<char>(_collisionType2);
}
bool operator==(const C_Collider2D::CollisionType_2D& _collisionType1, const char& _collisionType2)
{
	return (_collisionType2 < 0 ? -_collisionType2 : _collisionType2) == static_cast<char>(_collisionType1);
}
bool operator!=(const char& _collisionType1, const C_Collider2D::CollisionType_2D& _collisionType2)
{
	return (_collisionType1 < 0 ? -_collisionType1 : _collisionType1) != static_cast<char>(_collisionType2);
}
bool operator!=(const C_Collider2D::CollisionType_2D& _collisionType1, const char& _collisionType2)
{
	return (_collisionType2 < 0 ? -_collisionType2 : _collisionType2) != static_cast<char>(_collisionType1);
}

