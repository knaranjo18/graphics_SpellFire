#version 330

in vec3 norm;
in vec3 lightVec;

out vec4 outputColor; //this tells OpenGL that this variable is the output color
void main()
{
    float dotProd = dot(norm, lightVec) * 0.8;
	outputColor = vec4(dotProd * norm, 1.0);
	//outputColor = vec4(0.0, 1.0, 0.0, 0.0);
}