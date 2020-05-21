#include "pickup.h"

#define PLYSIZE 0.5f
#define POTSIZE 0.2f
#define ANIMATIONSPEED 0.05f

void healthPotCB(HITDATA& h) {
	h.health += 50;
}

void manaPotCB(HITDATA& h) {
	h.mana += 10;
}

Pickup::Pickup() {
	position = glm::vec3(0, 0, 0);
	scale = glm::vec3(1, 1, 1) * POTSIZE;
	modelSize = POTSIZE;
	modelSize *= PLYSIZE;
	box = new BoundingBox(glm::vec4(position, 1.0f), modelSize);
	type = HEALTHPOT;
	onHit = &healthPotCB;
	angle = 0;
	tick = 0;
	duration = 6;
	spawnTime = time(0);
}

Pickup::Pickup(glm::vec3 position, float angle, shaderType type, ISoundEngine *engine) {
	this->position = position;
	this->scale = glm::vec3(1, 1, 1) * POTSIZE;
	this->angle = angle;
	this->type = type;
	this->tick = 0;
	this->duration = 8;
	this->spawnTime = time(0);
	this->soundEngine = engine;
	
	if (type == HEALTHPOT) {
		onHit = &healthPotCB;
	} else if (type == MANAPOT) {
		onHit = &manaPotCB;
	} else {
		fprintf(stderr, "NOT A VALID POTION TYPE\n");
		exit(1);
	}

	modelSize = POTSIZE;
	modelSize *= PLYSIZE;
	box = new BoundingBox(glm::vec4(position, 1.0f), modelSize);
}

Pickup::~Pickup() {
	delete box;
}

void Pickup::draw(ShaderManager* shader, ply* myPly) {
	glm::mat4 transMat4(1.0f);
	transMat4 = placeObject();

	GLint trans_id = glGetUniformLocation(shader->program, "translationMatrix");
	glUniformMatrix4fv(trans_id, 1, false, glm::value_ptr(transMat4));

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, myPly->getTextureID());
	glUniform1i(glGetUniformLocation(shader->program, "tex"), 4);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	myPly->renderVBO();
}

const BoundingBox* Pickup::getBox() {
	return box;
}

shaderType Pickup::getType()
{
	return type;
}

t_hitfunc Pickup::getHitFunc()
{
	return onHit;
}

glm::mat4 Pickup::placeObject() {
	glm::mat4 transMat4(1.0f);

	position.y = HEIGHT + sin(tick) * 0.03;

	transMat4 = glm::translate(transMat4, position);
	transMat4 = glm::scale(transMat4, scale);
	transMat4 = glm::rotate(transMat4, angle, glm::vec3(0.0, 1.0, 0.0));

	tick += ANIMATIONSPEED;
	angle += ANIMATIONSPEED;

	return transMat4;
}

time_t Pickup::getSpawnTime() {
	return spawnTime;
}

float Pickup::getDuration() {
	return duration;
}

void Pickup::usePickupSound() {
	ISound *sound = NULL;
	switch (type) {
	case HEALTHPOT:
	case MANAPOT:
		sound = soundEngine->play2D("./audio/potion_swallow.mp3", false, false, true);
		break;
	}

	sound->setVolume(volume);
	sound->drop();
}