#ifndef Scenery_H
#define Scenery_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include "glm/gtx/string_cast.hpp"
#include "glm/ext.hpp"

#include "ShaderManager.h"
#include "ply.h"
#include "gfxDefs.h"

class Scenery {
public:
	Scenery();
	Scenery(shaderType sceneType, glm::vec3 _position, glm::vec3 _scale, float _angle);
	~Scenery();

	void draw(glm::mat4 modelView, ShaderManager *shader, ply *myPly);

	shaderType sceneType;

private:
	glm::mat4 placeObject();

	glm::vec3 position, scale;
	float angle;

};
#endif