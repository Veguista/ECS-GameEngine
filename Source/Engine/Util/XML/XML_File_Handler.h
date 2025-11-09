#pragma once

#include "Engine/ExternalLibraries/Pugixml/pugixml.hpp"
#include "Engine/DataTypes/Vectors/vector2d.h"
#include "Engine/DataTypes/Vectors/vector3d.h"
#include "Engine/DataTypes/Vectors/vector4d.h"
#include "Engine/DataTypes/Vectors/color.h"
#include <string>

namespace XML_UTIL
{
	static inline pugi::xml_parse_result ReadXMLFile(const std::string& _FilePath, pugi::xml_document& _BufferDocument)
	{
		return _BufferDocument.load_file(_FilePath.c_str());
	}

	static inline bool SaveXMLFile(const std::string& _FilePath, pugi::xml_document& _Document)
	{
		return _Document.save_file(_FilePath.c_str());
	}

	template<typename T>
	static void SaveToXMLNode(const T& _VarToSave, pugi::xml_node& _Component)
	{
		_Component.append_attribute("Value").set_value(std::to_string(_VarToSave));
	}
	template<>
	static void SaveToXMLNode(const bool& _BoolToSave, pugi::xml_node& _Component)
	{
		_Component.append_attribute("Value").set_value(_BoolToSave ? "True" : "False");
	}
	template<>
	static void SaveToXMLNode(const vec2& _VectorToSave, pugi::xml_node& _Component)
	{
		_Component.append_attribute("X").set_value(std::to_string(_VectorToSave.x));
		_Component.append_attribute("Y").set_value(std::to_string(_VectorToSave.y));
	}
	template<>
	static void SaveToXMLNode(const vec3& _VectorToSave, pugi::xml_node& _Component)
	{
		_Component.append_attribute("X").set_value(std::to_string(_VectorToSave.x));
		_Component.append_attribute("Y").set_value(std::to_string(_VectorToSave.y));
		_Component.append_attribute("Z").set_value(std::to_string(_VectorToSave.z));
	}
	template<>
	static void SaveToXMLNode(const vec4& _VectorToSave, pugi::xml_node& _Component)
	{
		_Component.append_attribute("X").set_value(std::to_string(_VectorToSave.x));
		_Component.append_attribute("Y").set_value(std::to_string(_VectorToSave.y));
		_Component.append_attribute("Z").set_value(std::to_string(_VectorToSave.z));
		_Component.append_attribute("W").set_value(std::to_string(_VectorToSave.w));
	}
	template<>
	static void SaveToXMLNode(const Color& _ColorToSave, pugi::xml_node& _Component)
	{
		_Component.append_attribute("R").set_value(std::to_string(_ColorToSave.r));
		_Component.append_attribute("G").set_value(std::to_string(_ColorToSave.g));
		_Component.append_attribute("B").set_value(std::to_string(_ColorToSave.b));
		_Component.append_attribute("A").set_value(std::to_string(_ColorToSave.a));
	}
	template<>
	static void SaveToXMLNode(const std::string& _StringToSave, pugi::xml_node& _Component)
	{
		_Component.append_attribute("Value").set_value(_StringToSave.c_str());
	}
	static void SaveToXMLNode(const char* _StringToSave, pugi::xml_node& _Component)
	{
		if (_StringToSave != nullptr)
		{
			_Component.append_attribute("Value").set_value(_StringToSave);
		}
		else
		{
			_Component.append_attribute("Value").set_value("");
		}
	}

	template<typename T>
	static void LoadXMLNodeToVariable(T& _VariableToFill, const pugi::xml_node& _Component)
	{
		_VariableToFill = static_cast<T>(std::stof(_Component.attribute("Value").value()));
	}
	template<>
	static void LoadXMLNodeToVariable(bool& _BoolToFill, const pugi::xml_node& _Component)
	{
		_BoolToFill = (_Component.attribute("Value").value() == std::string("True"));
	}
	template<>
	static void LoadXMLNodeToVariable(vec2& _VectorToFill, const pugi::xml_node& _Component)
	{
		_VectorToFill = vec2(
			std::stof(_Component.attribute("X").value()),
			std::stof(_Component.attribute("Y").value()));
	}
	template<>
	static void LoadXMLNodeToVariable(vec3& _VectorToFill, const pugi::xml_node& _Component)
	{
		_VectorToFill = vec3(
			std::stof(_Component.attribute("X").value()),
			std::stof(_Component.attribute("Y").value()),
			std::stof(_Component.attribute("Z").value()));
	}
	template<>
	static void LoadXMLNodeToVariable(vec4& _VectorToFill, const pugi::xml_node& _Component)
	{
		_VectorToFill = vec4(
			std::stof(_Component.attribute("X").value()),
			std::stof(_Component.attribute("Y").value()),
			std::stof(_Component.attribute("Z").value()),
			std::stof(_Component.attribute("W").value()));
	}
	template<>
	static void LoadXMLNodeToVariable(Color& _ColorToFill, const pugi::xml_node& _Component)
	{
		_ColorToFill = Color(
			std::stof(_Component.attribute("R").value()),
			std::stof(_Component.attribute("G").value()),
			std::stof(_Component.attribute("B").value()),
			std::stof(_Component.attribute("A").value()));
	}
	template<>
	static void LoadXMLNodeToVariable(std::string& _StringToFill, const pugi::xml_node& _Component)
	{
		_StringToFill = _Component.attribute("Value").value();
	}
};