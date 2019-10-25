#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include "../audio/audio.hpp"
#include "lights/lights.hpp"

#ifndef A1_MODEL_HPP
#define A1_MODEL_HPP

namespace model {
	GLuint TextureFromFile(const char* path, const std::string& directory);

	// Data structure for the vertex attributes of a model
	struct Vertex {
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoords;	// Texture coordinates
		// tangent
		glm::vec3 Tangent;
		// bitangent
		glm::vec3 Bitangent;
	};

	// Data structure for the texture of a model
	struct Texture {
		GLuint id;
		std::string type;	// type of texture image
		std::string path;	// path to the texture image
	};

	// Data structure for material only models
	struct Material {
		glm::vec3 Diffuse;
		glm::vec3 Specular;
		glm::vec3 Ambient;
		float Shininess;
	};

	// Data structure for a models hitbox
	struct HitBox {
		glm::vec3 origin;
		glm::vec3 size;
	};

	///<summary>Model mesh attributes and functions</summary
	class Mesh
	{
	public:
		// Mesh information
		std::vector<Vertex> vertices;
		std::vector<GLuint> indices;
		std::vector<Texture> textures;		// need multiple textures for certain meshes, should keep all in memory rather than loading
		std::string meshName;
		
		// Mesh local minimum and maximum vertices values, can be useful for transforms (find center of mesh for a transform)
		glm::vec3 minVertices;
		glm::vec3 maxVertices;
		glm::vec3 centerOfMesh;

		// Animation transform values
		float maxRotation;
		float minRotation;
		float currentRotation;
		float angleOfRotation;
		glm::vec3 axisOfRotation;

		GLuint VAO;

		// Public functions

		// Constructor
		Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures, std::string meshName, glm::vec3 minVertices, glm::vec3 maxVertices)
		{
			// Initialize input parameters
			this->vertices = vertices;
			this->indices = indices;
			this->textures = textures;
			this->minVertices = minVertices;
			this->maxVertices = maxVertices;
			this->meshName = meshName;

			// Initialize the default rotation transform (no rotation)
			this->maxRotation = 0.0f;
			this->minRotation = 0.0f;
			this->currentRotation = 0.0f;
			this->angleOfRotation = 0.0f;
			this->axisOfRotation = glm::vec3(0.0f, 1.0f, 0.0f);

			// Find center of the mesh
			this->centerOfMesh = glm::vec3(((minVertices.x + maxVertices.x) / 2.0f), ((minVertices.y + maxVertices.y) / 2.0f), ((minVertices.z + maxVertices.z) / 2.0f));
			
			// Initialize the mesh buffer objects/arrays
			initializeMesh();
		}

		///<summary>
		/// Render the mesh in opengl window.
		/// The mesh may have any number of diffuse and specular textures. We must loop over each texture and bind it to our
		/// mesh shader appropriately.
		/// Source: learnopengl.com
		///</summary>
		void Draw(GLuint shader, glm::mat4 view, glm::mat4 projection, glm::mat4 model, glm::vec3 position, glm::vec4 clippingPlane, glm::vec3 CamPos, glm::vec3 Forward)
		{
			unsigned int diffuseNr = 1;	// number to assign to diffuse texture
			unsigned int specularNr = 1;	// number to assign to specular texture
			unsigned int normalNr = 1;
			unsigned int heightNr = 1;
			for (unsigned int i = 0; i < this->textures.size(); i++)
			{
				glActiveTexture(GL_TEXTURE0 + i); // activate proper texture unit before binding
				// retrieve texture number (using our counters initialized above)
				std::string number;
				std::string name = textures[i].type;
				std::string mater = "material.";
				if (name == "texture_diffuse")
					number = std::to_string(diffuseNr++);
				else if (name == "texture_specular")
					number = std::to_string(specularNr++);
				else if (name == "texture_normal")
					number = std::to_string(normalNr++); // transfer unsigned int to stream
				else if (name == "texture_height")
					number = std::to_string(heightNr++); // transfer unsigned int to stream

				// Set the shader sampler to the current texture and bind it
				glUniform1i(glGetUniformLocation(shader, (mater + name + number).c_str()), i); //needs to be material.2DSampler *********Error here I think
				glBindTexture(GL_TEXTURE_2D, textures[i].id);
				//std::cout << " " << mater << name << number << " "; //--Indicates we only have diffuse maps for our models
			}
			
			//******************************************************************************************************************************************
			
			lights::light lighting(shader);
			shader = lighting.setup(CamPos, Forward);
			// Set material properties
			
			glUniform1f(glGetUniformLocation(shader, "material.shininess"), 5.0f);


			//*******************************************************************************************
			// draw mesh - apply view transformations from the camera
			glBindVertexArray(VAO);
			glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, &view[0][0]);
			glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, &projection[0][0]);


			// Apply the movement transform to the model
			model = glm::translate(model, position);

			// Rotation animation bounds
			if (currentRotation > maxRotation)
			{
				angleOfRotation = -angleOfRotation;
			}
			else if (currentRotation < minRotation)
			{
				angleOfRotation = -angleOfRotation;
			}
				
			// Apply rotation transform
			currentRotation += angleOfRotation;
			model = glm::translate(model, centerOfMesh);
			model = glm::rotate(model, currentRotation, axisOfRotation);
			model = glm::translate(model, -centerOfMesh);

			glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, &model[0][0]);
			// Add the clipping plane to the shader to clip parts of the scene if needed
			glUniform4f(glGetUniformLocation(shader, "clippingPlane"), clippingPlane[0], clippingPlane[1], clippingPlane[2], clippingPlane[3]);
			// Draw the model
			glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

			// cleanup
			glBindVertexArray(0);
			glActiveTexture(GL_TEXTURE0);
		}

		///<summary>Set a rotation transform loop for the mesh, takes a minimum angle, maximum angle,
		///			an incremental angle between frames and an axis of rotation usually about the Y axis.
		///</summary>
		void SetRotationTransformLoop(float minRotation, float maxRotation, float angle, glm::vec3 axis)
		{
			this->angleOfRotation = angle;
			this->axisOfRotation = axis;
			this->minRotation = minRotation;
			this->maxRotation = maxRotation;
		}

	private:
		// Private render data
		GLuint VBO, EBO;

		// Private functions

		///<summary>
		/// Creates all of the buffer objects and arrays for the model. (no need for a big block in main.cpp)
		/// Source: Learnopengl.com
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
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

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
			// vertex tangent
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
			// vertex bitangent
			glEnableVertexAttribArray(4);
			glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
			// cleanup
			glBindVertexArray(0);
		}
	};

	class Model {
	public:
		// Public model data
		//Texture texture;
		std::vector<Texture> loadedTextures;
		std::vector<Mesh> meshes;
		std::string directory;
		glm::vec3 position = glm::vec3(0, 0, 0);
		HitBox hitBox;
		std::vector<std::string> nodeNames;

		// Public functions

		// Constructor
		Model(std::string const& path)
		{
			// Load the model using ASSIMP library with the path to the model
			loadModel(path);
			// Initialise the source
			sound = audio::Source();
		}

		// Precondition:	file is an audio file in wav format. Reference distance is the
		//					radius the sound is played at full volume at
		// Postcondition:	sound is played from the source on this model
		void playSound(const char* file, bool loop, float reference_distance) {
			GLuint buffer = audio::loadAudio(file);
			sound.play(buffer);
			sound.setLooping(loop);
			sound.setReferenceDistance(reference_distance);
		}

		///<summary>
		/// Draw the model to the open gl window.
		/// Simply loop over the meshes in our vector and call the draw function of each.
		///</summary>
		void Draw(GLuint shader, glm::mat4 view, glm::mat4 projection, glm::mat4 model, glm::vec4 clippingPlane, glm::vec3 CamPos, glm::vec3 Forward)
		{
			// Draw the model using it's shader
			glUseProgram(shader);	// use the shader before drawing all the meshes.
			for (unsigned int i = 0; i < meshes.size(); i++)
			{
				meshes[i].Draw(shader, view, projection, model, position, clippingPlane, CamPos, Forward);
			}

			// Update the sound source position
			sound.setPosition(position);
		}

		///<summary>
		/// Move the model to a new position (x,y,z)
		///</summary>
		void MoveTo(glm::vec3 coordinates) 
		{
			position = position + coordinates;
			hitBox.origin = hitBox.origin + coordinates;
		}

		///<summary>
		/// Sets a Rotation animation loop on a specific mesh in the model, takes a meshName (corresponding to a value in the model.obj file -> blender mesh layer name)
		/// takes a minimum/maximum rotation angle (before the loop reverses), takes an incrementing angle, and an axis to rotate about.
		///</summary>
		void SetRotationAnimationLoop(std::string meshName, float minRotation, float maxRotation, float angleOfRotation, glm::vec3 axisOfRotation) 
		{
			for (int i = 0; i < meshes.size(); i++) 
			{
				if (meshName.compare(meshes[i].meshName) == 0)
				{
					meshes[i].SetRotationTransformLoop(minRotation, maxRotation, angleOfRotation, axisOfRotation);
				}
			}
		}

	private:
		// Private model data
		glm::vec3 maxVertices;	// keeps a record of the models overall max(x,y,z) coordinates
		glm::vec3 minVertices;	// as above for the minimum vertices
		bool verticesSet = false;	// flag that enables the vertices to be initialized on first loop over the mesh
		audio::Source sound;
		aiNode rootNode;		// stores the root node of the model

		///<summary>
		/// Load a model using assimp library.
		///</summary>
		void loadModel(std::string const& path)
		{
			Assimp::Importer importer;
			// read file via ASSIMP
			const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
			// check for errors
			if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
			{
				std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
				return;
			}
			// retrieve the directory path of the filepath
			directory = path.substr(0, path.find_last_of('/'));

			// process ASSIMP's root node recursively
			processNode(scene->mRootNode, scene);

			// initialize the models hitbox, origin is the minimum vertex in each axis
			hitBox.origin = glm::vec3((maxVertices.x + minVertices.x) / 2,
				(maxVertices.y + minVertices.y) / 2, (maxVertices.z + minVertices.z) / 2);
			hitBox.size = (maxVertices - minVertices) / glm::vec3(2, 2, 2);	// size is just the max - min
		}

		///<summary>
		/// Recursive function to process all children nodes of a model.
		/// Source: learnopengl.com
		///</summary>
		void processNode(aiNode* node, const aiScene* scene)
		{
			//std::cout << "Process Node: " << node->mName.C_Str() << std::endl;
			// process each mesh located at the current node
			for (unsigned int i = 0; i < node->mNumMeshes; i++)
			{
				aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
				Mesh madeMesh = processMesh(mesh, scene, node->mName.C_Str());
				meshes.push_back(madeMesh);
			}
			// after processing meshes, check for children nodes of thc current node, recursively process the children.
			for (unsigned int i = 0; i < node->mNumChildren; i++)
			{
				processNode(node->mChildren[i], scene);
			}
		}

		Mesh processMesh(aiMesh* mesh, const aiScene* scene, std::string meshName)
		{
			// data to fill
			std::vector<Vertex> vertices;
			std::vector<GLuint> indices;
			std::vector<Texture> textures;
			glm::vec3 localMinVertices = glm::vec3(0.0f, 0.0f, 0.0f);	//used to keep track of each meshes local miniumum/maximum vertices for local center calculation in transforms.
			glm::vec3 localMaxVertices = glm::vec3(0.0f, 0.0f, 0.0f);

			// loop through each of the mesh's vertices
			for (unsigned int i = 0; i < mesh->mNumVertices; i++)
			{
				Vertex vertex;
				glm::vec3 vector;

				// positions
				vector.x = mesh->mVertices[i].x;
				vector.y = mesh->mVertices[i].y;
				vector.z = mesh->mVertices[i].z;
				vertex.Position = vector;

				// normals
				vector.x = mesh->mNormals[i].x;
				vector.y = mesh->mNormals[i].y;
				vector.z = mesh->mNormals[i].z;
				vertex.Normal = vector;

				// texture coordinates
				if (mesh->mTextureCoords[0])
				{
					glm::vec2 vec;
					vec.x = mesh->mTextureCoords[0][i].x;
					vec.y = mesh->mTextureCoords[0][i].y;
					vertex.TexCoords = vec;
				}
				else
					vertex.TexCoords = glm::vec2(0.0f, 0.0f);
				vertices.push_back(vertex);

				// find the global min and max vertices for the hitbox
				// TODO: can this be optimized? probably...but it's not that slow
				if (!verticesSet)		// edge case, initialize all min and max values first
				{
					minVertices.x = mesh->mVertices[i].x;
					minVertices.y = mesh->mVertices[i].y;
					minVertices.z = mesh->mVertices[i].z;
					maxVertices.x = mesh->mVertices[i].x;
					maxVertices.y = mesh->mVertices[i].y;
					maxVertices.z = mesh->mVertices[i].z;
					verticesSet = true;
				}
				else 
				{
					if (mesh->mVertices[i].x < minVertices.x)
					{
						minVertices.x = mesh->mVertices[i].x;
					}
					if (mesh->mVertices[i].y < minVertices.y)
					{
						minVertices.y = mesh->mVertices[i].y;
					}
					if (mesh->mVertices[i].z < minVertices.z)
					{
						minVertices.z = mesh->mVertices[i].z;
					}
					if (mesh->mVertices[i].x > maxVertices.x)
					{
						maxVertices.x = mesh->mVertices[i].x;
					}
					if (mesh->mVertices[i].y > maxVertices.y)
					{
						maxVertices.y = mesh->mVertices[i].y;
					}
					if (mesh->mVertices[i].z > maxVertices.z)
					{
						maxVertices.z = mesh->mVertices[i].z;
					}
				}

				// Store local min and max vertices
				if (mesh->mVertices[i].x < localMinVertices.x)
				{
					localMinVertices.x = mesh->mVertices[i].x;
				}
				if (mesh->mVertices[i].y < localMinVertices.y)
				{
					localMinVertices.y = mesh->mVertices[i].y;
				}
				if (mesh->mVertices[i].z < localMinVertices.z)
				{
					localMinVertices.z = mesh->mVertices[i].z;
				}
				if (mesh->mVertices[i].x > localMaxVertices.x)
				{
					localMaxVertices.x = mesh->mVertices[i].x;
				}
				if (mesh->mVertices[i].y > localMaxVertices.y)
				{
					localMaxVertices.y = mesh->mVertices[i].y;
				}
				if (mesh->mVertices[i].z > localMaxVertices.z)
				{
					localMaxVertices.z = mesh->mVertices[i].z;
				}
			}

			// now loop through each of the mesh's faces (a face is a mesh its triangle), retrieve any indices
			for (unsigned int i = 0; i < mesh->mNumFaces; i++)
			{
				aiFace face = mesh->mFaces[i];
				// retrieve indices and store them in a vector
				for (unsigned int j = 0; j < face.mNumIndices; j++)
					indices.push_back(face.mIndices[j]);
			}
			// process materials
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

			// 1. diffuse maps
			std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
			// 2. specular maps
			std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
			// 3. normal maps
			std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
			textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
			// 4. height maps
			std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
			textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

			// TODO: load a simple material color, rather than textures.

			// Sample found at at: https://www.lighthouse3d.com/cg-topics/code-samples/importing-3d-models-with-assimp/

			// return a mesh object created from the extracted mesh data
			return Mesh(vertices, indices, textures, meshName, localMinVertices, localMaxVertices);
		}

		std::vector<Texture> loadMaterialTextures(aiMaterial * mat, aiTextureType type, std::string typeName)
		{
			std::vector<Texture> textures;
			for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
			{
				aiString str;
				mat->GetTexture(type, i, &str);

				// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
				bool skip = false;
				for (unsigned int j = 0; j < loadedTextures.size(); j++)
				{
					if (std::strcmp(loadedTextures[j].path.data(), str.C_Str()) == 0)
					{
						textures.push_back(loadedTextures[j]);
						skip = true; // texture was already loaded once into memory.
						break;
					}
				}
				if (!skip)	// the texture has already been loaded, no need to load the same image again.
				{   
					Texture texture;
					texture.id = TextureFromFile(str.C_Str(), this->directory);
					texture.type = typeName;
					std::cout << typeName; //The texture maps don't seem to have a specular type, possibly
					texture.path = str.C_Str();
					textures.push_back(texture);
					loadedTextures.push_back(texture); 
				}
			}
			return textures;
		}

		Material loadMaterial(aiMaterial* mat) {
			Material material;
			aiColor3D color(0.f, 0.f, 0.f);
			float shininess;

			mat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
			material.Diffuse = glm::vec3(color.r, color.b, color.g);
			std::cout << "Diffuse: " << material.Diffuse.r << ", " << material.Diffuse.b << ", " << material.Diffuse.g << " ."; //The texture maps don't seem to have a specular type, possibly
			mat->Get(AI_MATKEY_COLOR_AMBIENT, color);
			material.Ambient = glm::vec3(color.r, color.b, color.g);
			std::cout << "Ambient: " << material.Ambient.r << ", " << material.Ambient.b << ", " << material.Ambient.g << " ."; //The texture maps don't seem to have a specular type, possibly
			mat->Get(AI_MATKEY_COLOR_SPECULAR, color);
			material.Specular = glm::vec3(color.r, color.b, color.g);
			std::cout << "Specular: " << material.Specular.r << ", " << material.Specular.b << ", " << material.Specular.g << " ."; //The texture maps don't seem to have a specular type, possibly
			mat->Get(AI_MATKEY_SHININESS, shininess);
			material.Shininess = shininess;




			return material;
		}
	};

	GLuint TextureFromFile(const char* path, const std::string& directory)
	{
		std::string filename = std::string(path);
		filename = directory + '/' + filename;

		GLuint textureID;
		glGenTextures(1, &textureID);

		int width, height, nrComponents;
		unsigned char* data = SOIL_load_image(filename.c_str(), &width, &height, &nrComponents, 0);
		if (data)
		{
			GLenum format = GL_RED;
			if (nrComponents == 1)
				format = GL_RED;
			else if (nrComponents == 3)
				format = GL_RGB;
			else if (nrComponents == 4)
				format = GL_RGBA;

			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glBindTexture(GL_TEXTURE_2D, 0);
			SOIL_free_image_data(data);
		}
		else
		{
			std::cout << "(SOIL) Texture failed to load at path: " << path << std::endl;
			SOIL_free_image_data(data);
		}

		return textureID;
	}
}

#endif A1_MODEL_HPP
