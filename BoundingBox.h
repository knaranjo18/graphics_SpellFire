#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include <glm/glm.hpp>

// Actually its a bounding sphere, shhhh
class BoundingBox {
public:
	BoundingBox();
	BoundingBox(glm::vec4, float);

	glm::vec4 getCenter() const;
	float getRadius() const;


	void setRadius(float);
	void setCenter(glm::vec4);
	void translate(glm::mat4);

	bool doesCollide(const BoundingBox&) const;

private:
	float radius;
	glm::vec4 center;
};
#endif