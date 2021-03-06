#ifndef PLAYER_H
#define PLAYER_H

#include "BoundingBox.h"
#include "Camera.h"
#include "ShaderManager.h"
#include "ply.h"
#include "gamedefs.h"
#include <irrKlang/irrKlang.h>

using namespace irrklang;


class Player {
public:
	Player(ISoundEngine *engine);
	~Player();

	void restartPlayer();

	void changeHealth(float _health);
	void changePoints(int _points);
	void changeMana(int _mana);

	void chargeMana();
	void tickHeal();

	float getHealth();
	int getPoints();
	float getMana();
	float getSpellCost(shaderType spellType);
	glm::vec3 getPosition();
	glm::vec3 getLookVec();
	const BoundingBox* getBox();
	int getiFrames();

	bool isInvincible();
	bool isDead();

	void deciFrames();
	void setiFrames(int);
	void moveForward();
	void moveBackward();
	void moveLeft();
	void moveRight();
	void moveSight(int x_offset, int y_offset);

	void applyHit(t_hitfunc);
	void hurtSound();

	Camera *myCam;
	shaderType spellSelected;
	float maxMana, maxHealth, maxPoints;
private:
	int points;
	int iFrames; // invincibility frames after getting hit
	float speed, mana, health, yaw, pitch;

	BoundingBox* box;
	ISoundEngine *soundEngine;
};
#endif