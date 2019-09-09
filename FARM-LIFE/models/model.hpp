// Data structure for the vertex attributes of a model
struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;	// Texture coordinates
};

// Data structure for the texture of a model
struct Texture {
	GLuint id;
	std::string path;	// path to the texture image
};

///<summary>Model mesh attributes and functions</summary
class Mesh 
{
public:
	// Mesh information
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;
    GLuint VAO;

	// Public functions

	// Constructor
	Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures)
	{
		this->vertices = vertices;
		this->indices = indices;
		this->textures = textures;

		// Initialize the mesh buffer objects/arrays
		initializeMesh();
	}

private:
	// Private render data
    GLuint VBO, EBO;

	// Private functions

	///<summary>
	/// Creates all of the buffer objects and arrays for the model. (no need for a big block in main.cpp)
	///</summary>
	void initializeMesh()
	{
		// create buffers/arrays
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);
		// load data into vertex buffers
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
		// load data into indices buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		// set the vertex attribute pointers
		// vertex Positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		// vertex normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
		// vertex texture coords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

		glBindVertexArray(0);		// unbind vertex array
	}
};

class Model {
public:
	// Public model data
	Texture texture;
	std::vector<Mesh> meshes;

	// Public functions

	// Constructor
	Model(std::string const& path)
	{
		// Load the model using ASSIMP library with the path to the model
	}

	// More functions. We need a function to search the child nodes in ASSIMP and process meshes recursively.
};