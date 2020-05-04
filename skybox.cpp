#include "skybox.h"

Skybox::Skybox() {
	for (int i = 0; i < 6; i++) {
		texture[i] = NULL;
	}
	setupShader();
}

// order MUST be right left top bottom back front
Skybox::Skybox(string filename[6]) {
	setupShader();
	applyTexture(filename);
}

Skybox::~Skybox() {
	for (int i = 0; i < 6; i++) {
		if (texture[i]) {
			delete texture[i];
		}
	}
}

void Skybox::setupShader() {
	shader = new ShaderManager();

	shader->initShader("./shaders/330/skybox.vert", "./shaders/330/skybox.frag");
	bindVBO(shader->program);
}

void Skybox::applyTexture(string filename[6]) {
	for (int i = 0; i < 6; i++) {
		texture[i] = new ppm(filename[i]);
	}
	
	glGenTextures(1, &tex_id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, tex_id);

	for (int i = 0; i < 6; i++) {
		int width = texture[i]->getWidth();
		int height = texture[i]->getHeight();
		char* pixels = texture[i]->getPixels();

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, 
			width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 0);
	}

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

unsigned int Skybox::getTextureID() {
	if (texture) 
		return tex_id;
	else {
		fprintf(stderr, "skybox texture not initialized, cannot return id!\n");
		exit(1);
	}
}

//TODO do we need to use programID here?
void Skybox::bindVBO(unsigned int programID) {
	for (int i = 0; i < 12; i++) {
		indices_vao[i * 3 + 0] = i * 3 + 0;
		indices_vao[i * 3 + 1] = i * 3 + 1;
		indices_vao[i * 3 + 2] = i * 3 + 2;
	}

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vertexVBO_id);
	glBindBuffer(GL_ARRAY_BUFFER, vertexVBO_id);

	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * N_FLOATS, skyboxVertices, GL_STATIC_DRAW);
	
	
	GLint pos_attr = glGetAttribLocation(shader->program, "myPosition");
	
	glVertexAttribPointer(pos_attr, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(pos_attr);

	printf("Created vbo successfully\n");
	/* 
	glGenBuffers(1, &indicesVBO_id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesVBO_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * 36, indices_vao, GL_STATIC_DRAW);
	GLint position_attribute = glGetAttribLocation(shader->program, "myPosition");
	glVertexAttribPointer(position_attribute, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(position_attribute);
	*/
}

void Skybox::draw(glm::mat4 modelView, glm::mat4 projectionMatrix) {
	shader->useShader();

	// remove translation so that skybox does not move, appears huge
	modelView = glm::mat4(glm::mat3(modelView));
	GLint modelView_id = glGetUniformLocation(shader->program, "myModelviewMatrix");
	glUniformMatrix4fv(modelView_id, 1, GL_FALSE, glm::value_ptr(modelView));

	GLuint projection_id = glGetUniformLocation(shader->program, "myProjectionMatrix");
	glUniformMatrix4fv(projection_id, 1, false, glm::value_ptr(projectionMatrix));

	glDepthMask(GL_FALSE);

	glBindVertexArray(vao);
	glBindTexture(GL_TEXTURE_CUBE_MAP, tex_id);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	
	glDepthMask(GL_TRUE);
}