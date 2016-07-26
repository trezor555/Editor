#ifndef ISHADER_H
#define ISHADER_H
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
class IShader
{
public:
	virtual void SetWorldMatrix(glm::mat4& worldMatrix) {};
};

#endif 