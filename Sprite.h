#ifndef SPRITE_H
#define SPRITE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include "glm/gtx/string_cast.hpp"
#include "glm/ext.hpp"

#include "ShaderManager.h"
#include "ply.h"
#include "gfxDefs.h"



class Sprite {
public:
	Sprite();
	Sprite(shaderType _spriteType, glm::vec2 _position, glm::vec2 _scale, float _angle, glm::vec3 _color, groundType type);
	~Sprite();

	void draw(glm::mat4 modelView, ShaderManager *shader, ply *myPly);

	void setEverything(shaderType _spriteType, glm::vec2 _position, glm::vec2 _scale, float _angle, glm::vec3 _color, groundType type);
	
	void setPosition(glm::vec2 _pos);
	void setScale(glm::vec2 _scale);
	void setAngle(float _angle);
	void setColor(glm::vec3 _color);

	shaderType spriteType;
	groundType ground_type;
private:
	glm::vec3 position, scale, color;
	float angle;

	glm::mat4 placeSprite();


};

#endif