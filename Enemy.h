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

class Enemy {
public:
	Enemy();
	Enemy(shaderType _enemyType, glm::vec3 startPoint);
	~Enemy();

	void setHealth(float _health);
	
	float getHealth();
	const BoundingBox* getBox();

	void draw(glm::mat4 modelView, ShaderManager *shader, ply *myPly);
	
	void moveEnemy(glm::vec3 playerPos);

	void applyHit(t_hitfunc);

	
	shaderType enemyType;
	int pointValue;

	static bool debug_draw_hitbox;
private:
	float health, speed, initialAngle, currAngle, angularSpeed, modelSize;
	glm::vec3 position, lookVector, scaleSize;

	BoundingBox *box;

	glm::mat4 transMat4;
};
#endif