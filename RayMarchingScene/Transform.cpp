#include "Transform.h"
#include "Camera.h"
#include <cmath>
#include <glm\glm\glm.hpp>
#include <glm\glm\gtc\type_ptr.hpp>
#include <glm\glm\gtc\matrix_transform.hpp>

Transform::Transform() {
	position = glm::vec3(0.0f);
	eulerAngles = glm::vec3(0.0f);
	tMat = glm::mat4(1.0f);
	rMat = glm::mat4(1.0f);
	mMat = glm::mat4(1.0f);
	mvMat = glm::mat4(1.0f);
}

void Transform::SetPosition(float x, float y, float z) {
	position = glm::vec3(x, y, z);
	UpdateMatrices();
}

void Transform::SetPosition(glm::vec3 pos) {
	position = pos;
	UpdateMatrices();
}

void Transform::SetEulerAngles(float x, float y, float z, bool aroundOrigin) {
	eulerAngles = glm::vec3(x, y, z);
	UpdateMatrices(aroundOrigin);
}

void Transform::SetEulerAngles(glm::vec3 rot, bool aroundOrigin) {
	eulerAngles = rot;
	UpdateMatrices(aroundOrigin);
}

void Transform::UpdateMatrices(bool aroundOrigin) {
	rMat = glm::rotate(glm::mat4(1.0f), glm::radians(eulerAngles.x), glm::vec3(1, 0, 0));
	rMat *= glm::rotate(glm::mat4(1.0f), glm::radians(eulerAngles.y), glm::vec3(0, 1, 0));
	rMat *= glm::rotate(glm::mat4(1.0f), glm::radians(eulerAngles.z), glm::vec3(0, 0, 1));

	tMat = glm::translate(glm::mat4(1.0f), position);

	//rotation matrices are always around world space origin
	if (aroundOrigin)
		mMat = rMat * tMat;   //translate along global axes and then rotate
	else
		mMat = tMat * rMat;  //rotate then translate along local axes

	mvMat = Camera::activeCam->vMat * mMat;  //view matrix is from the static active camera
}

glm::vec3 Transform::GetPosition()
{
	return position;
}

glm::vec3 Transform::GetEulerAngles()
{
	return eulerAngles;
}

Transform::~Transform() {
}