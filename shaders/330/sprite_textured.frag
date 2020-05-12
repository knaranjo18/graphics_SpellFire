 #version 330

in vec3 spriteColorFrag;
in vec2 textureCoordinate;

uniform sampler2D tex;


out vec4 outputColor;

void main() {
	vec4 texColor = texture(tex, vec2(textureCoordinate.x - 0.3, textureCoordinate.y));
	outputColor = vec4(spriteColorFrag, 1.0) * texColor;
}