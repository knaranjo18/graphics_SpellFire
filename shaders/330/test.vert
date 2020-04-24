#version 330

uniform mat4 myModelviewMatrix; 
uniform mat4 myProjectionMatrix;
uniform vec3 lightPos;

in vec3 myPosition;
in vec3 myNormal;

out vec3 norm;
out vec3 lightVec;

void main() {
    norm = myNormal;
    lightVec = myPosition - lightPos;
    vec4 position = myProjectionMatrix * myModelviewMatrix * vec4(myPosition, 1.0);
    gl_Position = position;
}

