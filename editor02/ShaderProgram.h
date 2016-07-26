#pragma once

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "Shader.h"
class ShaderProgram
{
public:
	ShaderProgram(char* vsShaderFileName, char* psShaderFileName);
	void Setup();
	void SetUniform_mat4(char* name, glm::mat4&  mat);
	void SetUniform_float4(char* name, glm::vec4&  vec);
	void SetUniform_float3(char* name, glm::vec3&  vec);
	void SetUniform_float2(char* name, glm::vec2&  vec);
	void SetUniform_float1(char* name, float&  val);
	void SetUniform_int1(char* name, int&  val);
	virtual ~ShaderProgram();

private:
	
};

