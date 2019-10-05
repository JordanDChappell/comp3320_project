/* Add dependencies
	* SOIL - Library for loading textures (Simple OpenGL Image Library)
	* GLEW - OpenGL Extension Wrangler
	* GLFW - Graphics Library Framework
	* GLM - OpenGL Mathematics
	* OpenAL - Open Audio Library
	*/
#include <windows.h>
#include <sdl.h>
#include <SOIL.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "al.h" 
#include "alc.h" 

// Include the standard C++ headers
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

// Include project files
#include "util/mainUtil.hpp"
#include "util/camera.hpp"
#include "terrain/terrain.hpp"
#include "models/model.hpp"
#include "skybox/skybox.hpp"
#include "water/water.hpp"
#include "water/WaterFrameBuffers.hpp"

// Initial width and height of the window
GLuint SCREEN_WIDTH = 1200;
GLuint SCREEN_HEIGHT = 800;

// Distances to the near and the far plane. Used for the camera to clip space transform.
static constexpr float NEAR_PLANE = 0.1f;
static constexpr float FAR_PLANE = 1000.0f;

std::vector<model::Model> models;	// vector of all models to render
std::vector<model::HitBox> hitBoxes; // vector of all hitboxes in the scene for collision detections
static int debounceCounter = 0;		 // simple counter to debounce keyboard inputs

void process_input(GLFWwindow *window, const float &delta_time, utility::camera::Camera &camera, float terrainHeight)
{
	// Movement sensitivity is updated base on the delta_time and not framerate, gravity accelleration is also based on delta_time
	camera.set_movement_sensitivity(30.0f * delta_time);
	camera.gravity(delta_time, terrainHeight); // apply gravity, giving the floor of the current (x,y) position

	// Process movement inputs
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
	else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		camera.move_forward(hitBoxes);
	}
	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		camera.move_backward(hitBoxes);
	}
	else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		camera.move_left(hitBoxes);
	}
	else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		camera.move_right(hitBoxes);
	}

	// Process debounced inputs - this ensures we won't have 5 jump events triggering before we leave the ground etc.
	if (debounceCounter == 0)
	{
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		{
			camera.jump(delta_time, terrainHeight);
		}
		else if (glfwGetKey(window, GLFW_KEY_GRAVE_ACCENT) == GLFW_PRESS)
		{
			camera.toggleNoClip();
		}
		else if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
		{
			std::cout << "Model Hitbox: " << std::endl;
			std::cout << hitBoxes[0].origin.x << " " << hitBoxes[0].origin.y << " " << hitBoxes[0].origin.z << std::endl;
			std::cout << hitBoxes[0].size.x << " " << hitBoxes[0].size.y << " " << hitBoxes[0].size.z << std::endl;

			model::HitBox cameraHitBox = camera.getHitBox();

			std::cout << "Model Hitbox: " << std::endl;
			std::cout << cameraHitBox.origin.x << " " << cameraHitBox.origin.y << " " << cameraHitBox.origin.z << std::endl;
			std::cout << cameraHitBox.size.x << " " << cameraHitBox.size.y << " " << cameraHitBox.size.z << std::endl;
		}
	}
	// debounce inputs
	if (debounceCounter == 5)
	{
		debounceCounter = 0;
	}
	else
	{
		debounceCounter++;
	}
}

void render(terrain::Terrain terra, utility::camera::Camera camera, std::vector<model::Model> models, GLuint modelShader, glm::vec4 clippingPlane)
{
	// get the camera transforms
	glm::mat4 Hvw = camera.get_view_transform();
	glm::mat4 Hcv = camera.get_clip_transform();
	glm::mat4 Hwm = glm::mat4(1.0f);

	// Clear color buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// NOTE: Draw all other objects before the skybox

	// Draw the models
	glDepthFunc(GL_LESS);

	for (int i = 0; i < models.size(); i++)
	{
		models.at(i).Draw(modelShader, Hvw, Hcv, Hwm, clippingPlane);
	}

	//-------------
	// DRAW TERRAIN
	//-------------
	terra.draw(Hvw, Hcv, clippingPlane, camera.get_position(), glm::vec3(0.0, 50, 0.0), glm::vec3(1.0, 1.0, 1.0),
			   glfwGetTime());
}

// Loads a loading screen for FARM-LIFE: GAME OF THE YEAR EDITION
void addLoadingScreen()
{
	// Create and bind vertex array object
	GLuint vao1;
	glGenVertexArrays(1, &vao1);
	glBindVertexArray(vao1);

	// Create vertex buffer object
	float vertices[] = {
		-1.0f, 1.0f, 0.0f, 0.0f, // upper left
		1.0f, 1.0f, 1.0f, 0.0f,  // upper right
		1.0f, -1.0f, 1.0f, 1.0f, // lower right
		-1.0f, -1.0f, 0.0f, 1.0f // lower left
	};

	GLuint vbo1;
	glGenBuffers(1, &vbo1);
	glBindBuffer(GL_ARRAY_BUFFER, vbo1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Create and bind element buffer object
	int elements[] = {
		0, 1, 2,
		2, 3, 0};

	GLuint ebo1;
	glGenBuffers(1, &ebo1);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo1);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

	// Create shader program
	GLuint shader1 = LoadShaders("shaders/loading.vert", "shaders/loading.frag");
	glUseProgram(shader1);

	// Create and bind texture
	int width, height; // Variables for the width and height of image being loaded
	GLuint tex1;
	glGenTextures(1, &tex1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex1);
	unsigned char *image = SOIL_load_image("loading_screen.png", &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
				 GL_UNSIGNED_BYTE, image);
	glUniform1i(glGetUniformLocation(shader1, "screen"), 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Set vertex attributes
	GLuint posAttrib = glGetAttribLocation(shader1, "position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE,
						  4 * sizeof(float), 0);

	GLuint texAttrib = glGetAttribLocation(shader1, "texcoords");
	glEnableVertexAttribArray(texAttrib);
	glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE,
						  4 * sizeof(float), (void *)(2 * sizeof(float)));
}

int main(void)
{
	// Set the screen size by the current desktop height, width (for fullscreen)
	//setScreenSize(SCREEN_WIDTH, SCREEN_HEIGHT);

	std::srand(1);
	utility::camera::Camera camera(SCREEN_WIDTH, SCREEN_HEIGHT, NEAR_PLANE, FAR_PLANE);

	//Set the error callback
	glfwSetErrorCallback(error_callback);

	//Initialize GLFW
	if (!glfwInit())
	{
		exit(EXIT_FAILURE);
	}

	//Set the GLFW window creation hints - these are optional
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);				   //Request a specific OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);				   //Request a specific OpenGL version
	glfwWindowHint(GLFW_SAMPLES, 4);							   //Request 4x antialiasing
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //modern opengl

	//Declare a window object
	GLFWwindow *window;

	// Create a window and create its OpenGL context, creates a fullscreen window using glfwGetPrimaryMonitor(), requires a monitor for fullscreen
	//window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Farm-Life: GOTY Edition", glfwGetPrimaryMonitor(), NULL);

	//USE THIS LINE INSTEAD OF LINE ABOVE IF GETTING RUNTIME ERRORS
	window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Farm-Life: GOTY Edition", NULL, NULL);

	if (window == NULL)
	{
		std::cerr << "Failed to create GLFW window with dimension " << SCREEN_WIDTH << SCREEN_HEIGHT
				  << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window,
								   CCallbackWrapper(GLFWframebuffersizefun, utility::camera::Camera)(
									   std::bind(&utility::camera::Camera::framebuffer_size_callback,
												 &camera,
												 std::placeholders::_1,
												 std::placeholders::_2,
												 std::placeholders::_3)));

	// get glfw to capture and hide the mouse pointer
	// ----------------------------------------------
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(
		window,
		CCallbackWrapper(GLFWcursorposfun, utility::camera::Camera)(std::bind(&utility::camera::Camera::mouse_callback,
																			  &camera,
																			  std::placeholders::_1,
																			  std::placeholders::_2,
																			  std::placeholders::_3)));

	// get glfw to capture mouse scrolling
	// -----------------------------------
	glfwSetScrollCallback(
		window,
		CCallbackWrapper(GLFWscrollfun, utility::camera::Camera)(std::bind(&utility::camera::Camera::scroll_callback,
																		   &camera,
																		   std::placeholders::_1,
																		   std::placeholders::_2,
																		   std::placeholders::_3)));

	//Sets the key callback
	glfwSetKeyCallback(window, key_callback);

	//Initialize GLEW
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();

	//If GLEW hasn't initialized
	if (err != GLEW_OK)
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		return -1;
	}

	//--------------------------------------------------------
	//-----------COMPLETED DEPENDENCY INITITIALISATION--------
	//------------------INITIALISE SCENE----------------------
	//--------------------------------------------------------

	// Draw screen while waiting for the main program to load
	addLoadingScreen();
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glfwSwapBuffers(window);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);

	//---------------
	// CREATE TERRAIN
	//---------------
	// Set up variables and initialize a terrain instance
	int tresX = 1000; // x and y resolutions for the terrain, keep in a variable for use in other calculations
	int tresY = 1000;
	float terraScale = 1.0;
	int terraMaxHeight = 30;
	float terraYOffset = -20.0f; // the terrain is offset in the y by terraYOffset
	// Create main terrain
	terrain::Terrain terra = terrain::Terrain(tresX, tresY, terraScale, terraMaxHeight, terraYOffset, terraMaxHeight / 2.5);
	// Create water frame buffers for reflection and refraction
	water::WaterFrameBuffers fbos = water::WaterFrameBuffers();
	// Create water
	water::Water water = water::Water(tresX, tresY, terraScale, terraMaxHeight / 2.5, fbos);

	// Set up the camera offset, terrain is from (-500,-500) to (500,500) in the world, camera range is (0,0) to (1000,1000)
	int cameraOffsetX = tresX / 2;
	int cameraOffsetY = tresY / 2;

	//--------------
	// CREATE MODELS
	//--------------

	// Load the shaders to be used in the scene
	//GLuint shaderProgram = LoadShaders("shaders/shader.vert", "shaders/shader.frag");
	GLuint modelShader = LoadShaders("shaders/model.vert", "shaders/model.frag");

	// Load a model using model class
	model::Model giraffe = model::Model("models/giraffe/giraffe.obj");
	// Locate the model in the scene, simply give x and y coordinates (technically x and z in openGL)
	int modelXCoord = 100;
	int modelYCoord = 0;
	// get the terrain height at the current x,y coordinate in the scene, add the camera terrain height offset, add half the models height to get to ground level
	// need to fix the hitboxes for this to work effectively, currently models aren't stuck to the ground nicely
	float modelHeightInWorld = terra.getHeightAt(modelXCoord + cameraOffsetX, modelYCoord + cameraOffsetY) + terraYOffset + (giraffe.hitBox.size.y / 2);
	giraffe.MoveTo(glm::vec3(modelXCoord, modelHeightInWorld, modelYCoord)); // move the model to a space in the scene
	models.push_back(giraffe);												 // push the model to the render vector
	hitBoxes.push_back(giraffe.hitBox);										 // push the model's hitbox to the hitBox vector

	model::Model barn = model::Model("models/barn/barn.obj");
	modelXCoord = 10;
	modelYCoord = 10;
	modelHeightInWorld = terra.getHeightAt(modelXCoord + cameraOffsetX, modelYCoord + cameraOffsetY) + terraYOffset + (barn.hitBox.size.y / 2);
	barn.MoveTo(glm::vec3(modelXCoord, modelHeightInWorld, modelYCoord));
	models.push_back(barn);
	hitBoxes.push_back(barn.hitBox);

	model::Model cat = model::Model("models/cat/cat.obj");
	modelXCoord = 100;
	modelYCoord = 10;
	modelHeightInWorld = terra.getHeightAt(modelXCoord + cameraOffsetX, modelYCoord + cameraOffsetY) + terraYOffset + (cat.hitBox.size.y / 2);
	cat.MoveTo(glm::vec3(modelXCoord, modelHeightInWorld, modelYCoord));
	models.push_back(cat);
	hitBoxes.push_back(cat.hitBox);

	model::Model fence = model::Model("models/fence/fence.obj");
	fence.MoveTo(glm::vec3(-10, 0, -4));
	models.push_back(fence);

	model::Model bucket = model::Model("models/bucket/bucket.obj");
	bucket.MoveTo(glm::vec3(-10, 0, 10));
	models.push_back(bucket);

	model::Model trough = model::Model("models/trough/watertrough.obj");
	trough.MoveTo(glm::vec3(-10, -4, 9));

	//--------------
	// CREATE SKYBOX
	//--------------
	skybox::Skybox skybox = skybox::Skybox();
	skybox.getInt();

	// Init before the main loop
	float last_frame = glfwGetTime();
	float current_frame = 0.0f;
	float delta_time = 0.0f;
	//Set a background color
	glClearColor(0.0f, 0.0f, 0.6f, 0.0f);

	// Main Loop
	do
	{
		/* PROCESS INPUT */
		current_frame = glfwGetTime();
		delta_time = current_frame - last_frame;
		last_frame = current_frame;

		// find the current rough terrain height at the camera position
		int cameraX = (int)camera.get_position().x + cameraOffsetX;
		int cameraY = (int)camera.get_position().z + cameraOffsetY;
		float terrainHeight = terra.getHeightAt(cameraX, cameraY) + terraYOffset + 5.0f; // using the offset down 20.0f units and adding some height for the camera
		process_input(window, delta_time, camera, terrainHeight);

		glm::mat4 Hvw = camera.get_view_transform();
		glm::mat4 Hcv = camera.get_clip_transform();

		//------------------------------------------
		// RENDER REFLECTION AND REFRACTION TEXTURES
		//------------------------------------------
		// If camera is above the water, do reflection and refraction as you would expect
		if (camera.get_position().y > water.getHeight() - 0.5)
		{
			// Allow clipping
			glEnable(GL_CLIP_DISTANCE0);

			// Bind the reflection frame buffer
			fbos.bindReflectionFrameBuffer();

			// Move the camera
			float distance = 2 * (camera.get_position().y - water.getHeight());
			camera.move_y_position(-distance);
			camera.invert_pitch();

			// Render the scene
			render(terra, camera, models, modelShader, glm::vec4(0, 1, 0, -water.getHeight()));

			// Render skybox last, disable clipping for skybox
			glDisable(GL_CLIP_DISTANCE0);
			Hvw = glm::mat4(glm::mat3(camera.get_view_transform())); // remove translation from the view matrix. Keeps the skybox centered on camera.
			skybox.render(Hvw, Hcv);
			glEnable(GL_CLIP_DISTANCE0);

			// Move the camera back
			camera.move_y_position(distance);
			camera.invert_pitch();

			// Bind the refraction frame buffer
			fbos.bindRefractionFrameBuffer();

			// Render the scene
			render(terra, camera, models, modelShader, glm::vec4(0, -1, 0, water.getHeight()));
			// Render skybox last
			glDisable(GL_CLIP_DISTANCE0);
			Hvw = glm::mat4(glm::mat3(camera.get_view_transform())); // remove translation from the view matrix. Keeps the skybox centered on camera.
			skybox.render(Hvw, Hcv);
		}
		// If the camera is below the water, dont need reflection only refraction
		else
		{
			// Allow clipping
			glEnable(GL_CLIP_DISTANCE0);

			// Bind the reflection frame buffer
			fbos.bindReflectionFrameBuffer();

			// Render the scene, don't bother changing since this is refraction
			render(terra, camera, models, modelShader, glm::vec4(0, 1, 0, -water.getHeight()));
			// Render skybox last, disable clipping for skybox
			glDisable(GL_CLIP_DISTANCE0);
			Hvw = glm::mat4(glm::mat3(camera.get_view_transform())); // remove translation from the view matrix. Keeps the skybox centered on camera.
			skybox.render(Hvw, Hcv);
			glEnable(GL_CLIP_DISTANCE0);

			// Bind the refraction frame buffer
			fbos.bindRefractionFrameBuffer();
			// Render the scene
			render(terra, camera, models, modelShader, glm::vec4(0, -1, 0, water.getHeight()));
			// Render skybox last
			glDisable(GL_CLIP_DISTANCE0);
			Hvw = glm::mat4(glm::mat3(camera.get_view_transform())); // remove translation from the view matrix. Keeps the skybox centered on camera.
			skybox.render(Hvw, Hcv);
		}

		// Unbind the frame buffer before rendering the scene
		fbos.unbindCurrentFrameBuffer(SCREEN_WIDTH, SCREEN_HEIGHT);

		//-----------------
		// RENDER THE SCENE
		//-----------------
		// Render terrain, skybox and models
		render(terra, camera, models, modelShader, glm::vec4(0, 0, 0, 0));

		// TODO: Send in a light when lights are done
		// Render water
		water.draw(camera.get_view_transform(), camera.get_clip_transform(), camera.get_position(),
				   glfwGetTime(), glm::vec3(0.0, 50, 0.0), glm::vec3(1.0, 1.0, 1.0), (camera.get_position().y > water.getHeight() - 0.5));
		// Render skybox last
		Hvw = glm::mat4(glm::mat3(camera.get_view_transform())); // remove translation from the view matrix. Keeps the skybox centered on camera.
		skybox.render(Hvw, Hcv);

		//Swap buffers
		glfwSwapBuffers(window);
		//Get and organize events, like keyboard and mouse input, window resizing, etc...
		glfwPollEvents();

	} // Check if the ESC key had been pressed or if the window had been closed
	while (!glfwWindowShouldClose(window));

	// Cleanup (delete buffers etc)
	terra.cleanup();
	fbos.cleanup();

	// Close OpenGL window and terminate GLFW
	glfwDestroyWindow(window);
	// Finalize and clean up GLFW
	glfwTerminate();

	exit(EXIT_SUCCESS);
}