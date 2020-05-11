#ifndef PICKUP_H
#define PICKUP_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include "glm/gtx/string_cast.hpp"
#include "glm/ext.hpp"

#include "ShaderManager.h"
#include "ply.h"
#include "gfxDefs.h"
#include "BoundingBox.h"
#include "gamedefs.h"


class Pickup {
public:
	Pickup();
	Pickup(glm::vec3, float, shaderType);
	~Pickup();

	void draw(glm::mat4, ShaderManager*, ply*);

	const BoundingBox* getBox();
	shaderType getType();
	time_t getSpawnTime();
	float getDuration();

	t_hitfunc getHitFunc();

private:
	BoundingBox* box;
	glm::vec3 position, scale;
	float angle, modelSize, duration, tick;

	shaderType type;

	t_hitfunc onHit;
	time_t spawnTime;

	glm::mat4 placeObject();
};

#endif
