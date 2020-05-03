#include "BoundingBox.h"

// Default: center->[0,0,0] radius->0.0
BoundingBox::BoundingBox() {
	this->center = glm:: vec4(0, 0, 0, 1);
	this->radius = 0;
}

BoundingBox::BoundingBox(glm::vec4 center, float radius) {
	this->center = center;
	this->radius = radius;
}

glm::vec4 BoundingBox::getCenter() {
	return center;
}

float BoundingBox::getRadius() {
	return radius;
}

void BoundingBox::setCenter(glm::vec4 center) {
	this->center = center;
}

void BoundingBox::setRadius(float radius) {
	this->radius = radius;
}

// Move the center according to the translation matrix.
// Passing a rotation matrix may have unintended effects.
void BoundingBox::translate(glm::mat4 trans) {
	center = trans * center;
}

// Returns true if the boxes collide, false otherwise
bool BoundingBox::doesCollide(BoundingBox& other) {
	glm::vec4 closest_line = other.getCenter() - center;

	return glm::length(closest_line) < (radius + other.getRadius());
}