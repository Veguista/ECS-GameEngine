#include "C_Transform2D_PlusParenting.h"
#include "Engine/Util/Math/MyMath.h"

#ifdef DEBUG
#include "stdio.h"
#endif // DEBUG

vec2 C_Transform2D_PlusParenting::GetLocalPosition() const
{
	return m_pos;
}

vec2 C_Transform2D_PlusParenting::GetWorldPosition() const
{
	if (m_pParent != nullptr)
	{
		return m_pParent->GetWorldPosition() + (m_pos * m_scale).RotateBy(m_pParent->GetWorldRotation() * MyMath::DegreesToRad);
	}

	return m_pos;
}

float C_Transform2D_PlusParenting::GetLocalRotation() const
{
	return m_rotation;
}

float C_Transform2D_PlusParenting::GetWorldRotation() const
{
	if (m_pParent != nullptr)
	{
		float rotation = m_pParent->GetWorldRotation() + m_rotation;

		if (rotation > 360)
		{
			rotation -= 360 * (static_cast<int>(rotation) / 360);
		}

		return rotation;
	}

	return m_rotation;
}

vec2 C_Transform2D_PlusParenting::GetLocalScale() const
{
	return m_scale;
}

vec2 C_Transform2D_PlusParenting::GetWorldScale() const
{
	if (m_pParent != nullptr)
	{
		return m_pParent->GetWorldScale() * m_scale;
	}

	return m_scale;
}

void C_Transform2D_PlusParenting::SetParent(C_Transform2D_PlusParenting* _pParentTransform)
{
	// Updating old parent.
	if (m_pParent != nullptr)
	{
		m_pParent->RemoveChild(this);
	}

	m_pParent = _pParentTransform;

	// Updating new parent.
	if (_pParentTransform != nullptr)
	{
		_pParentTransform->m_pChildren.push_back(this);
	}
}

void C_Transform2D_PlusParenting::AddChild(C_Transform2D_PlusParenting* _pChildTransform)
{
	if (_pChildTransform == nullptr)
	{
#ifdef DEBUG
		printf("%s(%u) WARNING : Trying to add a child to a Transform but the child's pointer is nullptr.", __FILE__, __LINE__ - 5);
#endif // DEBUG

		return;
	}

	_pChildTransform->SetParent(this);
}

void C_Transform2D_PlusParenting::RemoveChild(C_Transform2D_PlusParenting* _pChildTransform)
{
	if (_pChildTransform == nullptr)
	{
#ifdef DEBUG
		printf("%s(%u) WARNING : Trying to remove a child from a Transform but the child's pointer is nullptr.", __FILE__, __LINE__ - 5);
#endif // DEBUG

		return;
	}

	for (auto it = m_pChildren.begin(); it != m_pChildren.end(); ++it)
	{
		if (*it == _pChildTransform)
		{
			(*it)->m_pParent = nullptr;
			m_pChildren.erase(it);
			return;
		}
	}

#ifdef DEBUG
	printf("%s(%u) WARNING : Trying to remove a child from a Transform but the child wasn't found.", __FILE__, __LINE__ - 22);
#endif // DEBUG
}

size_t C_Transform2D_PlusParenting::NumberOfChildren() const
{
	return m_pChildren.capacity();
}


C_Transform2D_PlusParenting* C_Transform2D_PlusParenting::TryGetChild(size_t _uChildIndex) const
{
	if (_uChildIndex >= m_pChildren.capacity())
	{
#ifdef DEBUG
		printf("%s(%u) WARNING : Trying to access the child number [%u] from Transform but it only has [%u] registered children.", __FILE__, __LINE__ - 5, _uChildIndex, m_pChildren.capacity());
#endif // DEBUG

		return nullptr;
	}

	return m_pChildren[_uChildIndex];
}

C_Transform2D_PlusParenting* C_Transform2D_PlusParenting::INTERNAL_GetChildWithoutChecks(size_t _uChildIndex) const
{
	return m_pChildren[_uChildIndex];
}


#pragma region Constructors

C_Transform2D_PlusParenting::C_Transform2D_PlusParenting(C_Transform2D_PlusParenting* _pParentTransform, const vec2& _position)
	: C_Transform2D(_position)
{
	SetParent(_pParentTransform);
}
C_Transform2D_PlusParenting::C_Transform2D_PlusParenting(C_Transform2D_PlusParenting* _pParentTransform, const vec2& _position, float _rotation)
	: C_Transform2D(_position, _rotation)
{
	SetParent(_pParentTransform);
}
C_Transform2D_PlusParenting::C_Transform2D_PlusParenting(C_Transform2D_PlusParenting* _pParentTransform, const vec2& _position, const vec2& _scale)
	: C_Transform2D(_position, _scale)
{
	SetParent(_pParentTransform);
}
C_Transform2D_PlusParenting::C_Transform2D_PlusParenting(C_Transform2D_PlusParenting* _pParentTransform, const vec2& _position, float _rotation, const vec2& _scale)
	: C_Transform2D(_position, _rotation, _scale)
{
	SetParent(_pParentTransform);
}
C_Transform2D_PlusParenting::C_Transform2D_PlusParenting(C_Transform2D_PlusParenting* _pParentTransform, const vec2& _position, const vec2& _orientation, const vec2& _scale)
	: C_Transform2D(_position, _orientation, _scale)
{
	SetParent(_pParentTransform);
}

#pragma endregion
