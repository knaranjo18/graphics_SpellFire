#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include <glm/glm.hpp>

// Actually its a bounding sphere, shhhh
class BoundingBox {
public:
	BoundingBox();
	BoundingBox(glm::vec4, float);

	glm::vec4 getCenter();
	float	  getRadius();


	void setRadius(float);
	void setCenter(glm::vec4);
	void translate(glm::mat4);

	bool doesCollide(BoundingBox&);

private:
	float radius;
	glm::vec4 center;
};
#endif