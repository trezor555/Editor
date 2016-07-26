#include "Shader.h"

bool Shader::CreateShader(char* vertexShaderPath, char* fragmentShaderPath)
{
	GLuint VertexShader;
	GLuint FragmentShader;

	if (!LoadShader(vertexShaderPath, VertexShader, GL_VERTEX_SHADER))
		return false;
	if (!LoadShader(fragmentShaderPath, FragmentShader, GL_FRAGMENT_SHADER))
		return false;

	LinkShaders(VertexShader, FragmentShader);
	//ReflectShader();
	return true;
}

bool Shader::LoadShader(char* fileName, GLuint& shaderID, GLenum shaderType)
{
	std::streampos size;
	char * memblock;
	std::string fullPath = "../Data/Shaders/";
	fullPath += fileName;
	std::ifstream file(fullPath, std::ios::in | std::ios::binary | std::ios::ate);
	if (file.is_open())
	{
		size = file.tellg();
		memblock = new char[size];
		file.seekg(0, std::ios::beg);
		file.read(memblock, size);
		file.close();

		/*if (fileName  == "basic.ps")
		{
		char* sources[2] = { "#define YELLOW" , memblock, };
		const GLint shaderLength[2] = { 14, static_cast<GLint>(size) };
		shaderID = glCreateShader(shaderType);
		glShaderSource(shaderID, 2, sources, shaderLength);
		}
		else
		{*/
		char* sources = { memblock };
		const GLint shaderLength = static_cast<GLint>(size);
		shaderID = glCreateShader(shaderType);
		glShaderSource(shaderID, 1, &sources, &shaderLength);
		//}

		glCompileShader(shaderID);
		// Check for compile time errors
		GLint success;
		GLchar infoLog[512];

		glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shaderID, 512, NULL, infoLog);
			std::cout << fileName << ":" << std::endl;
			std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
		}

		delete[] memblock;

		return true;
	}


	std::cout << "File not found: " << fileName << std::endl;
	return false;
}


GLuint Shader::GetShaderProgram()
{
	return m_ShaderProgram;
}

bool Shader::LinkShaders(GLuint& vertexShader, GLuint& fragmentShader)
{
	GLint success;
	GLchar infoLog[512];
	// Link shaders
	m_ShaderProgram = glCreateProgram();
	glAttachShader(m_ShaderProgram, vertexShader);
	glAttachShader(m_ShaderProgram, fragmentShader);
	glLinkProgram(m_ShaderProgram);
	// Check for linking errors
	glGetProgramiv(m_ShaderProgram, GL_LINK_STATUS, &success);

	if (!success)
	{
		glGetProgramInfoLog(m_ShaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;

	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return true;
}

void Shader::Begin()
{
	glUseProgram(m_ShaderProgram);
}

void Shader::End()
{
	glUseProgram(0);

}

Shader::~Shader()
{
	glDeleteProgram(m_ShaderProgram);
}

void Shader::SetUniform_Matrix(std::string name, glm::mat4& mat)
{
	glUniformMatrix4fv(glGetUniformLocation(m_ShaderProgram, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));

}

void Shader::SetUniform_Matrix(GLuint location, glm::mat4& mat)
{
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::SetUniform_float4(std::string name, glm::vec4& vec)
{
	glUniform4f(glGetUniformLocation(m_ShaderProgram, name.c_str()), vec.x, vec.y, vec.z, vec.w);
}

void Shader::SetUniform_float3(std::string name, glm::vec3& vec)
{
	glUniform3f(glGetUniformLocation(m_ShaderProgram, name.c_str()), vec.x, vec.y, vec.z);
}

void Shader::SetUniform_float3(GLuint location, glm::vec3& vec)
{
	glUniform3f(location, vec.x, vec.y, vec.z);
}

void Shader::ReflectShader()
{
	GLint numBlocks;
	glGetProgramiv(m_ShaderProgram, GL_ACTIVE_UNIFORMS, &numBlocks);

	GLint maxLength;
	glGetProgramiv(m_ShaderProgram, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLength);

	for (int i = 0; i < numBlocks; ++i)
	{
		ShaderVar var;
		var.varName.resize(maxLength);
		var.index = i;
		GLint length;
		var.varName.resize(maxLength);
		glGetActiveUniform(m_ShaderProgram, i, maxLength, &length, NULL, &var.varType, &var.varName[0]);
		uniformVars.push_back(var);
	}
}

void Shader::SetUniform_1i(GLuint location, unsigned int val)
{
	glUniform1i(location, val);
}

void Shader::SetUniform_1i(std::string name, unsigned int val)
{
	glUniform1i(glGetUniformLocation(m_ShaderProgram, name.c_str()), val);
}
