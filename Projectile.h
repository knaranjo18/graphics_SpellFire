#ifndef PROJECTILE_H
#define PROJECTILE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include "glm/gtx/string_cast.hpp"
#include "glm/ext.hpp"

#include "BoundingBox.h"
#include "ShaderManager.h"
#include "ply.h"
#include "gfxDefs.h"

class Projectile {
public:
	Projectile();
	Projectile(shaderType _type, glm::vec3 startPos, glm::vec3 directionFired);
	~Projectile();

	void draw(glm::mat4 modelView, ShaderManager *shader, ply *myPly);

	void moveProjectile();

	shaderType projectileType;
	static bool debug_draw_hitbox;
private:
	float duration, speed, manaCost, currAngleYaw, currAnglePitch, initialAngleYaw, initialAnglePitch, modelSize;
	glm::vec3 position, dirFired, orientation, scaleSize;
	glm::mat4 transMat4;

	BoundingBox *box;

};
#endif