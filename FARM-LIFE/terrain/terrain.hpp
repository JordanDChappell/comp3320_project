struct terraObj {
	GLuint terraShader;
	GLuint vbo;
	GLuint vao;
} ;

terraObj createTerrain() {
	terraObj terra;
	
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	terra.vao = vao;

	// Create flat plane of 4 vertices
	float vertices[24] = {
		//  Position		Color
		-0.5f, 0.0f,  0.5f, 0.0f, 1.0f, 0.0f, // Top-left
		 0.5f, 0.0f,  0.5f, 0.0f, 0.0f, 1.0f, // Top-right
		 0.5f, 0.0f, -0.5f, 1.0f, 0.0f, 0.0f, // Bottom-right
		-0.5f, 0.0f, -0.5f, 1.0f, 1.0f, 1.0f, // Bottom-left
	};


	//-----------------------------
	//----CREATE BUFFERS-----------
	//-----------------------------

	// Generate vertex buffers
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	terra.vbo = vbo;

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

	terra.terraShader = shaderProgram;
	
	return terra;
}

void generateTerrain(terraObj terra, glm::mat4 Hvw, glm::mat4 Hcv) {
	glUseProgram(terra.terraShader);
	glBindVertexArray(terra.vao);
	glBindBuffer(GL_ARRAY_BUFFER, terra.vbo);

	glm::mat4 Hwm = glm::mat4(1.0f);

	glUniformMatrix4fv(glGetUniformLocation(terra.terraShader, "Hvw"), 1, GL_FALSE, &Hvw[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(terra.terraShader, "Hcv"), 1, GL_FALSE, &Hcv[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(terra.terraShader, "Hwm"), 1, GL_FALSE, &Hwm[0][0]);
	
	glDrawArrays(GL_TRIANGLES, 0, 3);
}