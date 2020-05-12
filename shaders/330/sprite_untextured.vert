#version 330

uniform mat4 myProjectionMatrix;
uniform mat4 translationMatrix;
uniform vec3 spriteColor;

in vec3 myPosition;

out vec3 spriteColorFrag;

void main() {
	spriteColorFrag = spriteColor;

    // calculate vertex position
	vec4 position = myProjectionMatrix * translationMatrix * vec4(myPosition, 1.0);
    gl_Position = position;
}