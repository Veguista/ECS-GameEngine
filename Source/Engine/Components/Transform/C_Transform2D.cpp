#include "C_Transform2D.h"
#include "Engine/ExternalLibraries/Pugixml/pugixml.hpp"
#include "Engine/Util/Math/MyMath.h"
#include "Engine/Util/XML/XML_File_Handler.h"
#include <stdio.h>

void C_Transform2D::SetTransformValues(vec2 _position, vec2 _orientation, vec2 _scale)
{
	m_pos = _position;
	SetRotation(_orientation);
	m_scale = _scale;
}

void C_Transform2D::SetTransformValues(vec2 _position, float _rotation, vec2 _scale)
{
	m_pos = _position;
	SetRotation(_rotation);
	m_scale = _scale;
}

void C_Transform2D::SetPosition(vec2 _position)
{
	m_pos = _position;
}

void C_Transform2D::SetRotation(float _rotation)
{
	if (_rotation >= 0 && _rotation < 360)
	{
		m_rotation = _rotation;
		return;
	}

 	int naturalPart = static_cast<int>(_rotation);
	float decimalPart = _rotation - naturalPart;

	int sumHowManyTimes = naturalPart % 360;

	while (sumHowManyTimes < 0)
	{
		sumHowManyTimes += 1;
		naturalPart += 360;
	}
	while (sumHowManyTimes > 0)
	{
		sumHowManyTimes -= 1;
		naturalPart -= 360;
	}

	m_rotation = static_cast<float>(naturalPart) + decimalPart;
}

void C_Transform2D::SetRotation(vec2 _orientation)
{
	SetRotation(ENTITY_ORIENTATION_IDENTITY.ClockwiseAngle(_orientation) * MyMath::RadToDegrees);
}

C_Transform2D::C_Transform2D(vec2 _position, float _rotation) : m_pos{ _position }, m_rotation{ _rotation }
{
	SetRotation(_rotation);
}

C_Transform2D::C_Transform2D(vec2 _position, float _rotation, vec2 _scale) : m_pos{ _position }, m_scale{ _scale }
{
	SetRotation(_rotation);
}

C_Transform2D::C_Transform2D(vec2 _position, vec2 _orientation, vec2 _scale) : m_pos{ _position }, m_scale{ _scale }
{
	SetRotation(_orientation);
}

bool C_Transform2D::Serialize(pugi::xml_node* _ComponentNode) const
{
	if (_ComponentNode == nullptr || _ComponentNode->empty())
	{
		return false;
	}

	pugi::xml_node positionNode = _ComponentNode->append_child("Position");
	XML_UTIL::SaveToXMLNode(m_pos, positionNode);

	pugi::xml_node rotationNode = _ComponentNode->append_child("Rotation");
	XML_UTIL::SaveToXMLNode(m_rotation, rotationNode);

	pugi::xml_node scaleNode = _ComponentNode->append_child("Scale");
	XML_UTIL::SaveToXMLNode(m_scale, scaleNode);

	return true;
}

bool C_Transform2D::Load(const pugi::xml_node* _ComponentNode)
{
	if (_ComponentNode == nullptr || _ComponentNode->empty())
	{
		return false;
	}

	pugi::xml_node positionNode = _ComponentNode->child("Position");
	XML_UTIL::LoadXMLNodeToVariable(m_pos, positionNode);

	pugi::xml_node rotationNode = _ComponentNode->child("Rotation");
	XML_UTIL::LoadXMLNodeToVariable(m_rotation, rotationNode);

	pugi::xml_node scaleNode = _ComponentNode->child("Scale");
	XML_UTIL::LoadXMLNodeToVariable(m_scale, scaleNode);

	return true;
}

