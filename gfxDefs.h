#ifndef DEFS_H
#define DEFS_H

const float EPSILON = 1e-5;
#define PI glm::pi<float>()
#define IN_RANGE(a,b)   (((a>(b-EPSILON))&&(a<(b+EPSILON)))?1:0)
#define TO_RADIANS(a)   (a*PI/180.0f)
#define ARENA_SIZE 1.85f
enum shaderType { COW, BUNNY, ARENA };

#endif