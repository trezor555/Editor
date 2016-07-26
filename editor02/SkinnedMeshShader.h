#ifndef SKINNED_MESH_SHADER_H
#define SKINNED_MESH_SHADER_H
#define GLEW_STATIC
#include <GL/glew.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <string>
#include "Shader.h"
class SkinnedMeshShader
{
public:
	void Init();
	void SetWVP(const glm::mat4& WVP);
	void SetWorldMatrix(const glm::mat4& worldM);
	void SetBoneTransform(unsigned int Index, const glm::mat4& Transform);
	void Set();
private:
#define MAX_BONES 100
	GLuint gWVPLoc;
	GLuint gWorldLoc;
	GLuint diffuseTexLoc;
	GLuint gBonesLoc[MAX_BONES];
	Shader shader;
};

#endif