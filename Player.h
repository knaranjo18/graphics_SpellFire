#ifndef PLAYER_H
#define PLAYER_H

#include "BoundingBox.h"
#include "Camera.h"
#include "ShaderManager.h"
#include "ply.h"

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

	Camera *myCam;
	bool canMoveSight;
private:
	int points;
	float speed, mana, health, yaw, pitch;


};
#endif