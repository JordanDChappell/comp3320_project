/* Terrain.hpp
 * This Terrain class creates a terrain with the specified resolution, scale and maximum height.
 * The Terrain is a mesh of triangles. It uses a BMP image to generate the height of the Terrain. 
 * The Terrain is drawn so that the camera is on top of the center of the Terrain. 
 * This Terrain is textured with lower vertices being given the water texture, slightly higher 
 * vertices the rock texture, and the highest vertices the grass texture.
 */

#ifndef ASSIGNMENT_TERRAIN_HPP
#define ASSIGNMENT_TERRAIN_HPP

#include <vector>
#undef main

namespace terrain {

	class Terrain {
	
	public:
		// Terrain constructor
		Terrain(int resX_ = 1000, int resZ_ = 1000, float scale_ = 0.5, int maxHeight_ = 15) {
			// Initialise parameters for terrain size and resolution
			resX = resX_;
			resZ = resZ_;
			scale = scale_;

			// Specify how many vertex attributes there are
			int vertexAtt = 5;

			// Maximum height of the terrain
			int maxHeight = maxHeight_;

			//----------------
			// READ HEIGHT MAP 
			//----------------
			const std::vector<float>* heights = readHeightMap("terrain/heightmap.bmp");
			
			//--------------------
			// CREATE TERRAIN MESH
			//--------------------
			createMesh(vertexAtt, maxHeight, heights);

			//----------------------
			// CREATE SHADER PROGRAM
			//----------------------
			terraShader = LoadShaders("terrain/terrain.vert", "terrain/terrain.frag");

			//----------------
			// CREATE TEXTURES
			//----------------
			loadTextures();

			//----------------------------
			// LINK VERTEX DATA TO SHADERS
			//----------------------------
			GLint posAttrib = glGetAttribLocation(terraShader, "position");
			glEnableVertexAttribArray(posAttrib);
			glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE,
				vertexAtt * sizeof(float), 0);

			GLint texAttrib = glGetAttribLocation(terraShader, "texCoord");
			glEnableVertexAttribArray(texAttrib);
			glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE,
				vertexAtt * sizeof(float), (void*)(3 * sizeof(float)));
		}
		~Terrain() {}

		// Precondition:	Terrain object has been constructed
		// Postcondition:	Terrain is drawn
		void draw(const glm::mat4& Hvw, const glm::mat4& Hcv) {
			//------------------------
			// BIND SHADER AND BUFFERS
			//------------------------	
			glUseProgram(terraShader);
			glBindVertexArray(terraVao);
			glBindBuffer(GL_ARRAY_BUFFER, terraVbo);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terraEbo);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, terraTex[0]);
			glUniform1i(glGetUniformLocation(terraShader, "texGrass"), 0);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, terraTex[1]);
			glUniform1i(glGetUniformLocation(terraShader, "texRock"), 1);

			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, terraTex[2]);
			glUniform1i(glGetUniformLocation(terraShader, "texWater"), 2);

			//--------------------------------
			// SET CAMERA IN MIDDLE OF TERRAIN
			//--------------------------------
			glm::mat4 Hwm = glm::mat4(1.0f);
			Hwm[3] = glm::vec4(-(resX * scale) / 2, -20.0, -(resZ * scale) / 2, 1.0);

			glUniformMatrix4fv(glGetUniformLocation(terraShader, "Hvw"), 1, GL_FALSE, &Hvw[0][0]);
			glUniformMatrix4fv(glGetUniformLocation(terraShader, "Hcv"), 1, GL_FALSE, &Hcv[0][0]);
			glUniformMatrix4fv(glGetUniformLocation(terraShader, "Hwm"), 1, GL_FALSE, &Hwm[0][0]);
			glUniform1f(glGetUniformLocation(terraShader, "scale"), scale);

			//-------------
			// DRAW TERRAIN
			//-------------
			glDrawElements(GL_TRIANGLES, noVertices, GL_UNSIGNED_INT, 0);
			
			// Unbind texture and vertex array
			glBindVertexArray(0);
			glActiveTexture(GL_TEXTURE0);
		}

		// Precondition:	Vertex array, textures and buffers exist.
		// Postcondition:	Vertex array, textures and buffers deleted.
		void cleanup() {
			glDeleteBuffers(1, &terraVbo);
			glDeleteBuffers(1, &terraEbo);
			glDeleteVertexArrays(1, &terraVao);
			glDeleteTextures(3, &terraTex[0]);
		}

	private:
		// Store shader program and buffers
		GLuint terraShader;		// shader program
		GLuint terraVao;		// vertex array object
		GLuint terraVbo;		// vertex buffer object
		GLuint terraEbo;		// element buffer object
		GLuint terraTex[3];		// textures

		// Store terrain size and resolution
		float scale;		// how much to scale terrain down, if terrain is resX by resZ
		int resX;			// number of vertices wide (x-axis)
		int resZ;			// number of vertices long (z-axis)
		int noVertices;		// number of vertices to draw
		
		// Precondition:	vertexAtt is number of vertex attributes, maxHeight is maximum height of terrain
		//					heights is vector of all heights over mesh
		// Postcondition:	Mesh is created and loaded into VAO, VBO, EBO
		void createMesh(int vertexAtt, int maxHeight, const std::vector<float> * heights) {
			//---------------------------
			// BIND VERTEX ARRAY OBJECT
			//---------------------------
			glGenVertexArrays(1, &terraVao);
			glBindVertexArray(terraVao);

			//----------------
			// CREATE VERTICES
			//----------------
			float* vertices = new float[1000 * 1000 * 5];	// resolution multiplied by number of vertex attributes

			// Loop over grid
			for (int i = 0; i < resX; i++) {
				for (int j = 0; j < resZ; j++) {
					// Position
					vertices[(i * (resZ + resZ * (vertexAtt - 1))) + (j * vertexAtt) + 0] = (float)i;	// X
					vertices[(i * (resZ + resZ * (vertexAtt - 1))) + (j * vertexAtt) + 1] =
						(heights->at(j * (resX)+i)) * maxHeight;										// Y	
					vertices[(i * (resZ + resZ * (vertexAtt - 1))) + (j * vertexAtt) + 2] = (float)j;	// Z

					// Texture Coordinates
					vertices[(i * (resZ + resZ * (vertexAtt - 1))) + (j * vertexAtt) + 3] = i % 2 == 0 ? 1.0f : 0.0f;	// TEX COORDINATE X
					vertices[(i * (resZ + resZ * (vertexAtt - 1))) + (j * vertexAtt) + 4] = j % 2 == 0 ? 1.0f : 0.0f;	// TEX COORDINATE Y
				}
			}

			//---------
			// BIND VBO 
			//---------
			glGenBuffers(1, &terraVbo);
			glBindBuffer(GL_ARRAY_BUFFER, terraVbo);
			glBufferData(GL_ARRAY_BUFFER, resX * resZ * vertexAtt * sizeof(float), &vertices[0], GL_STATIC_DRAW);
			delete[] vertices;	// free memory from vertices

			//-----------------
			// CREATE TRIANGLES
			//-----------------
			std::vector<GLuint>* triangles = new std::vector<GLuint>();		// Vector that will store all the triangles

			// Loop over grid except last row and column to get triangles
			for (int i = 0; i < resX - 1; i++) {
				for (int j = 0; j < resZ - 1; j++) {
					// Add vertices of triangles (two triangles that make a square across and up to current vertex)

					// FIRST TRIANGLE
					triangles->push_back(((i * (resZ + resZ * (vertexAtt - 1))) + ((j + 1) * vertexAtt)) / vertexAtt);			// bottom-left	(i, j + 1)
					triangles->push_back(((i * (resZ + resZ * (vertexAtt - 1))) + (j * vertexAtt)) / vertexAtt);				// top-left		(i, j)
					triangles->push_back((((i + 1) * (resZ + resZ * (vertexAtt - 1))) + (j * vertexAtt)) / vertexAtt);			// top-right	(i + 1, j) 

					// SECOND TRIANGLE
					triangles->push_back((((i + 1) * (resZ + resZ * (vertexAtt - 1))) + (j * vertexAtt)) / vertexAtt);			// top-right	(i + 1, j)
					triangles->push_back((((i + 1) * (resZ + resZ * (vertexAtt - 1))) + ((j + 1) * vertexAtt)) / vertexAtt);	// bottom-right (i + 1, j + 1)
					triangles->push_back(((i * (resZ + resZ * (vertexAtt - 1))) + ((j + 1) * vertexAtt)) / vertexAtt);			// bottom-left	(i, j + 1)

					// Set the number of vertices for drawing later
					noVertices = triangles->size();
				}
			}

			//---------
			// BIND EBO
			//---------
			glGenBuffers(1, &terraEbo);

			// set elements to the triangles that were just made
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terraEbo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangles->size() * sizeof(float), &triangles->front(), GL_STATIC_DRAW);

			std::vector<GLuint>().swap(*triangles);	// free memory from triangles
		}

		// Precondition:	heightMap is the path to a .BMP file which is the height map image to be loaded
		//					heightMap is the same dimension as resX x resZ 
		// Postcondition:	returns a vector of heights for each vertex
		const std::vector<float>* readHeightMap(const std::string heightMap) {
			// Load in the height map
			SDL_Surface* img = SDL_LoadBMP(heightMap.c_str());

			// Make the vector to store the height of each vertex
			std::vector<float>* heights = new std::vector<float>;

			// Loop over all vertices in image and add the white percentage to heights
			for (int i = 0 ; i < img->h ; i++) {
				for (int j = 0 ; j < img->w ; j++) {
					// Get the pixel at the right coordinate
					Uint32 pixel = ((Uint32*)img->pixels)[i * img->pitch / 4 + j];

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
		void loadTextures() {
			int width, height;			// Variables for the width and height of image being loaded 
			glGenTextures(3, &terraTex[0]);		// Generate three texture names

			//--------------------
			// CREATE GRASS TEXTURE
			//--------------------
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, terraTex[0]);
			unsigned char* image = SOIL_load_image("terrain/grass.jpg", &width, &height, 0, SOIL_LOAD_RGB);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
				GL_UNSIGNED_BYTE, image);

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
			glBindTexture(GL_TEXTURE_2D, terraTex[1]);
			image = SOIL_load_image("terrain/rock.jpg", &width, &height, 0, SOIL_LOAD_RGB);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
				GL_UNSIGNED_BYTE, image);

			// Set the parameters for the rock texture
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glGenerateMipmap(GL_TEXTURE_2D);

			//---------------------
			// CREATE WATER TEXTURE
			//---------------------
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, terraTex[2]);
			image = SOIL_load_image("terrain/water.jpg", &width, &height, 0, SOIL_LOAD_RGB);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
				GL_UNSIGNED_BYTE, image);

			// Set the parameters for the water texture
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
	};
}

#endif		// ASSIGNMENT_TERRAIN_HPP