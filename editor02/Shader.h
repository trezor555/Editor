#ifndef SHADER_H
#define SHADER_H
#define GLEW_STATIC

#include <fstream>
#include <iostream>
#include <GL/glew.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <vector>

class Shader
{
public:
	~Shader();
	bool CreateShader(char* vertexShaderPath, char* fragmentShaderPath);
	void ReflectShader();
	GLuint GetShaderProgram();
	void Begin();
	void End();
	void SetUniform_Matrix(std::string name, glm::mat4&  mat);
	void SetUniform_Matrix(GLuint location, glm::mat4&  mat);

	void SetUniform_float4(std::string name, glm::vec4&  vec);
	void SetUniform_float3(std::string name, glm::vec3&  vec);
	void SetUniform_float3(GLuint location, glm::vec3&  vec);
	void SetUniform_1i(GLuint location, unsigned int val);
	void SetUniform_1i(std::string name, unsigned int val);
private:
	bool LoadShader(char* fileName, GLuint& shaderID, GLenum shaderType);
	bool LinkShaders(GLuint& vertexShader, GLuint& fragmentShader);
	struct ShaderVar
	{
		GLenum varType;
		std::string varName;
		GLuint index;
	};
	std::vector<ShaderVar> uniformVars;
	GLuint m_ShaderProgram;

};

#endif