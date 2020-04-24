#include "Camera.h"
#include <iostream>
using namespace std;

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
	rotU = rotV = rotW = 0;
}

//called by main.cpp as a part of the slider callback for controlling rotation
// the reason for computing the diff is to make sure that we are only incrementally rotating the camera
void Camera::setRotUVW(float u, float v, float w) {
	float diffU = u - rotU;
	float diffV = v - rotV;
	float diffW = w - rotW;
	rotateU(diffU);
	rotateV(diffV);
	rotateW(diffW);
	rotU = u;
	rotV = v;
	rotW = w;
}


// Sets where the camera is looking using a point
void Camera::orientLookAt(glm::vec3 eyePoint, glm::vec3 lookatPoint, glm::vec3 upVec) {
	eyeP = eyePoint;
	lookAtP = lookatPoint;
	upV = glm::normalize(upVec);
	lookV = glm::normalize(lookatPoint - eyePoint);
	makeRotationMatrix();
}

// Sets where the camera is looking using a vector
void Camera::orientLookVec(glm::vec3 eyePoint, glm::vec3 lookVec, glm::vec3 upVec) {
	eyeP = eyePoint;
	lookAtP = eyePoint + lookVec;
	upV = glm::normalize(upVec);
	lookV = glm::normalize(lookVec);
	makeRotationMatrix();
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


// Returns the translation matrix used in the model view matrix
glm::mat4 Camera::getTranslateMatrix() {
	glm::mat4 translateMat4(1.0);

	translateMat4[3][0] = -eyeP.x;
	translateMat4[3][1] = -eyeP.y;
	translateMat4[3][2] = -eyeP.z;

	return translateMat4;
}

// Creates the rotation matrix used in the model view matrix
void Camera::makeRotationMatrix() {
	glm::vec3 u, v, w;
	glm::mat4 tempRot(1.0);

	makeuvw(u, v, w);

	tempRot[0][0] = u.x;
	tempRot[1][0] = u.y;
	tempRot[2][0] = u.z;

	tempRot[0][1] = v.x;
	tempRot[1][1] = v.y;
	tempRot[2][1] = v.z;

	tempRot[0][2] = w.x;
	tempRot[1][2] = w.y;
	tempRot[2][2] = w.z;

	rotationMat4 = tempRot;
}

// Returns the model view matrix by multiplying (rotation * translate)
glm::mat4 Camera::getModelViewMatrix() {
	glm::mat4 modelViewMat4(1.0);

	glm::mat4 translateMat4 = getTranslateMatrix();

	modelViewMat4 = rotationMat4 * translateMat4;

	return modelViewMat4;
}

// Returns the inverse of the model view matrix by multplying (invTrans * invRotat)
glm::mat4 Camera::getInverseModelViewMatrix() {
	glm::mat4 invModelViewMat4(1.0);

	glm::mat4 invTranslate = getTranslateMatrix();
	glm::mat4 invRotation = glm::transpose(rotationMat4);

	invTranslate[3][0] *= -1.0;
	invTranslate[3][1] *= -1.0;
	invTranslate[3][2] *= -1.0;

	invModelViewMat4 = invTranslate * invRotation;

	return invModelViewMat4;
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

//*****************************************//
//		Rotation						  //
//***************************************//

void Camera::rotateV(float degrees) {
	glm::mat4 yRotate(1.0f);
	float theta = glm::radians(degrees);


	yRotate[0][0] = cos(theta);
	yRotate[0][2] = sin(theta);

	yRotate[2][0] = -sin(theta);
	yRotate[2][2] = cos(theta);

	rotationMat4 = yRotate * rotationMat4;
	lookV.x = -rotationMat4[0][2];
	lookV.y = -rotationMat4[1][2];
	lookV.z = -rotationMat4[2][2];

	upV.x = rotationMat4[0][1];
	upV.y = rotationMat4[1][1];
	upV.z = rotationMat4[2][1];
}

void Camera::rotateU(float degrees) {
	glm::mat4 xRotate(1.0f);
	float theta = glm::radians(degrees);


	xRotate[1][1] = cos(theta);
	xRotate[2][1] = sin(theta);

	xRotate[1][2] = -sin(theta);
	xRotate[2][2] = cos(theta);

	rotationMat4 = xRotate * rotationMat4;
	lookV.x = -rotationMat4[0][2];
	lookV.y = -rotationMat4[1][2];
	lookV.z = -rotationMat4[2][2];

	upV.x = rotationMat4[0][1];
	upV.y = rotationMat4[1][1];
	upV.z = rotationMat4[2][1];
	
}

void Camera::rotateW(float degrees) {
	glm::mat4 zRotate(1.0f);
	float theta = -glm::radians(degrees);


	zRotate[0][0] = cos(theta);
	zRotate[1][0] = sin(theta);

	zRotate[0][1] = -sin(theta);
	zRotate[1][1] = cos(theta);

	rotationMat4 = zRotate * rotationMat4;
	lookV.x = -rotationMat4[0][2];
	lookV.y = -rotationMat4[1][2];
	lookV.z = -rotationMat4[2][2];

	upV.x = rotationMat4[0][1];
	upV.y = rotationMat4[1][1];
	upV.z = rotationMat4[2][1];
}

// Rotates a point around an arbritrary axis
void Camera::rotate(glm::vec3 point, glm::vec3 axis, float degrees) {
	float rotRadY, rotRadZ, rotRadX;
	glm::mat4 bigMatrix(1.0f);

	rotRadY = atan(axis.z / axis.x);

	glm::mat4 M1 = glm::rotate(glm::mat4(1.0), rotRadY, glm::vec3(0.0, 1.0, 0.0));

	rotRadZ = -asin(axis.y / glm::length(axis));

	glm::mat4 M2 = glm::rotate(glm::mat4(1.0), rotRadZ, glm::vec3(0.0, 0.0, 1.0));

	rotRadX = glm::radians(degrees);
	
	glm::mat4 M3 = glm::rotate(glm::mat4(1.0), rotRadX, glm::vec3(1.0, 0.0, 0.0));

	bigMatrix = glm::transpose(M1) * glm::transpose(M2) * M3 * M2 * M1;

	point = glm::vec3(bigMatrix * glm::vec4(point, 0));
}

//**************************************//
//			End of Rotation			   //
//************************************//

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