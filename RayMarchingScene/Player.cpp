#include "Player.h"

Player::Player(float s, float headHeight)
{
	sensitivity = s;
	height = headHeight;
	position = glm::vec3(0);
	headHeightBoost = glm::vec3(0, height, 0);
}

void Player::Move(float x, float y, float z)
{
	//change movement from world to player space vector
	glm::mat4 t = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
	glm::mat4 r = glm::rotate(glm::mat4(1.0), glm::radians(Camera::activeCam->GetEulerAngles().y), glm::vec3(0, 1, 0));
	glm::vec4 move = r * t * glm::vec4(x, y, z, 1.0);
	position += glm::vec3(move.x, move.y, move.z);
}

void Player::Move(glm::vec3 p)
{
	glm::mat4 t = glm::translate(glm::mat4(1.0f), p);
	glm::mat4 r = glm::rotate(glm::mat4(1.0), glm::radians(Camera::activeCam->GetEulerAngles().y), glm::vec3(0, 1, 0));
	glm::vec4 move = r * t * glm::vec4(p, 1.0);
	position += glm::vec3(move.x, move.y, move.z);
}

void Player::Update(float deltaTime)
{
	Move(glm::vec3(Input::sideways, 0, Input::forward) * deltaTime);   //moves player along local axes
	Camera::activeCam->SetPosition(position + headHeightBoost);
	Camera::activeCam->SetEulerAngles(Camera::activeCam->GetEulerAngles() + glm::vec3(Input::rawMouseY, Input::rawMouseX, 0) * sensitivity * deltaTime);

	if (Input::mouseLeftClick && Cursor::visible)
		Cursor::SetVisible(false);
}
