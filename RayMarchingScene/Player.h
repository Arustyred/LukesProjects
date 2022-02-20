#pragma once
#ifndef Player_H
#define Player_H
#include "Camera.H"
#include "Cursor.H"
#include "Input.H"
#include <glm\glm\glm.hpp>

class Player
{
public:
	Player(float s, float headHeight);

	glm::vec3 position;

	void Move(float x, float y, float z);
	void Move(glm::vec3 p);
	void Update(float deltaTime);

	float sensitivity;

protected:
	float height;  //head height
	glm::vec3 headHeightBoost;
};

#endif