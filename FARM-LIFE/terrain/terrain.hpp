/* Terrain.hpp
 * This Terrain class creates a terrain with the specified resolution, scale and maximum height.
 * The Terrain is a mesh of triangles. It uses a BMP image to generate the height of the Terrain. 
 * The Terrain is drawn so that the camera is on top of the center of the Terrain. 
 * This Terrain is textured with lower vertices the rock texture, and the highest vertices the grass texture.
 */

#ifndef ASSIGNMENT_TERRAIN_HPP
#define ASSIGNMENT_TERRAIN_HPP

#undef main

namespace terrain
{

class Terrain
{
public:
	// Terrain constructor
	Terrain(int resX_ = 1000, int resZ_ = 1000, float scale_ = 0.5, int maxHeight_ = 30, float yOffset_ = -20.0f, float waterHeight_ = 12)
	{
		// Initialise parameters for terrain size and resolution
		resX = resX_;
		resZ = resZ_;
		scale = scale_;
		yOffset = yOffset_;
		waterHeight = waterHeight_;

		// Specify how many vertex attributes there are
		int vertexAtt = 3;

		// Maximum height of the terrain
		int maxHeight = maxHeight_;

		// Initialize the terraVertices matrix - used to find height of terrain at coordinates
		terraVertices = new float *[resX_];
		for (int i = 0; i < resX_; i++)
		{
			terraVertices[i] = new float[resZ_];
		}

		// Specify height of where the grass starts
		grassHeight = maxHeight / 1.7;

		//----------------
		// READ HEIGHT MAP
		//----------------
		const std::vector<float> *heights = readHeightMap("terrain/heightmap.bmp");

		//--------------------
		// CREATE TERRAIN MESH
		//--------------------
		createMesh(vertexAtt, maxHeight, heights);

		//----------------------
		// CREATE SHADER PROGRAM
		//----------------------
		shader = LoadShaders("terrain/terrain.vert", "terrain/terrain.frag");
		grassShader = LoadShaders("terrain/grass.vert", "terrain/grass.frag");
		AddGeometryShader(grassShader, "terrain/grass.geom");

		//----------------
		// CREATE TEXTURES
		//----------------
		loadTextures();

		//----------------------------
		// LINK VERTEX DATA TO SHADERS
		//----------------------------
		GLint posAttrib = glGetAttribLocation(shader, "position");
		glEnableVertexAttribArray(posAttrib);
		glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE,
							  vertexAtt * sizeof(float), 0);

		// Initialise sound
		sound = audio::Source();
		sound.setLooping(true);
	}

	// Destructor
	~Terrain() {}

	// Precondition:	file is an audio file in wav format
	// Postcondition:	sound is played from the source on this model
	void playSound(const char *file)
	{
		GLuint buffer = audio::loadAudio(file);
		sound.play(buffer);
	}

	// Precondition:	Terrain object has been constructed
	// Postcondition:	Terrain is drawn
	void draw(const glm::mat4 &Hvw, const glm::mat4 &Hcv, const glm::vec4 &clippingPlane, glm::vec3 cameraPosition, glm::vec3 lightPosition, glm::vec3 lightColour, float time)
	{
		//------------------------
		// BIND SHADER AND BUFFERS
		//------------------------
		glUseProgram(shader);
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex[0]);
		glUniform1i(glGetUniformLocation(shader, "texGrass"), 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, tex[1]);
		glUniform1i(glGetUniformLocation(shader, "texRock"), 1);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, tex[2]);
		glUniform1i(glGetUniformLocation(shader, "texSand"), 2);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, tex[3]);
		glUniform1i(glGetUniformLocation(shader, "normalMap"), 3);

		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, tex[4]);
		glUniform1i(glGetUniformLocation(shader, "waterNormalMap"), 4);

		//--------------------------------
		// SET CAMERA IN MIDDLE OF TERRAIN
		//--------------------------------
		glm::mat4 Hwm = glm::mat4(1.0f);
		Hwm[3] = glm::vec4(-(resX * scale) / 2, yOffset, -(resZ * scale) / 2, 1.0);

		glUniformMatrix4fv(glGetUniformLocation(shader, "Hvw"), 1, GL_FALSE, &Hvw[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(shader, "Hcv"), 1, GL_FALSE, &Hcv[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(shader, "Hwm"), 1, GL_FALSE, &Hwm[0][0]);

		// Set uniforms
		glUniform1f(glGetUniformLocation(shader, "scale"), scale);
		glUniform1f(glGetUniformLocation(shader, "grassHeight"), grassHeight);
		glUniform1f(glGetUniformLocation(shader, "resolutionX"), resX);
		glUniform1f(glGetUniformLocation(shader, "resolutionZ"), resZ);
		glUniform1f(glGetUniformLocation(shader, "time"), time);
		glUniform1f(glGetUniformLocation(shader, "waterHeight"), waterHeight);

		glUniform4f(glGetUniformLocation(shader, "clippingPlane"), clippingPlane[0], clippingPlane[1], clippingPlane[2], clippingPlane[3]);

		glUniform3f(glGetUniformLocation(shader, "cameraPosition"), cameraPosition[0], cameraPosition[1], cameraPosition[2]);
		glUniform3f(glGetUniformLocation(shader, "lightPosition"), lightPosition[0], lightPosition[1], lightPosition[2]);
		glUniform3f(glGetUniformLocation(shader, "lightColour"), lightColour[0], lightColour[1], lightColour[2]);

		//-------------
		// DRAW TERRAIN
		//-------------
		glDrawElements(GL_TRIANGLES, noVertices, GL_UNSIGNED_INT, 0);

		// Update sound position
		sound.setPosition(cameraPosition);

		// ----------
		// DRAW GRASS
		//-----------
		// Buffers and shader
		glUseProgram(grassShader); // switch to the grass shader
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		// Uniforms
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, grassTex[0]);
		glUniform1i(glGetUniformLocation(grassShader, "normalMap"), 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, grassTex[1]);
		glUniform1i(glGetUniformLocation(grassShader, "grassTex1"), 1);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, grassTex[2]);
		glUniform1i(glGetUniformLocation(grassShader, "grassTex2"), 2);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, grassTex[3]);
		glUniform1i(glGetUniformLocation(grassShader, "grassTex3"), 3);

		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, grassTex[4]);
		glUniform1i(glGetUniformLocation(grassShader, "grassTex4"), 4);

		glUniformMatrix4fv(glGetUniformLocation(grassShader, "Hvw"), 1, GL_FALSE, &Hvw[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(grassShader, "Hcv"), 1, GL_FALSE, &Hcv[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(grassShader, "Hwm"), 1, GL_FALSE, &Hwm[0][0]);

		glUniform4f(glGetUniformLocation(grassShader, "clippingPlane"), clippingPlane[0], clippingPlane[1], clippingPlane[2], clippingPlane[3]);
		glUniform1f(glGetUniformLocation(grassShader, "scale"), scale);
		glUniform1f(glGetUniformLocation(grassShader, "resX"), resX);
		glUniform1f(glGetUniformLocation(grassShader, "resZ"), resZ);
		glUniform1f(glGetUniformLocation(grassShader, "grassHeight"), grassHeight);
		glUniform1f(glGetUniformLocation(grassShader, "grassScale"), 3.0);

		// Draw grass
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDrawArrays(GL_POINTS, 0, (resX * resZ)); // draw the points
		glDisable(GL_BLEND);
		// Unbind texture and vertex array
		glBindVertexArray(0);
		glActiveTexture(GL_TEXTURE0);
	}

	// Precondition:	Vertex array, textures and buffers exist.
	// Postcondition:	Vertex array, textures and buffers deleted.
	void cleanup()
	{
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &ebo);
		glDeleteVertexArrays(1, &vao);
		glDeleteTextures(5, &tex[0]);
		glDeleteTextures(5, &grassTex[0]);
		sound.cleanup();
	}

	// Precondition:	x is in [0, resX], y is in [0, resZ]
	// Postcondition:	Returns the terrain height at the given (x,y) coordinate
	float getHeightAt(int x, int y)
	{
		return terraVertices[x][y];
	}

private:
	// Store shader program and buffers
	GLuint shader;		// shader program
	GLuint grassShader; // grass shader program
	GLuint vao;			// vertex array object
	GLuint vbo;			// vertex buffer object
	GLuint ebo;			// element buffer object
	GLuint tex[5];		// textures
	GLuint grassTex[5]; // grass textures

	// Store terrain size and resolution
	float scale;		 // how much to scale terrain down, if terrain is resX by resZ
	float yOffset;		 // how much to offset the terrain by in the y direction (from 0.0f)
	int resX;			 // number of vertices wide (x-axis)
	int resZ;			 // number of vertices long (z-axis)
	int noVertices;		 // number of vertices to draw
	float waterHeight;   // height of the water
	audio::Source sound; // source for the terrain ambient sound

	// Store terrain (x,y,z) vector for height detection
	float **terraVertices;

	float grassHeight; // height that the grass starts to grow

	// Precondition:	vertexAtt is number of vertex attributes, maxHeight is maximum height of terrain
	//					heights is vector of all heights over mesh
	// Postcondition:	Mesh is created and loaded into VAO, VBO, EBO
	void createMesh(int vertexAtt, int maxHeight, const std::vector<float> *heights)
	{
		//---------------------------
		// BIND VERTEX ARRAY OBJECT
		//---------------------------
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		//----------------
		// CREATE VERTICES
		//----------------
		float *vertices = new float[resX * resZ * vertexAtt]; // resolution multiplied by number of vertex attributes

		// Loop over grid
		for (int i = 0; i < resX; i++)
		{
			for (int j = 0; j < resZ; j++)
			{
				float currentHeight = (heights->at(j * (resX) + i)) * maxHeight; // find the current height of the terrain at the x,z pos
				// Position
				vertices[(i * (resZ + resZ * (vertexAtt - 1))) + (j * vertexAtt) + 0] = (float)i;	  // X
				vertices[(i * (resZ + resZ * (vertexAtt - 1))) + (j * vertexAtt) + 1] = currentHeight; // Y
				vertices[(i * (resZ + resZ * (vertexAtt - 1))) + (j * vertexAtt) + 2] = (float)j;	  // Z

				// Vertices to use in collisions etc.
				terraVertices[i][j] = currentHeight;
			}
		}

		//---------
		// BIND VBO
		//---------
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, resX * resZ * vertexAtt * sizeof(float), &vertices[0], GL_STATIC_DRAW);
		delete[] vertices; // free memory from vertices

		//-----------------
		// CREATE TRIANGLES
		//-----------------
		std::vector<GLuint> *triangles = new std::vector<GLuint>(); // Vector that will store all the triangles

		// Loop over grid except last row and column to get triangles
		for (int i = 0; i < resX - 1; i++)
		{
			for (int j = 0; j < resZ - 1; j++)
			{
				// Add vertices of triangles (two triangles that make a square across and up to current vertex)

				// FIRST TRIANGLE
				triangles->push_back(((i * (resZ + resZ * (vertexAtt - 1))) + ((j + 1) * vertexAtt)) / vertexAtt); // bottom-left	(i, j + 1)
				triangles->push_back(((i * (resZ + resZ * (vertexAtt - 1))) + (j * vertexAtt)) / vertexAtt);	   // top-left		(i, j)
				triangles->push_back((((i + 1) * (resZ + resZ * (vertexAtt - 1))) + (j * vertexAtt)) / vertexAtt); // top-right	(i + 1, j)

				// SECOND TRIANGLE
				triangles->push_back((((i + 1) * (resZ + resZ * (vertexAtt - 1))) + (j * vertexAtt)) / vertexAtt);		 // top-right	(i + 1, j)
				triangles->push_back((((i + 1) * (resZ + resZ * (vertexAtt - 1))) + ((j + 1) * vertexAtt)) / vertexAtt); // bottom-right (i + 1, j + 1)
				triangles->push_back(((i * (resZ + resZ * (vertexAtt - 1))) + ((j + 1) * vertexAtt)) / vertexAtt);		 // bottom-left	(i, j + 1)

				// Set the number of vertices for drawing later
				noVertices = triangles->size();
			}
		}

		//---------
		// BIND EBO
		//---------
		glGenBuffers(1, &ebo);

		// set elements to the triangles that were just made
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangles->size() * sizeof(float), &triangles->front(), GL_STATIC_DRAW);

		std::vector<GLuint>().swap(*triangles); // free memory from triangles
	}

	// Precondition:	heightMap is the path to a .BMP file which is the height map image to be loaded
	//					heightMap is the same dimension as resX x resZ
	// Postcondition:	returns a vector of heights for each vertex
	const std::vector<float> *readHeightMap(const std::string heightMap)
	{
		// Load in the height map
		SDL_Surface *img = SDL_LoadBMP(heightMap.c_str());

		// Make the vector to store the height of each vertex
		std::vector<float> *heights = new std::vector<float>;

		// Loop over all vertices in image and add the white percentage to heights
		for (int i = 0; i < img->h; i++)
		{
			for (int j = 0; j < img->w; j++)
			{
				// Get the pixel at the right coordinate
				Uint32 pixel = ((Uint32 *)img->pixels)[i * img->pitch / 4 + j];

				// Get red, green and blue values for this pixel
				Uint8 r, g, b;
				SDL_GetRGB(pixel, img->format, &r, &g, &b);

				// Get a number between 0 and 1 relating to how white/black the pixel is
				heights->push_back((float)r / 255.0);
			}
		}

		// Return the vector of vertex heights
		return heights;
	}

	// Precondition:	Textures have not been set. This is called in creation of Terrain.
	// Postcondition:	Terrain textures are created and bound for grass, rock and water.
	void loadTextures()
	{
		int width, height;		   // Variables for the width and height of image being loaded
		glGenTextures(5, &tex[0]); // Generate three texture names

		//--------------------
		// CREATE GRASS TEXTURE
		//--------------------
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex[0]);
		unsigned char *image = SOIL_load_image("terrain/dirt.png", &width, &height, 0, SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

		// Set the parameters for the grass texture
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glGenerateMipmap(GL_TEXTURE_2D);

		//--------------------
		// CREATE ROCK TEXTURE
		//--------------------
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, tex[1]);
		image = SOIL_load_image("terrain/rock.png", &width, &height, 0, SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

		// Set the parameters for the rock texture
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glGenerateMipmap(GL_TEXTURE_2D);

		//--------------------
		// CREATE SAND TEXTURE
		//--------------------
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, tex[2]);
		image = SOIL_load_image("terrain/sand.png", &width, &height, 0, SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

		// Set the parameters for the rock texture
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glGenerateMipmap(GL_TEXTURE_2D);

		//------------------
		// CREATE NORMAL MAP
		//------------------
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, tex[3]);
		image = SOIL_load_image("terrain/normalmap.png", &width, &height, 0, SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

		// Set the parameters for the rock texture
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		//------------------------
		// CREATE WATER NORMAL MAP
		//------------------------
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, tex[4]);
		image = SOIL_load_image("water/normalmap.png", &width, &height, 0, SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

		// Set the parameters for the rock texture
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		//---------------
		// GRASS TEXTURES
		//---------------

		glGenTextures(5, &grassTex[0]); // Generate texture

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, grassTex[0]);

		image = SOIL_load_image("terrain/normalmap.png", &width, &height, 0, SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

		// Set the parameters for the normal texture
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// GRASS CARD TEXTURES
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, grassTex[1]);

		image = SOIL_load_image("terrain/grass/grass1.png", &width, &height, 0, SOIL_LOAD_RGBA);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

		// Set the parameters for the normal texture
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glGenerateMipmap(GL_TEXTURE_2D);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, grassTex[2]);

		image = SOIL_load_image("terrain/grass/grass2.png", &width, &height, 0, SOIL_LOAD_RGBA);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

		// Set the parameters for the normal texture
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glGenerateMipmap(GL_TEXTURE_2D);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, grassTex[3]);

		image = SOIL_load_image("terrain/grass/grass3.png", &width, &height, 0, SOIL_LOAD_RGBA);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

		// Set the parameters for the normal texture
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glGenerateMipmap(GL_TEXTURE_2D);

		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, grassTex[4]);

		image = SOIL_load_image("terrain/grass/grass4.png", &width, &height, 0, SOIL_LOAD_RGBA);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

		// Set the parameters for the normal texture
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	// Precondition:	Vertices is populated, contains point (i,j), vertex attributes
	//					represents how many vertex attributes per vertex.
	// Postcondition:	Given a mesh of points, returns the (i,j)th point
	glm::vec3 getVertex(int i, int j, float *vertices, int vertexAtt)
	{
		glm::vec3 vertex;
		vertex.x = vertices[(i * (resZ + resZ * (vertexAtt - 1))) + (j * vertexAtt) + 0]; // X
		vertex.y = vertices[(i * (resZ + resZ * (vertexAtt - 1))) + (j * vertexAtt) + 1]; // Y
		vertex.z = vertices[(i * (resZ + resZ * (vertexAtt - 1))) + (j * vertexAtt) + 2]; // Z
		return vertex;
	}
};
} // namespace terrain

#endif // ASSIGNMENT_TERRAIN_HPP