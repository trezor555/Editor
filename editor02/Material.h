#ifndef MATERIAL_H
#define MATERIAL_H
#include "Shader.h"

struct ShaderVariableData 
{
	ShaderVariableData()
	{
		VarType = "";
		DefaultValue = "";
	}
	std::string VarType;
	std::string DefaultValue;
};

class Material
{
public:
	Material();
	Shader* shader;
};

#endif