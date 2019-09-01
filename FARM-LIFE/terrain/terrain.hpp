GLuint createTerrain() {
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Create flat plane of 4 vertices
	float vertices[] = {
		//  Position		Color
		-0.5f,  0.0f, 0.5f, 1.0f, 0.0f, 0.0f, // Top-left
		 0.5f,  0.0f, 0.5f, 1.0f, 0.0f, 0.0f, // Top-right
		 0.5f,  0.0f, -0.5f, 1.0f, 0.0f, 0.0f, // Bottom-right
		-0.5f,  0.0f, -0.5f, 1.0f, 0.0f, 0.0f, // Bottom-left
	};


	//-----------------------------
	//----CREATE BUFFERS-----------
	//-----------------------------

	// Generate vertex buffers
	GLuint vbo;
	glGenBuffers(1, &vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Generate element buffer
	GLuint ebo;
	glGenBuffers(1, &ebo);
	GLuint elements[] = {
		0, 1, 2,
		2, 3, 0
	};

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

	//-----------------------------
	//-CREATE SHADERS AND ADD DATA-
	//-----------------------------

	// Create and link the vertex and fragment shaders
	GLuint shaderProgram = LoadShaders("terrain/terrain.vert", "terrain/terrain.frag");

	// Link data to shaders
	GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE,
		6 * sizeof(float), 0);

	GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
	glEnableVertexAttribArray(colAttrib);
	glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE,
		6 * sizeof(float), (void*)(3 * sizeof(float)));
	return shaderProgram;
}

void generateTerrain(GLuint terrainShader, glm::mat4 Hvw, glm::mat4 Hcv) {
	glUseProgram(terrainShader);
	
	glm::mat4 Hwm = glm::mat4(1.0f);

	glUniformMatrix4fv(glGetUniformLocation(terrainShader, "Hvw"), 1, GL_FALSE, &Hvw[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(terrainShader, "Hcv"), 1, GL_FALSE, &Hcv[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(terrainShader, "Hwm"), 1, GL_FALSE, &Hwm[0][0]);
	
	glDrawArrays(GL_TRIANGLES, 0, 4);
}