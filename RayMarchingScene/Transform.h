#pragma once
#ifndef Transform_H
#define Transform_H
#include <cmath>
#include <glm\glm\glm.hpp>
#include <glm\glm\gtc\type_ptr.hpp>
#include <glm\glm\gtc\matrix_transform.hpp>

class Transform
{
public:
	//Constructor
	Transform();
	~Transform();

	//Methods
	virtual void SetPosition(float x, float y, float z);
	virtual void SetPosition(glm::vec3 pos);

	virtual void SetEulerAngles(float x, float y, float z, bool aroundOrigin = false);
	virtual void SetEulerAngles(glm::vec3 rot, bool aroundOrigin = false);

	glm::vec3 GetPosition();
	glm::vec3 GetEulerAngles();


	//Properties
	glm::mat4 tMat;
	glm::mat4 rMat;
	glm::mat4 mMat;
	glm::mat4 mvMat;
protected:
	glm::vec3 position;
	glm::vec3 eulerAngles;

	virtual void UpdateMatrices(bool aroundOrigin = false);
};

#endif