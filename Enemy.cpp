#include "Enemy.h"

#define COWSIZE 0.3f
#define BUNNYSIZE 0.5f

bool Enemy::debug_draw_hitbox = false;

Enemy::Enemy() {
	health = 100.0;
	speed = 0.0001;
	position = glm::vec3(1, -0.4, 0.0);
	lookVector = glm::vec3(1.0, 0.0, 0.0);
	scaleSize = glm::vec3(0.3, 0.3, 0.3);
	pointValue = 1;
	initialAngle = 0;
	currAngle = 0;
	angularSpeed = PI / 10080.0;
	enemyType = COW;
	modelSize = COWSIZE * PLYSIZE;
	box = new BoundingBox(glm::vec4(position, 1.0f), modelSize);
	transMat4 = glm::mat4(1.0f);
	transMat4 = glm::translate(transMat4, position);
	transMat4 = glm::scale(transMat4, scaleSize);
	transMat4 = glm::rotate(transMat4, currAngle + initialAngle, glm::vec3(0.0, 1.0, 0.0));
}

Enemy::Enemy(shaderType _enemyType, glm::vec3 startPoint) {
	switch (_enemyType) {
	case(COW):
		health = 100.0;
		speed = 0.0001;
		position = startPoint;
		lookVector = glm::vec3(1.0, 0.0, 0.0);
		scaleSize = glm::vec3(1, 1, 1) * COWSIZE;
		pointValue = 1;
		initialAngle = 0;
		currAngle = 0;
		angularSpeed = PI / 6000.0;
		enemyType = _enemyType;
		modelSize = PLYSIZE * COWSIZE;
		break;
	case(BUNNY):
		health = 200.0;
		speed = 0.0002;
		position = startPoint;
		lookVector = glm::vec3(1.0, 0.0, 0.0);
		scaleSize = glm::vec3(1, 1, 1) * BUNNYSIZE;
		pointValue = 2;
		initialAngle = PI;
		currAngle = 0;
		angularSpeed = PI / 6000.0;
		enemyType = _enemyType;
		modelSize = PLYSIZE * BUNNYSIZE;
		break;
	default:
		printf("NOT A VALID ENEMY");
		exit(1);
		break;
	}
	box = new BoundingBox(glm::vec4(startPoint, 1.0f), modelSize);
	transMat4 = glm::mat4(1.0f);
	transMat4 = glm::translate(transMat4, position);
	transMat4 = glm::scale(transMat4, scaleSize);
	transMat4 = glm::rotate(transMat4, currAngle + initialAngle, glm::vec3(0.0, 1.0, 0.0));
}


Enemy::~Enemy() {
	if (box)
		delete box;
}

void Enemy::setHealth(float _health) {
	health = _health;
}

float Enemy::getHealth() {
	return health;
}

const BoundingBox* Enemy::getBox() {
	return box;
}

void Enemy::applyHit(t_hitfunc f) {
	HITDATA data(health, 0, speed); // enemies have no mana (yet)
	f(data);
	health = data.health;
	speed = data.speed;
}

void Enemy::draw(glm::mat4 modelView, ShaderManager* shader, ply* myPly) {
	shader->useShader();
	GLint modelView_id = glGetUniformLocation(shader->program, "myModelviewMatrix");
	glUniformMatrix4fv(modelView_id, 1, false, glm::value_ptr(modelView));

	GLint trans_id = glGetUniformLocation(shader->program, "translationMatrix");
	glUniformMatrix4fv(trans_id, 1, false, glm::value_ptr(transMat4));

	//renders the object
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	myPly->renderVBO();

	if (debug_draw_hitbox) {
		glutSolidSphere(0.5, 15, 15);
	}
}

float boundAngle(float angle) {
	if (angle < 0) {
		angle += 2 * PI;
	}
	else if (angle > 2 * PI) {
		angle -= 2 * PI;
	}

	return angle;
}

void Enemy::moveEnemy(glm::vec3 playerPos) {
	glm::vec3 direction = glm::normalize(playerPos - position) * speed;

	// Used for 2D angle calculations
	glm::vec2 direction2 = glm::normalize(glm::vec2(direction.x, direction.z));
	glm::vec2 look2 = glm::vec2(lookVector.x, lookVector.z);

	// Calculates angle that the enemy should rotate to face the player
	float angle_offset = acos(glm::dot(glm::normalize(direction2), glm::normalize(look2)));
	if (direction.z > 0) {
		angle_offset = (2 * PI) - angle_offset;  //Deals with some stupid loop in the angle
	}

	float inv_angle_offset = boundAngle(angle_offset - PI);
	
	float counterClockAng = boundAngle(angle_offset - currAngle);
	float clockAng = boundAngle(inv_angle_offset - currAngle);

	if (counterClockAng <= clockAng)
		currAngle = boundAngle(currAngle + angularSpeed);
	else
		currAngle = boundAngle(currAngle - angularSpeed);

	glm::vec3 newLook(cos(currAngle), 0.0, -sin(currAngle));
	position += glm::normalize(newLook) * speed;

	box->setCenter(glm::vec4(position, 1.0f));

	glm::mat4 transMat4(1.0f);
	transMat4 = glm::translate(transMat4, position);
	transMat4 = glm::scale(transMat4, scaleSize);
	transMat4 = glm::rotate(transMat4, currAngle + initialAngle, glm::vec3(0.0, 1.0, 0.0));

	this->transMat4 = transMat4;
}

