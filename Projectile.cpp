#include "Projectile.h"

#define FIREBALLSIZE 0.1f


bool Projectile::debug_draw_hitbox = false;

void fireball_hitCB(HITDATA &h) {
	h.health -= 100;
}

Projectile::Projectile() {
	projectileType = FIREBALL;
	speed = 0.05;
	duration = 2;
	currAngleYaw = currAnglePitch = 0;
	initialAngleYaw = 0;
	initialAnglePitch = 0;
	position = glm::vec3(0, 0, 0);
	orientation = glm::vec3(1, 0, 0);
	scaleSize = dirFired = glm::vec3(1, 1, 1);
	modelSize = FIREBALLSIZE * PLYSIZE;
	box = new BoundingBox(glm::vec4(position, 1.0f), modelSize);
	spawnTime = time(0);
	onHit = &fireball_hitCB;
}

Projectile::Projectile(shaderType _type, glm::vec3 startPos, glm::vec3 directionFired, ISoundEngine *engine) {
	ISound *sound = NULL;
	switch (_type) {
	case(FIREBALL):
		speed = 0.05;
		duration = 2;
		currAngleYaw = currAnglePitch = 0;
		initialAngleYaw = 0;
		initialAnglePitch = 0;
		scaleSize = glm::vec3(1.0f, 1.0f, 1.0f) * FIREBALLSIZE;
		position = startPos;
		dirFired = directionFired;
		orientation = glm::vec3(1.0, 1.0, 1.0);
		modelSize = FIREBALLSIZE * PLYSIZE;
		box = new BoundingBox(glm::vec4(position, 1.0f), modelSize);
		spawnTime = time(0);
		onHit = &fireball_hitCB;

		sound = engine->play2D("./audio/fireball.mp3", false, false, true);
		break;
	default:
		printf("NOT A VALID SPELL");
		exit(1);
		break;
	}
	sound->setVolume(PROJECTILE_VOLUME);
	sound->drop();
	projectileType = _type;

}

void Projectile::makeFireball(shaderType _type, glm::vec3 startPos, glm::vec3 directionFired) {
	// todo, move fireball code here
}

Projectile::~Projectile() {
	if (box)
		delete box;
}

void Projectile::draw(ShaderManager *shader, ply *myPly) {
	GLint trans_id = glGetUniformLocation(shader->program, "translationMatrix");
	glUniformMatrix4fv(trans_id, 1, false, glm::value_ptr(transMat4));

	// Pass scenery texture to the shader
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, myPly->getTextureID());
	glUniform1i(glGetUniformLocation(shader->program, "tex"), 1);

	//renders the object
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	myPly->renderVBO();

	//if (debug_draw_hitbox) glutSolidSphere(0.5, 15, 15);
}

void Projectile::moveProjectile() {
	glm::vec2 directionXZ = glm::normalize(glm::vec2(dirFired.x, dirFired.z));
	glm::vec2 lookXZ = glm::vec2(orientation.x, orientation.z);

	// Calculates angle that the projectile should face left/right
	currAngleYaw = acos(glm::dot(glm::normalize(directionXZ), glm::normalize(lookXZ)));
	if (dirFired.z > 0) {
		currAngleYaw = (2 * PI) - currAngleYaw;  //Deals with some stupid loop in the angle
	}

	glm::vec2 directionYZ = glm::normalize(glm::vec2(dirFired.y, dirFired.z));
	glm::vec2 lookYZ = glm::vec2(orientation.y, orientation.z);

	// Calculates angle that the projectile should face up/down
	float currAnglePitch = acos(glm::dot(glm::normalize(directionYZ), glm::normalize(lookYZ)));
	if (dirFired.z > 0) {
		currAnglePitch = (2 * PI) - currAnglePitch;  //Deals with some stupid loop in the angle
	}

	position += glm::normalize(dirFired) * speed;

	box->setCenter(glm::vec4(position, 1.0f));

	glm::mat4 transMat4(1.0f);
	transMat4 = glm::translate(transMat4, position);
	transMat4 = glm::scale(transMat4, scaleSize);
	transMat4 = glm::rotate(transMat4, currAngleYaw + initialAngleYaw, glm::vec3(0.0, 1.0, 0.0));
	transMat4 = glm::rotate(transMat4, currAnglePitch + initialAnglePitch, glm::vec3(1.0, 0.0, 0.0));

	this->transMat4 = transMat4;
}

t_hitfunc Projectile::getHitfunc()
{
	return onHit;
}

const BoundingBox* Projectile::getBox()
{
	return box;
}

time_t Projectile::getSpawnTime() {
	return spawnTime;
}

float Projectile::getDuration() {
	return duration;
}

bool Projectile::hitFloor() {
	return position.y < -0.15;
}

shaderType Projectile::getType() {
	return projectileType;
}