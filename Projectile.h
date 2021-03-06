#ifndef PROJECTILE_H
#define PROJECTILE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include "glm/gtx/string_cast.hpp"
#include "glm/ext.hpp"

#include "gamedefs.h"
#include "BoundingBox.h"
#include "ShaderManager.h"
#include "ply.h"
#include "gfxDefs.h"
#include <irrKlang/irrKlang.h>

using namespace irrklang;

class Projectile {
public:
	Projectile();
	Projectile(shaderType _type, glm::vec3 startPos, glm::vec3 directionFired, ISoundEngine *engine);
	~Projectile();

	time_t getSpawnTime();
	float  getDuration();
	t_hitfunc getHitfunc();
	shaderType getType();
	const BoundingBox* getBox();
	
	
	bool hitFloor();

	void draw(ShaderManager *shader, ply *myPly);

	void moveProjectile();

	shaderType projectileType;
	static bool debug_draw_hitbox;

private:

	float duration, speed, currAngleYaw, currAnglePitch, initialAngleYaw, initialAnglePitch, modelSize;

	glm::vec3 position, dirFired, orientation, scaleSize;
	glm::mat4 transMat4;
	time_t spawnTime;
	t_hitfunc onHit;
	BoundingBox *box;

	void makeFireball(shaderType _type, glm::vec3 startPos, glm::vec3 directionFired);
};
#endif