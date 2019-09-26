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
		// Terrain constructor
		Water(int resX_ = 1000, int resZ_ = 1000, float scale_ = 0.5, float height_ = 6.0) {
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

			//----------------------------
			// LINK VERTEX DATA TO SHADERS
			//----------------------------
			GLint posAttrib = glGetAttribLocation(shader, "position");
			glEnableVertexAttribArray(posAttrib);
			glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE,
				vertexAtt * sizeof(float), 0);
		}
		~Water() {}

		// Precondition:	Terrain object has been constructed
		// Postcondition:	Terrain is drawn
		void draw(const glm::mat4& Hvw, const glm::mat4& Hcv, const glm::vec3& colour) {
			//------------------------
			// BIND SHADER AND BUFFERS
			//------------------------	
			glUseProgram(shader);
			glBindVertexArray(vao);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

			//--------------------------------
			// SET CAMERA IN MIDDLE OF WATER
			//--------------------------------
			glm::mat4 Hwm = glm::mat4(1.0f);
			Hwm[3] = glm::vec4(-(resX * scale) / 2, -20.0, -(resZ * scale) / 2, 1.0);

			glUniformMatrix4fv(glGetUniformLocation(shader, "Hvw"), 1, GL_FALSE, &Hvw[0][0]);
			glUniformMatrix4fv(glGetUniformLocation(shader, "Hcv"), 1, GL_FALSE, &Hcv[0][0]);
			glUniformMatrix4fv(glGetUniformLocation(shader, "Hwm"), 1, GL_FALSE, &Hwm[0][0]);
			glUniform1f(glGetUniformLocation(shader, "scale"), scale);
   			glUniform3fv(glGetUniformLocation(shader, "colour"), 1, GL_FALSE, &colour[0]);

			//-------------
			// DRAW TERRAIN
			//-------------
			glDrawElements(GL_TRIANGLES, noVertices, GL_UNSIGNED_INT, 0);
			
			// Unbind vertex array
			glBindVertexArray(0);
		}

		// Precondition:	Vertex array, textures and buffers exist.
		// Postcondition:	Vertex array, textures and buffers deleted.
		void cleanup() {
			glDeleteBuffers(1, &vbo);
			glDeleteBuffers(1, &ebo);
			glDeleteVertexArrays(1, &vao);
		}

	private:
		// Store shader program and buffers
		GLuint shader;	// shader program
		GLuint vao;		// vertex array object
		GLuint vbo;		// vertex buffer object
		GLuint ebo;		// element buffer object

		// Store terrain size and resolution
		float scale;		// how much to scale terrain down, if terrain is resX by resZ
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