#ifndef ENEMY_H
#define ENEMY_H


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include "glm/gtx/string_cast.hpp"
#include "glm/ext.hpp"

#include "BoundingBox.h"
#include "ShaderManager.h"
#include "ply.h"
#include "gfxDefs.h"
#include "gamedefs.h"
#include <irrKlang/irrKlang.h>

using namespace irrklang;

class Enemy {
public:
	Enemy();
	Enemy(shaderType _enemyType, glm::vec3 startPoint, ISoundEngine *engine);
	~Enemy();

	void setHealth(float _health);
	
	float getHealth();
	glm::vec3 getPosition();
	shaderType getType();
	const BoundingBox* getBox();

	void draw(ShaderManager *shader, ply *myPly);
	
	void moveEnemy(glm::vec3 playerPos);

	void applyHit(t_hitfunc);
	void callSound();
	void deathSound();

	bool isDead();

	t_hitfunc getHitFunc();
	
	shaderType enemyType;
	int pointValue;

	static bool debug_draw_hitbox;
private:
	float health, speed, initialAngle, currAngle, angularSpeed, modelSize;
	glm::vec3 position, lookVector, scaleSize;

	BoundingBox *box;

	t_hitfunc onHit;
	ISoundEngine *soundEngine;

	glm::mat4 transMat4;
};
#endif