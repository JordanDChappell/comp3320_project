    /* Add dependencies
	* SOIL - Library for loading textures (Simple OpenGL Image Library)
	* GLEW - OpenGL Extension Wrangler
	* GLFW - Graphics Library Framework
	* GLM - OpenGL Mathematics
	*/
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
	#include "skybox/skybox.hpp";

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
		window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Farm-Life: GOTY Edition", glfwGetPrimaryMonitor(), NULL);

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

		// Enable depth test
		glEnable(GL_DEPTH_TEST);

		// Triangle Vertices
		float vertices[] = {
			//  Position      Color
				-0.5f,  0.5f, 1.0f, 0.0f, 0.0f, // Top-left
				 0.5f,  0.5f, 0.0f, 1.0f, 0.0f, // Top-right
				 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // Bottom-right
		};

		// Load the shaders to be used in the scene
		//GLuint shaderProgram = LoadShaders("shaders/shader.vert", "shaders/shader.frag");
		GLuint modelShader = LoadShaders("shaders/model.vert", "shaders/model.frag");

		/* -------------------------- Draw the Triangle -------------------------- */
		// Triangle VAO, VBO
		//GLuint vao, vbo;
		//glGenVertexArrays(1, &vao);
		//glBindVertexArray(vao);
		//glGenBuffers(1, &vbo);		
		//glBindBuffer(GL_ARRAY_BUFFER, vbo);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
		//// link vertex data (position, colour and texture coords) to shader
		//GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
		//glEnableVertexAttribArray(posAttrib);
		//glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE,
		//	5 * sizeof(float), 0);
		//GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
		//glEnableVertexAttribArray(colAttrib);
		//glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE,
		//	5 * sizeof(float), (void*)(2 * sizeof(float)));

		// Load a model using model class
		model::Model giraffe = model::Model("models/giraffe/giraffe.obj");
		//giraffe.MoveTo(glm::vec3(0, 0, 0));
		giraffe.MoveTo(glm::vec3(10, 10, 10));	// move the model to a space in the scene

		// Create the skybox class instance
		skybox::Skybox skybox = skybox::Skybox();
		skybox.getInt();		

		// Init before the main loop
        float last_frame = glfwGetTime();

		// Create terrain
		terraObj terra = createTerrain();

        //Set a background color  
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

			/* RENDER */
			// Clear color buffer  
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
			
			// NOTE: Draw all other objects before the skybox

			// Draw the triangle
			// Accept fragment if it closer to the camera than the former one
			//glDepthFunc(GL_LESS);
			//glUseProgram(shaderProgram);
			/*glm::mat4 Hvw = camera.get_view_transform();
			glm::mat4 Hcv = camera.get_clip_transform();
			glm::mat4 Hwm = glm::mat4(1.0f);*/
			//glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "Hvw"), 1, GL_FALSE, &Hvw[0][0]);
			//glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "Hcv"), 1, GL_FALSE, &Hcv[0][0]);
			//glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "Hwm"), 1, GL_FALSE, &Hwm[0][0]);
			//glBindVertexArray(vao);
			//glDrawArrays(GL_TRIANGLES, 0, 3);
			//glBindVertexArray(0);

			// Draw the model
			glDepthFunc(GL_LESS);
			glUseProgram(modelShader);
			glm::mat4 Hvw = camera.get_view_transform();
			glm::mat4 Hcv = camera.get_clip_transform();
			glm::mat4 Hwm = glm::mat4(1.0f);
			giraffe.Draw(modelShader, Hvw, Hcv, Hwm);
			

			//-------------
			// DRAW TERRAIN 
			//-------------
			Hvw = camera.get_view_transform();
			generateTerrain(terra, Hvw, Hcv);

			//--------------------------
			// DRAW SKYBOX - always last
			//--------------------------
			Hvw = glm::mat4(glm::mat3(camera.get_view_transform()));	// remove translation from the view matrix.
			skybox.render(Hvw, Hcv);

            //Swap buffers  
            glfwSwapBuffers(window);  
            //Get and organize events, like keyboard and mouse input, window resizing, etc...  
            glfwPollEvents();  
      
        } //Check if the ESC key had been pressed or if the window had been closed  
        while (!glfwWindowShouldClose(window));  
      
        //Close OpenGL window and terminate GLFW  
        glfwDestroyWindow(window);  
        //Finalize and clean up GLFW  
        glfwTerminate();  
      
        exit(EXIT_SUCCESS);  
    }  