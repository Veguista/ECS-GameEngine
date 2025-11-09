#include "C_RigidBody2D.h"
#include "Engine/Components/Transform/C_Transform2D.h"
#include "Engine/Util/XML/XML_File_Handler.h"

void C_Rigidbody2D::UpdatePhysics(C_Transform2D& _transform, float _deltaTime)
{
	// Applying gravity.
	m_velocity = m_velocity + vec2::Down() * -9.8f * _deltaTime * m_gravityScale;

	// Drag Check
	if (m_velocity.Length() != 0)
	{
		m_velocity = m_velocity - m_velocity * m_dragScale * _deltaTime;

		// Applying and clamping velocity.
		if (m_velocity.Length() > 0.001f)
		{
			_transform.m_pos = _transform.m_pos + m_velocity * _deltaTime;
		}
		else
		{
			m_velocity = vec2::Zero();
		}
	}
}

bool C_Rigidbody2D::Serialize(pugi::xml_node* _ComponentNode)
{
	if (_ComponentNode == nullptr || _ComponentNode->empty())
	{
		return false;
	}

	pugi::xml_node velocityNode = _ComponentNode->append_child("Velocity");
	XML_UTIL::SaveToXMLNode(m_velocity, velocityNode);

	pugi::xml_node dragScaleNode = _ComponentNode->append_child("DragScale");
	XML_UTIL::SaveToXMLNode(m_dragScale, dragScaleNode);

	pugi::xml_node gravityScaleNode = _ComponentNode->append_child("GravityScale");
	XML_UTIL::SaveToXMLNode(m_gravityScale, gravityScaleNode);

	return true;
}

bool C_Rigidbody2D::Load(const pugi::xml_node* _ComponentNode)
{
	if (_ComponentNode == nullptr || _ComponentNode->empty())
	{
		return false;
	}

	bool hadFailedLoads = false;

	pugi::xml_node velocityNode = _ComponentNode->child("Velocity");
	if (!velocityNode.empty())
	{
		XML_UTIL::LoadXMLNodeToVariable(m_velocity, velocityNode);
	}
	else
	{
		hadFailedLoads = true;
	}

	pugi::xml_node dragScaleNode = _ComponentNode->child("DragScale");
	if (!dragScaleNode.empty())
	{
		XML_UTIL::LoadXMLNodeToVariable(m_dragScale, dragScaleNode);
	}
	else
	{
		hadFailedLoads = true;
	}

	pugi::xml_node gravityScaleNode = _ComponentNode->child("GravityScale");
	if (!gravityScaleNode.empty())
	{
		XML_UTIL::LoadXMLNodeToVariable(m_gravityScale, gravityScaleNode);
	}
	else
	{
		hadFailedLoads = true;
	}

	return !hadFailedLoads;
}

C_Rigidbody2D::C_Rigidbody2D(vec2 _initialVelocity) : m_velocity{ _initialVelocity } {}

C_Rigidbody2D::C_Rigidbody2D(vec2 _initialVelocity, float _dragScale) : m_velocity{ _initialVelocity }, m_dragScale{ _dragScale } {}
