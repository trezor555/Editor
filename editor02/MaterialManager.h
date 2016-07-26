#ifndef MATERIAL_MANAGER_H
#define MATERIAL_MANAGER_H
#include "Material.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <map>
class MaterialManager
{
public:
	static Material* Create(const char* MaterialName);
private:
	

	glm::mat4 world;
	glm::mat4 view;
	glm::mat4 projection;
};

#endif