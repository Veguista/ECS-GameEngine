#include "C_TextureRenderer.h"
#include "Engine/ExternalLibraries/Tigr/tigr.h"
#include "Engine/EngineConfiguration.h"
#include "Engine/Engine.h"
#include "Engine/Util/XML/XML_File_Handler.h"
#include "Engine/Components/Transform/C_Transform2D.h"
#include "Engine/Components/Transform/C_Transform2D_PlusParenting.h"

void C_TextureRenderer::SetImageAsset(const std::string& _ImagePath)
{
	if (m_pImage != nullptr)
	{
		tigrFree(m_pImage);
		m_pImage = nullptr;
		TextureFilePath.clear();
	}
	
	// We do allow to set the reference to null (inside the Renderer T at least).
	if (_ImagePath.empty())
	{
		return;
	}

	m_pImage = tigrLoadImage(_ImagePath.c_str());

	if (m_pImage != nullptr)
	{
		TextureFilePath = _ImagePath;
		m_uWidth = m_pImage->w;
		m_uHeight = m_pImage->h;
		m_uChannels = static_cast<unsigned char>(m_pImage->ch);
	}
}

void C_TextureRenderer::Render(const C_Transform2D* _transform) const
{
	if (_transform == nullptr)
	{
#ifdef DEBUG
		printf("%s(%u) ERROR : Trying to render a Texture from a component but the pointer to the Transform Component was nullptr.", __FILE__, __LINE__ - 5);
#endif // DEBUG

		return;
	}

	const Engine* engine = Engine::GetInstance();
	Tigr* window = engine->GetTigrScreen();

	if (m_visible == false || m_pImage == nullptr || window == nullptr)
	{
		return;
	}

	tigrBlitTint(window, m_pImage, _transform->m_pos.x, _transform->m_pos.y,
	 	m_u0 * m_uWidth, m_v0 * m_uHeight, (m_u1 - m_u0) * m_uWidth, (m_v1 - m_v0) * m_uHeight,
		TPixel(m_tintColor.x, m_tintColor.y, m_tintColor.z, m_tintColor.w));
}

void C_TextureRenderer::Render(const C_Transform2D_PlusParenting* _transform) const
{
	if (_transform == nullptr)
	{
#ifdef DEBUG
		printf("%s(%u) ERROR : Trying to render a Texture from a component but the pointer to the Transform Component was nullptr.", __FILE__, __LINE__ - 5);
#endif // DEBUG

		return;
	}

	const Engine* engine = Engine::GetInstance();
	Tigr* window = engine->GetTigrScreen();

	if (m_visible == false || m_pImage == nullptr || window == nullptr)
	{
		return;
	}

	vec2 worldPosition = _transform->GetWorldPosition();

	tigrBlitTint(window, m_pImage, worldPosition.x, worldPosition.y,
		m_u0 * m_uWidth, m_v0 * m_uHeight, (m_u1 - m_u0) * m_uWidth, (m_v1 - m_v0) * m_uHeight,
		TPixel(m_tintColor.x / 255.0f, m_tintColor.y / 255.0f, m_tintColor.z / 255.0f, m_tintColor.w / 255.0f));
}

void C_TextureRenderer::SetTintColor(const float _r, const float _g, const float _b, const float _a)
{
	m_tintColor = Color(_r, _g, _b, _a);
}

void C_TextureRenderer::SetDrawingPositionInTexture(float _u0, float _u1, float _v0, float _v1)
{
	m_u0 = _u0;
	m_u1 = _u1;
	m_v0 = _v0;
	m_v1 = _v1;
}

C_TextureRenderer::C_TextureRenderer(const std::string& _ImagePath)
{
	SetImageAsset(_ImagePath);
}

C_TextureRenderer::~C_TextureRenderer()
{
	if (m_pImage != nullptr)
	{
		tigrFree(m_pImage);
		m_pImage = nullptr;
		TextureFilePath.clear();
	}
}

bool C_TextureRenderer::Serialize(pugi::xml_node* _ComponentNode)
{
	if (_ComponentNode == nullptr || _ComponentNode->empty())
	{
		return false;
	}

	pugi::xml_node visibleNode = _ComponentNode->append_child("Visible");
	XML_UTIL::SaveToXMLNode(m_visible, visibleNode);

	pugi::xml_node filePathNode = _ComponentNode->append_child("TextureFilePath");
	XML_UTIL::SaveToXMLNode(TextureFilePath, filePathNode);

	pugi::xml_node tintColorNode = _ComponentNode->append_child("TintColor");
	XML_UTIL::SaveToXMLNode(m_tintColor, tintColorNode);

	pugi::xml_node coordinatesNode = _ComponentNode->append_child("TextureCoordinates");

	pugi::xml_node u0CoordinatesNode = coordinatesNode.append_child("u0");
	XML_UTIL::SaveToXMLNode(m_u0, u0CoordinatesNode);

	pugi::xml_node u1CoordinatesNode = coordinatesNode.append_child("u1");
	XML_UTIL::SaveToXMLNode(m_u1, u1CoordinatesNode);

	pugi::xml_node v0CoordinatesNode = coordinatesNode.append_child("v0");
	XML_UTIL::SaveToXMLNode(m_v0, v0CoordinatesNode);

	pugi::xml_node v1CoordinatesNode = coordinatesNode.append_child("v1");
	XML_UTIL::SaveToXMLNode(m_v1, v1CoordinatesNode);

	return true;
}

bool C_TextureRenderer::Load(const pugi::xml_node* _ComponentNode)
{
	if (_ComponentNode == nullptr || _ComponentNode->empty())
	{
		return false;
	}

	bool hadFailedLoads = false;

	pugi::xml_node visibleNode = _ComponentNode->child("Visible");
	if (!visibleNode.empty())
	{
		XML_UTIL::LoadXMLNodeToVariable(m_visible, visibleNode);
	}
	else
	{
		hadFailedLoads = true;
	}

	pugi::xml_node filePathNode = _ComponentNode->child("TextureFilePath");
	if (!filePathNode.empty())
	{
		XML_UTIL::LoadXMLNodeToVariable(TextureFilePath, filePathNode);
	}
	else
	{
		hadFailedLoads = true;
	}

	pugi::xml_node tintColorNode = _ComponentNode->child("TintColor");
	if (!tintColorNode.empty())
	{
		XML_UTIL::LoadXMLNodeToVariable(m_tintColor, tintColorNode);
	}
	else
	{
		hadFailedLoads = true;
	}

	pugi::xml_node coordinatesNode = _ComponentNode->child("TextureCoordinates");

	pugi::xml_node u0CoordinatesNode = coordinatesNode.child("u0");
	if (!u0CoordinatesNode.empty())
	{
		XML_UTIL::LoadXMLNodeToVariable(m_u0, u0CoordinatesNode);
	}
	else
	{
		hadFailedLoads = true;
	}

	pugi::xml_node u1CoordinatesNode = coordinatesNode.child("u1");
	if (!u1CoordinatesNode.empty())
	{
		XML_UTIL::LoadXMLNodeToVariable(m_u1, u1CoordinatesNode);
	}
	else
	{
		hadFailedLoads = true;
	}

	pugi::xml_node v0CoordinatesNode = coordinatesNode.child("v0");
	if (!v0CoordinatesNode.empty())
	{
		XML_UTIL::LoadXMLNodeToVariable(m_v0, v0CoordinatesNode);
	}
	else
	{
		hadFailedLoads = true;
	}

	pugi::xml_node v1CoordinatesNode = coordinatesNode.child("v1");
	if (!v1CoordinatesNode.empty())
	{
		XML_UTIL::LoadXMLNodeToVariable(m_v1, v1CoordinatesNode);
	}
	else
	{
		hadFailedLoads = true;
	}

	if (!TextureFilePath.empty())
	{
		SetImageAsset(TextureFilePath);
	}

	return !hadFailedLoads;
}
