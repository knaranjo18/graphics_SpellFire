#version 330

in vec3 texCoords;
uniform samplerCube skybox;

out vec4 outputColor;

void main() {
	outputColor = texture(skybox, vec3(-texCoords.x, texCoords.y, -texCoords.z));
}