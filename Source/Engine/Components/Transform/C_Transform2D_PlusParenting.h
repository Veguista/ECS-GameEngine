#pragma once

#include "Engine/Components/Transform/C_Transform2D.h"
#include "Engine/EngineConfiguration.h"
#include <vector>

struct C_Transform2D_PlusParenting : public C_Transform2D
{
protected:
	C_Transform2D_PlusParenting* m_pParent{ nullptr };
	std::vector<C_Transform2D_PlusParenting*> m_pChildren;

public:
	vec2 GetLocalPosition() const;
	vec2 GetWorldPosition() const;

	float GetLocalRotation() const;
	float GetWorldRotation() const;

	vec2 GetLocalScale() const;
	vec2 GetWorldScale() const;

	void SetParent(C_Transform2D_PlusParenting* _pParentTransform);
	void AddChild(C_Transform2D_PlusParenting* _pChildTransform);
	void RemoveChild(C_Transform2D_PlusParenting* _pChildTransform);
	size_t NumberOfChildren() const;

	C_Transform2D_PlusParenting* TryGetChild(size_t _uChildIndex) const;
	C_Transform2D_PlusParenting* INTERNAL_GetChildWithoutChecks(size_t _uChildIndex) const;

	// Constructors
	C_Transform2D_PlusParenting() : C_Transform2D() {}
	C_Transform2D_PlusParenting(const vec2& _position) : C_Transform2D(_position) {}
	C_Transform2D_PlusParenting(const vec2& _position, float _rotation) : C_Transform2D(_position, _rotation) {}
	C_Transform2D_PlusParenting(const vec2& _position, const vec2& _scale) : C_Transform2D(_position, _scale) {}
	C_Transform2D_PlusParenting(const vec2& _position, float _rotation, const vec2& _scale) : C_Transform2D(_position, _rotation, _scale) {}
	C_Transform2D_PlusParenting(const vec2& _position, const vec2& _orientation, const vec2& _scale) : C_Transform2D(_position, _orientation, _scale) {}

	C_Transform2D_PlusParenting(C_Transform2D_PlusParenting* _pParentTransform, const vec2& _position);
	C_Transform2D_PlusParenting(C_Transform2D_PlusParenting* _pParentTransform, const vec2& _position, float _rotation);
	C_Transform2D_PlusParenting(C_Transform2D_PlusParenting* _pParentTransform, const vec2& _position, const vec2& _scale);
	C_Transform2D_PlusParenting(C_Transform2D_PlusParenting* _pParentTransform, const vec2& _position, float _rotation, const vec2& _scale);
	C_Transform2D_PlusParenting(C_Transform2D_PlusParenting* _pParentTransform, const vec2& _position, const vec2& _orientation, const vec2& _scale);

	// We don't need a destructor to notify children because we deal with destruction of entities within the EntityPool script.
};