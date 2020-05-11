#include "Player.h"

#define MANA_CHARGE 0.01
#define HEAL_PER_TICK 0.01
#define PLAYERSIZE 0.15f
#define PLAYERSPEED 0.02f

Player::Player() {
	glm::vec3 startPoint(0.0, HEIGHT, 0.0);
	glm::vec3 startLook(1.0, 0.0, 0.0);
	glm::vec3 upVec(0.0, 1.0, 0.0);

	myCam = new Camera();
	myCam->orientLookAt(startPoint, startLook, upVec);
	
	health = 100.0;
	points = 0;
	mana = 10.0;
	speed = PLAYERSPEED;
	canMoveSight = false;
	maxMana = 20.0;
	maxHealth = 100;
	maxPoints = 10;
	spellSelected = FIREBALL;
	box = new BoundingBox(glm::vec4(myCam->getEyePoint(), 1.0), PLAYERSIZE);
	iFrames = 0;
}

Player::~Player() {
	delete myCam;
	delete box;
}

void Player::deciFrames()
{
	this->iFrames = max(0, --iFrames);
}

void Player::setiFrames(int iFrames)
{
	this->iFrames = iFrames;
}

void Player::moveForward() {
	glm::vec3 eyeP = myCam->getEyePoint();
	glm::vec3 lookV = myCam->getLookVector();
	float distance;

	eyeP.x += speed * lookV.x;
	eyeP.z += speed * lookV.z;

	// Bounds player in arena
	distance = sqrt(eyeP.x * eyeP.x + eyeP.z * eyeP.z);
	if (distance < ARENA_SIZE) {
		myCam->setEyePoint(eyeP);
		box->setCenter(glm::vec4(eyeP, 1.0f));
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
	if (distance < ARENA_SIZE) {
		myCam->setEyePoint(eyeP);
		box->setCenter(glm::vec4(eyeP, 1.0f));
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
	if (distance < ARENA_SIZE) {
		myCam->setEyePoint(eyeP);
		box->setCenter(glm::vec4(eyeP, 1.0f));
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
	if (distance < ARENA_SIZE) {
		myCam->setEyePoint(eyeP);
		box->setCenter(glm::vec4(eyeP, 1.0f));
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
void Player::changeHealth(float _health) {
	float temp = health + _health;
	if (temp <= maxHealth) {
		health += _health;
	}
}


void Player::changePoints(int _points) {
	int temp = points + _points;
	if (temp <= maxPoints) {
		points = temp;
	} else {
		points = 0;
		maxPoints *= 2;
	}
}


void Player::chargeMana() {
	float temp = mana + MANA_CHARGE;
	if (temp <= maxMana)
		mana = temp;
}

void Player::tickHeal() {
	health = min(maxHealth, health + HEAL_PER_TICK);
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

void Player::changeMana(int _mana) {
	float temp = mana + _mana;
	if (temp <= maxMana)
		mana = temp;
	else if (temp < 0) {
		mana = 0;
	}
}

float Player::getSpellCost(shaderType spellType) {
	switch (spellType) {
	case(FIREBALL):
		return 2.0;
		break;
	default:
		printf("You don't know that spell!\n");
		return INFINITY;
	}
}

const BoundingBox* Player::getBox()
{
	return box;
}

int Player::getiFrames()
{
	return iFrames;
}

bool Player::isInvincible() {
	return iFrames > 0;
}

bool Player::isDead() {
	return health <= 0;
}

void Player::applyHit(t_hitfunc f) {
	HITDATA data(health, mana, speed);

	f(data);

	health = min(data.health, maxHealth);
	mana = min(max(data.mana, 0), maxMana);
	speed = data.speed;
}