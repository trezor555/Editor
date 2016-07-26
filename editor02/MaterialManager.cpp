#include "MaterialManager.h"
#include "tinyxml.h"
#include <string>
Material* MaterialManager::Create(const char* MaterialFile)
{
	std::string MaterialName;
	std::string VertexShaderFile;
	std::string PixelShaderFile;
	std::map<std::string, ShaderVariableData> shaderData;

	std::string file = "../Data/Materials/" + std::string(MaterialFile);
	
	TiXmlDocument doc;
	bool err = doc.LoadFile(file.c_str());
	if (err == false)
	{
		return nullptr;
	}

	TiXmlElement* root = doc.FirstChildElement();
	MaterialName = root->Attribute("name");

	for (TiXmlElement* elem = root->FirstChildElement(); elem != nullptr; elem = elem->NextSiblingElement())
	{
		std::string currentNode(elem->Value());
		
		if (currentNode == "ShaderFiles")
		{
			TiXmlElement* shaderElement = elem->FirstChildElement();
			
			std::string shaderType(shaderElement->Value());
			if (shaderType == "VertexShader")
			{
				VertexShaderFile = shaderElement->Attribute("FileName");
			}
			shaderType = shaderElement->NextSiblingElement()->Value();
			if (shaderType == "PixelShader")
			{
				PixelShaderFile = shaderElement->NextSiblingElement()->Attribute("FileName");
			}
			
		}

		if (currentNode == "UniformVariables")
		{
			for (TiXmlElement* variables = elem->FirstChildElement(); variables != nullptr; variables = variables->NextSiblingElement())
			{
				ShaderVariableData st;
				st.VarType = variables->Attribute("Type");
				std::string VarName = variables->Attribute("Name");
				if (variables->Attribute("DefaultValue") != nullptr)
					st.DefaultValue = variables->Attribute("DefaultValue");
				
				shaderData.insert(std::pair<std::string, ShaderVariableData>(VarName, st));
			}

		}
	}
	return nullptr;

}
