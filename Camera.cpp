#include "Camera.h"

Camera::Camera() {
	reset();
}

Camera::~Camera() {
}

// Resets the camera when first initialized
void Camera::reset() {
	orientLookAt(glm::vec3(0.0f, 0.0f, DEFAULT_FOCUS_LENGTH), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	setViewAngle(VIEW_ANGLE);
	setNearPlane(NEAR_PLANE);
	setFarPlane(FAR_PLANE);
	screenWidth = screenHeight = 200;
	screenWidthRatio = 1.0f;
}

// Sets where the camera is looking using a point
void Camera::orientLookAt(glm::vec3 eyePoint, glm::vec3 lookatPoint, glm::vec3 upVec) {
	eyeP = eyePoint;
	lookAtP = lookatPoint;
	upV = glm::normalize(upVec);
	lookV = glm::normalize(lookatPoint - eyePoint);
}

// Sets where the camera is looking using a vector
void Camera::orientLookVec(glm::vec3 eyePoint, glm::vec3 lookVec, glm::vec3 upVec) {
	eyeP = eyePoint;
	lookAtP = eyePoint + lookVec;
	upV = glm::normalize(upVec);
	lookV = glm::normalize(lookVec);
}

// Returns the scale matrix of the projection matrix
glm::mat4 Camera::getScaleMatrix() {
	glm::mat4 scaleMat4(1.0);
	float thetaH = glm::radians(viewAngle);
	float thetaW = get_thetaW(thetaH);

	scaleMat4[0][0] = 1.0 / (tan(thetaW / 2.0) * farPlane);
	scaleMat4[1][1] = 1.0 / (tan(thetaH / 2.0) * farPlane);
	scaleMat4[2][2] = 1.0 / farPlane;

	return scaleMat4;
}

// Returns the inverse of the scale matrix of the projection matrix
glm::mat4 Camera::getInverseScaleMatrix() {
	glm::mat4 invScaleMat4(1.0);

	float thetaH = glm::radians(viewAngle);
	float thetaW = get_thetaW(thetaH);

	invScaleMat4[0][0] = (tan(thetaW / 2.0) * farPlane);
	invScaleMat4[1][1] = (tan(thetaH / 2.0) * farPlane);
	invScaleMat4[2][2] = farPlane;

	return invScaleMat4;
}

// Returns the unhinge matrix of the projection matrix
glm::mat4 Camera::getUnhingeMatrix() {
	glm::mat4 unhingeMat4(1.0);

	float c = -nearPlane / farPlane;

	unhingeMat4[2][2] = -1.0 / (c + 1.0);
	unhingeMat4[3][3] = 0;
	unhingeMat4[3][2] = c / (c + 1.0);
	unhingeMat4[2][3] = -1;

	return unhingeMat4;
}

// Returns the projection matrix by multiplying (Unhinge * Scale)
glm::mat4 Camera::getProjectionMatrix() {
	glm::mat4 projMat4(1.0f);
	glm::mat4 scaleMat4 = getScaleMatrix();
	glm::mat4 unhingeMat4 = getUnhingeMatrix();

	projMat4 = unhingeMat4 * scaleMat4;

	return projMat4;
}

// Returns the model view matrix by multiplying (rotation * translate)
glm::mat4 Camera::getModelViewMatrix() {
	return glm::lookAt(eyeP, eyeP + lookV, upV);
}

// Returns the inverse of the model view matrix by multplying (invTrans * invRotat)
glm::mat4 Camera::getInverseModelViewMatrix() {
	glm::mat4 modelViewMatrix = getModelViewMatrix();
	return glm::inverse(modelViewMatrix);
}


// Calculates thetaW from thetaH to do the scaling in the projection matrix
float Camera::get_thetaW(float thetaH) {
	float thetaW = 2.0 * atan(screenWidthRatio * tan(thetaH / 2.0));
	
	return thetaW;
}

// Calculates u, v, w based on the current look vector and up vector
void Camera::makeuvw(glm::vec3 &u, glm::vec3 &v, glm::vec3 &w) {
	glm::vec3 tempCross;

	w = -lookV / glm::length(lookV);

	tempCross = glm::cross(upV, w);
	u = tempCross / glm::length(tempCross);

	v = glm::cross(w, u);
}

// Applies translation v to the eye point
void Camera::translate(glm::vec3 v) {
	glm::mat4 translateMat4(1.0);

	translateMat4[3][0] = v.x;
	translateMat4[3][1] = v.y;
	translateMat4[3][2] = v.z;

	eyeP = glm::vec3(translateMat4 * glm::vec4(eyeP, 1.0));
}


// Rotates the camera by the yaw (around v) and pitch (around u)
void Camera::rotateView(float yaw, float pitch) {
	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	lookV = glm::normalize(direction);
}


//***********************************//
//		Simple Setters				//
//*********************************//
void Camera::setViewAngle(float _viewAngle) {
	viewAngle = _viewAngle;
}

void Camera::setNearPlane(float _nearPlane) {
	nearPlane = _nearPlane;
}

void Camera::setFarPlane(float _farPlane) {
	farPlane = _farPlane;
}

void Camera::setScreenSize(int _screenWidth, int _screenHeight) {
	screenWidth = _screenWidth;
	screenHeight = _screenHeight;
	screenWidthRatio = (float)screenWidth / (float)screenHeight;
}


void Camera::setEyePoint(glm::vec3 eyePoint) {
	eyeP = eyePoint;
}

//***********************************//
//		End of Setters				//
//*********************************//


//*************************************//
//		Simple Getters				  //
//***********************************//
glm::vec3 Camera::getEyePoint() {
	return eyeP;
}

glm::vec3 Camera::getLookVector() {
	return lookV;
}

glm::vec3 Camera::getUpVector() {
	return upV;
}

float Camera::getViewAngle() {
	return viewAngle;
}

float Camera::getNearPlane() {
	return nearPlane;
}

float Camera::getFarPlane() {
	return farPlane;
}

int Camera::getScreenWidth() {
	return screenWidth;
}

int Camera::getScreenHeight() {
	return screenHeight;
}

float Camera::getScreenWidthRatio() {
	return screenWidthRatio;
}

float Camera::getFilmPlanDepth() {
	return filmPlanDepth;
}

//*************************************//
//		End of Getters				 //
//**********************************//