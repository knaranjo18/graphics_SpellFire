#version 330

in vec3 norm;
in vec3 lightVec;

out vec4 outputColor; //this tells OpenGL that this variable is the output color
void main()
{
	vec3 newNorm = normalize(norm);
	vec3 newLightVec = normalize(lightVec);
    float diffuse = max(dot(newNorm, newLightVec), 0.0);
	if (diffuse <= 0.0) {
		outputColor = vec4(0.05, 0.05, 0.05, 1.0);
	} else {
		outputColor = vec4(diffuse, diffuse, diffuse, 1.0);
	}
}