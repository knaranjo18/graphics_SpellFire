#ifndef PLAYER_H
#define PLAYER_H

#include "BoundingBox.h"
#include "Camera.h"
#include "ShaderManager.h"
#include "ply.h"
#include "gamedefs.h"

class Player {
public:
	Player();
	~Player();

	void setHealth(float _health);
	void setPoints(int _points);
	void setMana(float _mana);

	float getHealth();
	int getPoints();
	float getMana();


	void moveForward();
	void moveBackward();
	void moveLeft();
	void moveRight();
	void moveSight(int x_offset, int y_offset);

	void applyHit(t_hitfunc);

	Camera *myCam;
	bool canMoveSight;
	shaderType spellSelected;
private:
	int points;
	float speed, mana, health, yaw, pitch;


};
#endif