    /* Add dependencies
	* SOIL - Library for loading textures (Simple OpenGL Image Library)
	* GLEW - OpenGL Extension Wrangler
	* GLFW - Graphics Library Framework
	* GLM - OpenGL Mathematics
	*/
	#include <windows.h>	
	#include <sdl.h>
	#include <SOIL.h>
    #include <GL/glew.h>  
	#include <GLFW/glfw3.h>  
	#include "glm/glm.hpp"
	#include "glm/gtc/matrix_transform.hpp"
	#include "glm/gtc/type_ptr.hpp"

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
	GLuint SCREEN_WIDTH = 1920;
    GLuint SCREEN_HEIGHT = 1080;

	// Distances to the near and the far plane. Used for the camera to clip space transform.
	static constexpr float NEAR_PLANE = 0.1f;
	static constexpr float FAR_PLANE = 1000.0f;

	void process_input(GLFWwindow* window, const float& delta_time, utility::camera::Camera& camera) {
		camera.set_movement_sensitivity(0.005f * delta_time);

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, true);
		}
		else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			camera.move_forward();
		}
		else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			camera.move_backward();
		}
		else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			camera.move_left();
		}
		else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			camera.move_right();
		}
	}

	void render(terrain::Terrain terra, utility::camera::Camera camera, skybox::Skybox skybox, std::vector<model::Model> models, GLuint modelShader, glm::vec4 clippingPlane) {
		// get the camera transforms
		glm::mat4 Hvw = camera.get_view_transform();
		glm::mat4 Hcv = camera.get_clip_transform();
		glm::mat4 Hwm = glm::mat4(1.0f);
		
		// Clear color buffer  
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// NOTE: Draw all other objects before the skybox

		// Draw the models
		glDepthFunc(GL_LESS);

		for (int i = 0; i < models.size(); i++) {
			models.at(i).Draw(modelShader, Hvw, Hcv, Hwm, clippingPlane);
		}

		//-------------
		// DRAW TERRAIN 
		//-------------
		terra.draw(Hvw, Hcv, clippingPlane);

		//--------------------------
		// DRAW SKYBOX - always last
		//--------------------------
		Hvw = glm::mat4(glm::mat3(camera.get_view_transform()));	// remove translation from the view matrix. Keeps the skybox centered on camera.
		skybox.render(Hvw, Hcv);
	}

    int main( void )  
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
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //Request a specific OpenGL version  
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); //Request a specific OpenGL version  
		glfwWindowHint(GLFW_SAMPLES, 4); //Request 4x antialiasing  
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  //modern opengl

		//Declare a window object  
		GLFWwindow* window;

		// Create a window and create its OpenGL context, creates a fullscreen window using glfwGetPrimaryMonitor(), requires a monitor for fullscreen
		//window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Farm-Life: GOTY Edition", glfwGetPrimaryMonitor(), NULL);

		//USE THIS LINE INSTEAD OF LINE ABOVE IF GETTING RUNTIME ERRORS
		window = glfwCreateWindow(1200, 800, "Farm-Life: GOTY Edition", NULL, NULL);

		if (window == NULL) {
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
		//--------------------------------------------------------

		// Enable depth test
		glEnable(GL_DEPTH_TEST);

		// Load the shaders to be used in the scene
		GLuint modelShader = LoadShaders("shaders/model.vert", "shaders/model.frag");

		std::vector<model::Model> models;

		// Load a model using model class
		model::Model giraffe = model::Model("models/giraffe/giraffe.obj");
		giraffe.MoveTo(glm::vec3(10, 10, 10));	// move the model to a space in the scene
		models.push_back(giraffe);

		model::Model barn = model::Model("models/barn/barn.obj");
		barn.MoveTo(glm::vec3(0, 0, 0));
		models.push_back(barn);

		model::Model cat = model::Model("models/cat/cat.obj");
		cat.MoveTo(glm::vec3(-10, -1, 0));
		models.push_back(cat);

		model::Model fence = model::Model("models/fence/fence.obj");
		fence.MoveTo(glm::vec3(-10, 0, -4));
		models.push_back(fence);

		model::Model bucket = model::Model("models/bucket/bucket.obj");
		bucket.MoveTo(glm::vec3(-10, 0, 10));
		models.push_back(bucket);

		model::Model trough = model::Model("models/trough/watertrough.obj");
		trough.MoveTo(glm::vec3(-10, -4, 9));
		models.push_back(trough);

		// Create the skybox class instance
		skybox::Skybox skybox = skybox::Skybox();
		skybox.getInt();

		// Init before the main loop
		float last_frame = glfwGetTime();

		// Create Terrain
		terrain::Terrain terra = terrain::Terrain();

		// Create water frame buffers for reflection and refraction
		water::WaterFrameBuffers fbos = water::WaterFrameBuffers();

		// Create water
		water::Water water = water::Water(1000, 1000, 0.5, 6.0, fbos.getRefractionTexture(), fbos.getReflectionTexture());

		// Set a background color  
		glClearColor(0.0f, 0.0f, 0.6f, 0.0f);

		float delta_time = 0.0f;

        // Main Loop  
        do  
        {  
			/* PROCESS INPUT */
			float current_frame = glfwGetTime();
			float delta_time = current_frame - last_frame;
			float last_frame = current_frame;
			process_input(window, delta_time, camera);

			glEnable(GL_CLIP_DISTANCE0);

			// Render the reflection
			fbos.bindReflectionFrameBuffer();
			float distance = 2 * (camera.get_position().y - water.getHeight());
			camera.move_y_position(-distance);
			camera.invert_pitch();
			render(terra, camera, skybox, models, modelShader, glm::vec4(0, 1, 0, -water.getHeight()));
			camera.move_y_position(distance);
			camera.invert_pitch();
			
			// Render the refraction
			fbos.bindRefractionFrameBuffer();
			render(terra, camera, skybox, models, modelShader, glm::vec4(0, -1, 0, water.getHeight()));
			
			// Unbind the frame buffer before rendering the scene
			fbos.unbindCurrentFrameBuffer(1200, 800);
			
			glDisable(GL_CLIP_DISTANCE0);
			// Render the scene
			render(terra, camera, skybox, models, modelShader, glm::vec4(0, 0, 0, 0));
			water.draw(camera.get_view_transform(), camera.get_clip_transform(), camera.get_position(), 
				glm::vec3(0.0f, 0.467f, 0.745f), glfwGetTime(), 1.5, glm::vec3(0.0, 50, 0.0), glm::vec3(1.0, 1.0, 1.0));
            
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