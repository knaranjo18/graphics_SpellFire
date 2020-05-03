#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include "glm/gtx/string_cast.hpp"
#include "glm/ext.hpp"

#include "gfxDefs.h"

#define DEFAULT_FOCUS_LENGTH 1.0f
#define NEAR_PLANE 0.01f
#define FAR_PLANE 20.0f
#define VIEW_ANGLE 60.0f

class Camera {
public:
	Camera();
	~Camera();

	void reset();
	void orientLookAt(glm::vec3 eyePoint, glm::vec3 focusPoint, glm::vec3 upVec);
	void orientLookVec(glm::vec3 eyePoint, glm::vec3 lookVec, glm::vec3 upVec);
	
	void setViewAngle(float _viewAngle);
	void setNearPlane(float _nearPlane);
	void setFarPlane(float _farPlane);
	void setScreenSize(int _screenWidth, int _screenHeight);
	void setEyePoint(glm::vec3 eyePoint);
	
	void rotateView(float yaw, float pitch);
	void translate(glm::vec3 v);

	glm::mat4 getUnhingeMatrix();
	glm::mat4 getProjectionMatrix();
	glm::mat4 getScaleMatrix();
	glm::mat4 getInverseScaleMatrix();
	glm::mat4 getModelViewMatrix();
	glm::mat4 getInverseModelViewMatrix();

	glm::vec3 getEyePoint();
	glm::vec3 getLookVector();
	glm::vec3 getUpVector();
	float getViewAngle();
	float getNearPlane();
	float getFarPlane();
	float getFilmPlanDepth();
	float getScreenWidthRatio();
	int getScreenWidth();
	int getScreenHeight();

private:
	float viewAngle, filmPlanDepth, nearPlane, farPlane, screenWidthRatio;
	int screenWidth, screenHeight;	
	glm::mat4 rotationMat4;
	glm::vec3 lookV, upV, eyeP, lookAtP;

	float get_thetaW(float thetaH);
	void makeuvw(glm::vec3 & u, glm::vec3 & v, glm::vec3 & w);
};
#endif

