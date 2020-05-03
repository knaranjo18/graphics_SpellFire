#include "Scenery.h"

Scenery::Scenery() {
	position = glm::vec3(0.0, 0.0, 0.0);
	scale = glm::vec3(1.0, 1.0, 1.0);
	angle = 0.0;
	sceneType = ARENA;
}

Scenery::Scenery(shaderType _sceneType,  glm::vec3 _position, glm::vec3 _scale, float _angle) {
	position = _position;
	scale = _scale;
	angle = _angle;
	sceneType = _sceneType;
}

Scenery::~Scenery() {

}

void Scenery::draw(glm::mat4 modelView, ShaderManager *shader, ply *myPly) {
	shader->useShader();
	
	GLint modelView_id = glGetUniformLocation(shader->program, "myModelviewMatrix");
	glUniformMatrix4fv(modelView_id, 1, false, glm::value_ptr(modelView));

	glm::mat4 transMat4(1.0f);
	transMat4 = placeObject();

	GLint trans_id = glGetUniformLocation(shader->program, "translationMatrix");
	glUniformMatrix4fv(trans_id, 1, false, glm::value_ptr(transMat4));

	// Pass scenery texture to the shader
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, myPly->getTextureID());
	glUniform1i(glGetUniformLocation(shader->program, "tex"), 0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	myPly->renderVBO();
}

glm::mat4 Scenery::placeObject() {
	glm::mat4 transMat4(1.0f);

	transMat4 = glm::translate(transMat4, position);
	transMat4 = glm::scale(transMat4, scale);
	transMat4 = glm::rotate(transMat4, angle, glm::vec3(0.0, 1.0, 0.0));

	return transMat4;
}