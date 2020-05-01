#version 330

uniform mat4 myModelviewMatrix; 
uniform mat4 myProjectionMatrix;
uniform mat4 translationMatrix;
//uniform vec3 lightPos;
//uniform vec3 lightDir;

in vec3 myPosition;
in vec3 myNormal;

out vec3 norm;
out vec3 lightVec;

void main() {
    norm = myNormal;
   // lightVec =   myPosition - lightPos;
    lightVec = vec3(0.0, -1.0, 0.5);
	vec4 position = myProjectionMatrix * myModelviewMatrix * translationMatrix * vec4(myPosition, 1.0);
    gl_Position = position;
}

