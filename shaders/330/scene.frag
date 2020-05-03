#version 330

#define PI 3.14159265359
#define EPSILON 0.00001
#define BLEND 0.0
#define IN_RANGE(a,b)   (((a>(b-EPSILON))&&(a<(b+EPSILON)))?true:false)

in vec3 norm;
in vec3 lightVec;
in vec2 textureCoordinate;

// texture data
uniform sampler2D tex;

out vec4 outputColor; //this tells OpenGL that this variable is the output color

vec4 getTextureColorSphereMap(int, int);
vec3 findCubeIntersect();
vec4 getTextureColorCubeMap(int, int);

void main()
{
	vec3 color = vec3(0.7, 0.4, 0.2);
	vec3 newNorm = normalize(norm);
	vec3 newLightVec = normalize(lightVec);
    float diffuse = max(dot(newNorm, newLightVec), 0.0);
	

	vec4 texColor;
	if (isnan(textureCoordinate.x)) { // if we have no embedded texture coordinates
		texColor = getTextureColorSphereMap(10, 10);
	} else {
		texColor = texture(tex, vec2(textureCoordinate.x, 1-textureCoordinate.y));
	}

	// cap dark areas at 20% brightness 
	if (diffuse <= 0.2) {
		diffuse = 0.2;
	}

	color.x = BLEND * color.x + (1 - BLEND) * texColor.x;
	color.y = BLEND * color.y + (1 - BLEND) * texColor.y;
	color.z = BLEND * color.z + (1 - BLEND) * texColor.z;
	color *= diffuse;
	outputColor = vec4(color.xyz, 1.0);
	//outputColor = vec4(textureCoordinate.x, textureCoordinate.y, 0, 1);
}

// sphere intersect copied from a5 sphere code
vec4 getTextureColorSphereMap(int u_scale, int v_scale) {
// wrap like a sphere, does not work for the arena D:
	float u;
	float v;
	float theta = atan(norm.z, norm.x); // works like atan2 in c++
	
	if (theta < 0) {
		u = -theta / (2 * PI);
	} else {
		u = 1 - (theta / (2 * PI));
	}

	float phi = asin(norm.y / 0.5);
	v = 0.5 + (phi / PI);
	
	if (u > 0.5) {
		u = u - 0.5;
	} else {
		u = u + 0.5;
	}

	if (abs(v) < EPSILON || abs(v - 1.0) < EPSILON) {
		u = 0.5;
	}
	return texture(tex, vec2(u * u_scale,v * v_scale));
}

// not a true cubemap because we are going to use this texture as all 6 faces of the cube
vec4 getTextureColorCubeMap(int u_scale, int v_scale) {
	vec3 pt = findCubeIntersect();
	float u;
	float v;
	if (IN_RANGE(pt.x, 0.5)) {
		u = (pt.z - 0.5) / -1.0f;
		v = (pt.y + 0.5) / 1.0f;
	} else if (IN_RANGE(pt.x, -0.5)) {
		u = (pt.z + 0.5) / 1.0;
		v = (pt.y + 0.5) / 1.0;
	} else if (IN_RANGE(pt.y, 0.5)) {
		u = (pt.x + 0.5) / 1.0;
		v = (pt.z + 0.5) / 1.0;
	} else if (IN_RANGE(pt.y, -0.5)) {
		u = (pt.x + 0.5) / 1.0;
		v = (pt.z + 0.5) / 1.0;
	} else if (IN_RANGE(pt.z, 0.5)) {
		u = (pt.x + 0.5) / 1.0;
		v = (pt.y + 0.5) / 1.0;
	} else {
		u = (pt.x - 0.5) / -1.0;
		v = (pt.y + 0.5) / 1.0;
	}
	return texture(tex, vec2(u * u_scale, v * v_scale));
}


vec3 findCubeIntersect() {
	float x = abs(norm.x);
	float y = abs(norm.y);
	float z = abs(norm.z);
	float scale = 0.5 / max(max(x, y), z);
	// scale the largest factor to be 0.5, putting it on a side of the cube
	// the other 2 coordinates will be used for calculation the position on the cube
	return vec3(norm) * scale;
}