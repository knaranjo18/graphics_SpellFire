#include "Player.h"

Player::Player() {
	glm::vec3 startPoint(0.0, -0.2, 0.0);
	glm::vec3 startLook(1.0, 0.0, 0.0);
	glm::vec3 upVec(0.0, 1.0, 0.0);

	myCam = new Camera();
	myCam->orientLookAt(startPoint, startLook, upVec);
	
	health = 100.0;
	points = 0;
	mana = 10.0;
	speed = 0.05;
	canMoveSight = false;
}

Player::~Player() {
	delete myCam;
}


void Player::moveForward() {
	glm::vec3 eyeP = myCam->getEyePoint();
	glm::vec3 lookV = myCam->getLookVector();
	float distance;

	eyeP.x += speed * lookV.x;
	eyeP.z += speed * lookV.z;

	// Bounds player in arena
	distance = sqrt(eyeP.x * eyeP.x + eyeP.z * eyeP.z);
	if (distance < 1.9) {
		myCam->setEyePoint(eyeP);
	}

}


void Player::moveBackward() {
	glm::vec3 eyeP = myCam->getEyePoint();
	glm::vec3 lookV = myCam->getLookVector();
	float distance;

	eyeP.x -= speed * lookV.x;
	eyeP.z -= speed * lookV.z;

	// Bounds player in arena
    distance = sqrt(eyeP.x * eyeP.x + eyeP.z * eyeP.z);
	if (distance < 1.9) {
		myCam->setEyePoint(eyeP);
	}
}


void Player::moveLeft() {
	glm::vec3 eyeP = myCam->getEyePoint();
	glm::vec3 lookV = myCam->getLookVector();
	glm::vec3 upV = myCam->getUpVector();
	glm::vec3 tempV = glm::normalize(glm::cross(lookV, upV));
	float distance;

	eyeP.x -= speed * tempV.x;
	eyeP.z -= speed * tempV.z;

	// Bounds player in arena
	distance = sqrt(eyeP.x * eyeP.x + eyeP.z * eyeP.z);
	if (distance < 1.9) {
		myCam->setEyePoint(eyeP);
	}
}


void Player::moveRight() {
	glm::vec3 eyeP = myCam->getEyePoint();
	glm::vec3 lookV = myCam->getLookVector();
	glm::vec3 upV = myCam->getUpVector();
	glm::vec3 tempV = glm::normalize(glm::cross(lookV, upV));
	float distance;

	eyeP.x += speed * tempV.x;
	eyeP.z += speed * tempV.z;

	// Bounds player in arena
	distance = sqrt(eyeP.x * eyeP.x + eyeP.z * eyeP.z);
	if (distance < 1.9) {
		myCam->setEyePoint(eyeP);
	}

}

void Player::moveSight(int x_offset, int y_offset) {
	yaw += x_offset;
	pitch += y_offset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	else if (pitch < -89.0f)
		pitch = -89.0f;

	myCam->rotateView(yaw, pitch);
}


/*---------------Setters------------------*/
void Player::setHealth(float _health) {
	health = _health;
}


void Player::setPoints(int _points) {
	points = _points;
}


void Player::setMana(float _mana) {
	mana = _mana;
}


/*------------Getters--------------------*/
float Player::getHealth() {
	return health;
}


int Player::getPoints() {
	return points;
}


float Player::getMana() {
	return mana;
}


