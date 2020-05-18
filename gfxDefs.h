#ifndef DEFS_H
#define DEFS_H

const float EPSILON = 1e-5;
#define PI glm::pi<float>()
#define IN_RANGE(a,b)   (((a>(b-EPSILON))&&(a<(b+EPSILON)))?1:0)
#define TO_RADIANS(a)   (a*PI/180.0f)
#define TO_VEC3(v)		vec3df(v.x, v.y, v.z)
#define ARENA_SIZE 2.75f
#define HEIGHT 0.0
#define PLYSIZE 0.35f
#define NEAR_PLANE 0.01f
#define FAR_PLANE 20.0f

#define MASTER_VOLUME 0.0f
#define MUSIC_VOLUME 0.1f
#define ENEMY_VOLUME 0.8f
#define PROJECTILE_VOLUME 0.5f
#define PLAYER_VOLUME 0.7f
#define PICKUP_VOLUME 1.2f

enum shaderType { SPRITE_LOADING, GOOP, JAD, FIREBALL, SPRITE_UNTEXTURED, SPRITE_DEATH, ARENA, HEALTHPOT, MANAPOT, SPRITE_MAIN, BUTTON_START, 
					BUTTON_OPTIONS, BUTTON_QUIT, BUTTON_CONTROLS, SKYBOX };
enum groundType { FOREGROUND, BACKGROUND };
enum GameState { MAIN_MENU, LOADING, PLAYING, PAUSE, OPTIONS, CONTROLS, DEAD };
#endif
