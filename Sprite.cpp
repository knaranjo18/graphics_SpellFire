#include "Sprite.h"

Sprite::Sprite() {
	position = glm::vec3(0.0, 0.0, 0.0);
	scale = glm::vec3(1.0, 1.0, 1.0);
	color = glm::vec3(1.0, 1.0, 1.0);
	angle = 0.0f;
	spriteType = SPRITE;
	initComplete = false;
	ground_type = BACKGROUND;
}

Sprite::Sprite(shaderType _spriteType, glm::vec2 _position, glm::vec2 _scale, float _angle, glm::vec3 _color, groundType type) {
	spriteType = _spriteType;
	scale = glm::vec3(_scale, 1.0);
	angle = _angle;
	initComplete = true;
	ground_type = type;
	color = _color;
	position = glm::vec3(_position, 0.0);
}

Sprite::~Sprite() {

}

void Sprite::draw(glm::mat4 modelView, ShaderManager *shader, ply *myPly) {
	shader->useShader();

	GLint modelView_id = glGetUniformLocation(shader->program, "myModelviewMatrix");
	glUniformMatrix4fv(modelView_id, 1, false, glm::value_ptr(modelView));

	glm::mat4 transMat4(1.0f);
	transMat4 = placeSprite();

	GLint trans_id = glGetUniformLocation(shader->program, "translationMatrix");
	glUniformMatrix4fv(trans_id, 1, false, glm::value_ptr(transMat4));

	GLint color_id = glGetUniformLocation(shader->program, "spriteColor");
	glUniform3f(color_id, color.x, color.y, color.z);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	myPly->renderVBO();
}

glm::mat4 Sprite::placeSprite() {
	glm::mat4 transMat4(1.0f);

	transMat4 = glm::translate(transMat4, position);
	transMat4 = glm::scale(transMat4, scale);
	transMat4 = glm::rotate(transMat4, angle, glm::vec3(0.0, 0.0, 1.0));

	return transMat4;
}

void Sprite::setPosition(glm::vec2 _pos) {
	position = glm::vec3(_pos, 0.0);
}

void Sprite::setScale(glm::vec2 _scale) {
	scale = glm::vec3(_scale, 1.0);
}

void Sprite::setAngle(float _angle) {
	angle = _angle;
}

void Sprite::setColor(glm::vec3 _color) {
	color = _color;
}

void Sprite::setEverything(shaderType _spriteType, glm::vec2 _position, glm::vec2 _scale, float _angle, glm::vec3 _color, groundType type) {
	spriteType = _spriteType;
	position = glm::vec3(_position, 0.0);
	scale = glm::vec3(_scale, 1.0);
	angle = _angle;
	initComplete = true;
	color = _color;
	ground_type = type;
}