#ifndef GAMEDEF_H
#define GAMEDEF_H

// Definitions required for the game,


// HITDATA represents the current state of an entity and the t_hitfunc represents
// how that state is transformed when that entity is hit by a projectile
// t_hitfunc is called by the hit entity by thier applyHit funciton
struct HITDATA {
	float health;
	float mana;
	float speed;
	HITDATA(float h, float m, float s) {
		health = h;
		mana = m;
		speed = s;
	}
};

// t_hitfunc is what projectiles apply to an entity's hitdata.  The entity will then
// use those values to update its state
typedef void(*t_hitfunc)(HITDATA&);


#endif