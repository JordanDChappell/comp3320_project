/* Water.hpp
 * This class creates water in a region of size resX by resZ, scaled with the scale parameter.
 * The water will be semi-transparent and be reflective. It will move with small wave patterns.
 */

#ifndef ASSIGNMENT_WATER_HPP
#define ASSIGNMENT_WATER_HPP

#include <vector>

namespace water {
    
    class Water {
      public:
		// Water constructor
		Water(int resX_, int resZ_, float scale_, float height_, GLuint refractTex_, GLuint reflectTex_) {
			// Initialise parameters for terrain size and resolution
			resX = resX_;
			resZ = resZ_;
			scale = scale_;
            height = height_;

			// Specify how many vertex attributes there are
			int vertexAtt = 3;

			//--------------------
			// CREATE WATER MESH
			//--------------------
			createMesh(vertexAtt, height);

			//----------------------
			// CREATE SHADER PROGRAM
			//----------------------
			shader = LoadShaders("water/water.vert", "water/water.frag");

			//-------------
			// SET TEXTURES
			//-------------
			int width, height;			// Variables for the width and height of image being loaded 
			glGenTextures(3, &tex[0]);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, tex[0]);
			tex[0] = refractTex_;

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, tex[1]);
			tex[1] = reflectTex_;

			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, tex[2]);
			unsigned char* image = SOIL_load_image("water/dudvmap.png", &width, &height, 0, SOIL_LOAD_RGB);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
				GL_UNSIGNED_BYTE, image);

			// Set the parameters for the grass texture
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			//----------------------------
			// LINK VERTEX DATA TO SHADERS
			//----------------------------
			GLint posAttrib = glGetAttribLocation(shader, "position");
			glEnableVertexAttribArray(posAttrib);
			glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE,
				vertexAtt * sizeof(float), 0);
		}

		~Water() {}

		// Precondition:	Water object has been constructed
		// Postcondition:	Water is drawn
		void draw(const glm::mat4& Hvw, const glm::mat4& Hcv, const glm::vec3& camPos, const glm::vec3& colour, float time, float waveHeight) {
			//------------------------
			// BIND SHADER AND BUFFERS
			//------------------------	
			glUseProgram(shader);
			glBindVertexArray(vao);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, tex[0]);
			glUniform1i(glGetUniformLocation(shader, "refractionTexture"), 0);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, tex[1]);
			glUniform1i(glGetUniformLocation(shader, "reflectionTexture"), 1);

			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, tex[2]);
			glUniform1i(glGetUniformLocation(shader, "dudvMap"), 2);

			//--------------------------------
			// SET CAMERA IN MIDDLE OF WATER
			//--------------------------------
			glm::mat4 Hwm = glm::mat4(1.0f);
			Hwm[3] = glm::vec4(-(resX * scale) / 2, -20.0, -(resZ * scale) / 2, 1.0);

			glUniformMatrix4fv(glGetUniformLocation(shader, "Hvw"), 1, GL_FALSE, &Hvw[0][0]);
			glUniformMatrix4fv(glGetUniformLocation(shader, "Hcv"), 1, GL_FALSE, &Hcv[0][0]);
			glUniformMatrix4fv(glGetUniformLocation(shader, "Hwm"), 1, GL_FALSE, &Hwm[0][0]);
			glUniform3f(glGetUniformLocation(shader, "cameraPosition"), camPos[0], camPos[1], camPos[2]);

			glUniform1f(glGetUniformLocation(shader, "scale"), scale);
   			glUniform3f(glGetUniformLocation(shader, "colour"), colour[0], colour[1], colour[2]);
			glUniform1f(glGetUniformLocation(shader, "time"), time);
			glUniform1f(glGetUniformLocation(shader, "waveHeight"), waveHeight);

			//-----------
			// DRAW WATER
			//-----------
			glDrawElements(GL_TRIANGLES, noVertices, GL_UNSIGNED_INT, 0);
			
			// Unbind vertex array
			glBindVertexArray(0);
		}

		// Precondition:	Has a height value
		// Postcondition:	Returns the height value
		float getHeight() {
			return height - 20;
		}

		// Precondition:	Vertex array, textures and buffers exist.
		// Postcondition:	Vertex array, textures and buffers deleted.
		void cleanup() {
			glDeleteBuffers(1, &vbo);
			glDeleteBuffers(1, &ebo);
			glDeleteVertexArrays(1, &vao);
			glDeleteTextures(3, &tex[0]);
		}

	private:
		// Store shader program and buffers
		GLuint shader;		// shader program
		GLuint vao;			// vertex array object
		GLuint vbo;			// vertex buffer object
		GLuint ebo;			// element buffer object
		GLuint tex[3];		// textures

		// Store terrain size and resolution
		float scale;		// how much to scale water, if water is resX by resZ
		int resX;			// number of vertices wide (x-axis)
		int resZ;			// number of vertices long (z-axis)
		int noVertices;		// number of vertices to draw
        float height;       // height of the water
		
		// Precondition:	vertexAtt is number of vertex attributes, maxHeight is maximum height of terrain
		//					heights is vector of all heights over mesh
		// Postcondition:	Mesh is created and loaded into VAO, VBO, EBO
		void createMesh(int vertexAtt, float height) {
			//---------------------------
			// BIND VERTEX ARRAY OBJECT
			//---------------------------
			glGenVertexArrays(1, &vao);
			glBindVertexArray(vao);

			//----------------
			// CREATE VERTICES
			//----------------
			float* vertices = new float[1000 * 1000 * 3];	// resolution multiplied by number of vertex attributes

			// Loop over grid
			for (int i = 0; i < resX; i++) {
				for (int j = 0; j < resZ; j++) {
					// Position
					vertices[(i * (resZ + resZ * (vertexAtt - 1))) + (j * vertexAtt) + 0] = (float)i;	// X
					vertices[(i * (resZ + resZ * (vertexAtt - 1))) + (j * vertexAtt) + 1] = height;		// Y	
					vertices[(i * (resZ + resZ * (vertexAtt - 1))) + (j * vertexAtt) + 2] = (float)j;	// Z
				}
			}

			//---------
			// BIND VBO 
			//---------
			glGenBuffers(1, &vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
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
			glGenBuffers(1, &ebo);

			// set elements to the triangles that were just made
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangles->size() * sizeof(float), &triangles->front(), GL_STATIC_DRAW);

			std::vector<GLuint>().swap(*triangles);	// free memory from triangles
		}

    };

}

#endif  // ASSIGNMENT_WATER_HPP