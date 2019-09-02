#include <vector>
struct terraObj {
	GLuint terraShader;
	GLuint vbo;
	GLuint vao;
	int resX;
	int resZ;
	int noVertices;
} ;

terraObj createTerrain() {
	terraObj terra;
	terra.resX = 128;
	terra.resZ = 128;

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	terra.vao = vao;

	float vertices[128][128][6];

	bool flag = true;

	for (int i = 0; i < 128; i++) {
		for (int j = 0; j < 128; j++) {
			// Position
			vertices[i][j][0] = (float)i;		// X
			vertices[i][j][1] = -1.0f;			// Y
			vertices[i][j][2] = (float)j;		// Z

			// Colour
			vertices[i][j][3] = 0.0f;
			vertices[i][j][4] = flag ? 0.3f : 0.8f;	// alternate greens
			flag = !flag;
			vertices[i][j][5] = 0.0f;
		}
	}

	// Order triangles
	std::vector<float> triangles;
	int tri1[3][2];
	int tri2[3][2];

	for (int i = 0; i < 128 - 1; i++) {
		for (int j = 0; j < 128 - 1; j++) {
			// Assume a square
			// Triangle 1 is bottom-left, top-left, top-right
			tri1[0][0] = i;
			tri1[0][1] = j + 1;
			tri1[1][0] = i;
			tri1[1][1] = j;
			tri1[2][0] = i + 1;
			tri1[2][1] = j;
			
			// Triangle 2 is top-right, bottom-right, bottom-left
			tri2[0][0] = i + 1;
			tri2[0][1] = j;
			tri2[1][0] = i + 1;
			tri2[1][1] = j + 1;
			tri2[2][0] = i;
			tri2[2][1] = j + 1;
			for (int n = 0; n < 3; n++) {
				for (int k = 0; k < 6; k++) {
					triangles.push_back(vertices[ tri1[n][0] ][ tri1[n][1] ][ k ]);
				}
			}
			for (int n = 0; n < 3; n++) {
				for (int k = 0; k < 6; k++) {
					triangles.push_back(vertices[ tri2[n][0] ][ tri2[n][1] ][ k ]);
					
				}
			}

			terra.noVertices = triangles.size() / 6;
		}
	}


	//-----------------------------
	//----CREATE BUFFERS-----------
	//-----------------------------

	// Generate vertex buffers
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &triangles.front(), GL_STATIC_DRAW);

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
	
	glDrawArrays(GL_TRIANGLES, 0, terra.noVertices);
	
}