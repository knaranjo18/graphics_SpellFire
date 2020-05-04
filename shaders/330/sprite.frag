 #version 330

in vec3 spriteColorFrag;

out vec4 outputColor;

void main() {
	outputColor = vec4(spriteColorFrag, 1.0);
}