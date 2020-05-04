#version 330

uniform mat4 myModelviewMatrix; 
uniform mat4 myProjectionMatrix;
layout (location = 0) in vec3 myPosition;

out vec3 texCoords;

void main() {
	texCoords = myPosition;
	gl_Position = myProjectionMatrix * myModelviewMatrix * vec4(myPosition, 1.0);
}