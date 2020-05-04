#version 330

uniform mat4 myProjectionMatrix;
uniform mat4 translationMatrix;
uniform vec3 spriteColor;

in vec3 myPosition;

in vec2 myTextureCoordinate;

out vec2 textureCoordinate;


out vec3 spriteColorFrag;

void main() {
	spriteColorFrag = spriteColor;
	textureCoordinate = myTextureCoordinate;

    // calculate vertex position
	vec4 position = myProjectionMatrix * translationMatrix * vec4(myPosition, 1.0);
    gl_Position = position;
}