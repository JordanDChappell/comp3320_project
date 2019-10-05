#ifndef A1_CAMERA_HPP
#define A1_CAMERA_HPP

#include <functional>
#include <algorithm>
#include <cmath>
#include "models/model.hpp"

namespace utility {
	namespace camera {

		// A simple FPS-style camera
		// -------------------------
		class Camera {
		public:
			// Create the camera and set some useful defaults
			// ----------------------------------------------
			Camera(const int& width, const int& height, const float& near_plane, const float& far_plane) {
				this->width = width;
				this->height = height;
				this->near_plane = near_plane;
				this->far_plane = far_plane;

				fov = 45.0f;
				aspect_ratio = static_cast<float>(width) / static_cast<float>(height);
				forward = glm::vec3(0.0f, 0.0f, -1.0f);
				up = glm::vec3(0.0f, 1.0f, 0.0f);
				world_up = glm::vec3(0.0f, 1.0f, 0.0f);
				position = glm::vec3(0.0f, 50.0f, 0.0f);
				right = glm::normalize(glm::cross(forward, up));

				orientation = glm::vec2(-90.0f, 0.0f);
				update_camera_basis();

				first_mouse = true;
				last_mouse_pos = glm::vec2(static_cast<float>(width) * 0.5f, static_cast<float>(height) * 0.5f);
				rotation_sensitivity = 0.05f;
				movement_sensitivity = 10.0f;

				// init the camera hitbox, used for collision detection, currently a 1.0f unit cube
				hitBox.origin = position - glm::vec3(0.5f, 0.5f, 0.5f);
				hitBox.size = position + glm::vec3(0.5f, 0.5f, 0.5f) - hitBox.origin;

				noClip = false;
				downVelocity = 0;
			}

			// Callback function so GLFW can tell us about mouse scroll events
			// ---------------------------------------------------------------
			void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
				// Clamp field of view between 1 and 45 degrees
				fov = std::min(std::max(1.0f, fov - static_cast<float>(yoffset)), 45.0f);
			}

			// Callback function so GLFW can tell us about mouse movement events
			// -----------------------------------------------------------------
			void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
				// prevent erratic movements when the mouse first enters the screen
				if (first_mouse) {
					last_mouse_pos = glm::vec2(static_cast<float>(xpos), static_cast<float>(ypos));
					first_mouse = false;
				}
				else {
					// get the current position of the mouse
					glm::vec2 current_mouse_pos(static_cast<float>(xpos), static_cast<float>(ypos));

					// invert y-ccordinates since they range from top to bottom
					glm::vec2 offset(current_mouse_pos.x - last_mouse_pos.x, last_mouse_pos.y - current_mouse_pos.y);

					// update the last mouse position
					last_mouse_pos = current_mouse_pos;

					// update camera rotation
					orientation += offset * rotation_sensitivity;

					// clamp pitch to [-89, 89] degrees
					// weird things happen when pitch is at +/- 90
					orientation.y = std::min(std::max(-89.0f, orientation.y), 89.0f);

					update_camera_basis();
				}
			}

			void update_camera_basis() {
				// calculate yaw, pitch, roll
				const float cos_yaw = std::cos(glm::radians(orientation.x));
				const float sin_yaw = std::sin(glm::radians(orientation.x));
				const float cos_pitch = std::cos(glm::radians(orientation.y));
				const float sin_pitch = std::sin(glm::radians(orientation.y));

				// update camera forward, right, and up vectors
				forward = glm::normalize(glm::vec3(cos_pitch * cos_yaw, sin_pitch, cos_pitch * sin_yaw));
				right = glm::normalize(glm::cross(forward, world_up));
				up = glm::normalize(glm::cross(right, forward));
			}

			// Callback function so GLFW can tell us about window resize events
			// ----------------------------------------------------------------
			void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
				// make sure the viewport matches the new window dimensions; note that width and
				// height will be significantly larger than specified on retina displays.
				glViewport(0, 0, width, height);
				this->width = width;
				this->height = height;
				aspect_ratio = static_cast<float>(width) / static_cast<float>(height);
			}

			// Move the camera's y position by distance
			void move_y_position(float distance) {
				position.y += distance;
			}

			// Invert (negate) the pitch of the camera
			void invert_pitch() {
				orientation.y = -orientation.y;
				update_camera_basis();
			}

			// Invert (negate) the yaw of the camera
			void invert_yaw() {
				orientation.x = -orientation.x;
				update_camera_basis();
			}


			// Calculate and return the world to camera transform
			// --------------------------------------------------
			glm::mat4 get_view_transform() {
				return glm::lookAt(position, position + forward, up);
			}

			// Calculate and return the camera to clip transform
			// --------------------------------------------------
			glm::mat4 get_clip_transform() {
				return glm::perspective(glm::radians(fov), aspect_ratio, near_plane, far_plane);
			}

			// Return the camera position
			// --------------------------
			glm::vec3 get_position() {
				return position;
			}

			// Return the camera view direction
			// --------------------------------
			glm::vec3 get_view_direction() {
				return forward;
			}

			// Return the camera hitbox
			model::HitBox getHitBox()
			{
				return hitBox;
			}

			// Set the sensitivity of keyboard movement events
			// -----------------------------------------------
			void set_movement_sensitivity(const float& sensitivity) {
				movement_sensitivity = sensitivity;
			}

			// Set the sensitivity of mouse movement events
			// --------------------------------------------
			void set_rotation_sensitivity(const float& sensitivity) {
				rotation_sensitivity = sensitivity;
			}

			// Strafe left
			// Moves the camera to the left if there are no collisions detected
			void move_left(std::vector<model::HitBox> hitBoxes) {
				// tempOrigin is the proposed next location of the camera, used to test for a collision on the next movement space
				glm::vec3 tempOrigin = hitBox.origin - right * movement_sensitivity;
				// if noClip is not set, detect a collision, otherwise ignore it and allow the camera to clip through things
				if (!noClip) 
				{
					if (!collisionDetected(hitBoxes, tempOrigin, hitBox.size))
					{
						position -= right * movement_sensitivity;	// the next position is valid so we can move the camera
						hitBox.origin = tempOrigin;					// move the hitbox with the camera
					}
				}
				else
				{
					position -= right * movement_sensitivity;
					hitBox.origin = tempOrigin;
				}
			}

			// Strafe right
			// The opposite movement of left, a clone of the left function with a positive right direction
			void move_right(std::vector<model::HitBox> hitBoxes) {
				glm::vec3 tempOrigin = hitBox.origin + right * movement_sensitivity;
				if (!noClip)
				{
					if (!collisionDetected(hitBoxes, tempOrigin, hitBox.size))
					{
						position += right * movement_sensitivity;
						hitBox.origin = tempOrigin;
					}
				}
				else
				{
					position += right * movement_sensitivity;
					hitBox.origin = tempOrigin;
				}
			}

			// Move forward
			// ------------
			void move_forward(std::vector<model::HitBox> hitBoxes) {
				// Remove Y axis movement from the forward vector, this will keep the camera from taking flight!
				glm::vec3 movementForward = glm::vec3(forward.x, 0.0f, forward.z);
				glm::vec3 tempOrigin = hitBox.origin + movementForward * movement_sensitivity;
				if (!noClip)
				{
					if (!collisionDetected(hitBoxes, tempOrigin, hitBox.size))
					{
						position += movementForward * movement_sensitivity;
						hitBox.origin = tempOrigin;
					}
				}
				else
				{
					position += forward * movement_sensitivity;
					hitBox.origin = tempOrigin;
				}
			}

			// Move backward
			// -------------
			void move_backward(std::vector<model::HitBox> hitBoxes) {
				// Remove Y axis movement from the forward vector, this will keep the camera from taking flight!
				glm::vec3 movementForward = glm::vec3(forward.x, 0.0f, forward.z);
				glm::vec3 tempOrigin = hitBox.origin - movementForward * movement_sensitivity;
				if (!noClip)
				{
					if (!collisionDetected(hitBoxes, tempOrigin, hitBox.size))
					{
						position -= movementForward * movement_sensitivity;
						hitBox.origin = tempOrigin;
					}
				}
				else
				{
					position -= forward * movement_sensitivity;
					hitBox.origin = tempOrigin;
				}
			}

			// Move up
			// -------
			void move_up(std::vector<model::HitBox> hitBoxes) {
				glm::vec3 tempOrigin = hitBox.origin + up * movement_sensitivity;
				if (!collisionDetected(hitBoxes, tempOrigin, hitBox.size))
				{
					position += up * movement_sensitivity;
					hitBox.origin = tempOrigin;
				}
			}

			// Move down
			// ---------
			void move_down(std::vector<model::HitBox> hitBoxes) {
				glm::vec3 tempOrigin = hitBox.origin - up * movement_sensitivity;
				if (!collisionDetected(hitBoxes, tempOrigin, hitBox.size))
				{
					position -= up * movement_sensitivity;
					hitBox.origin = tempOrigin;
				}
			}

			// Toggle no clip mode
			// Unbinds the camera from the terrain height
			void toggleNoClip()
			{
				noClip = !noClip;
			}

			// Returns true if no clip is enabled
			bool getNoClip()
			{
				return noClip;
			}

			///<summary>Apply downwards velocity to the camera if it is above the terrain height</summmary>
			void gravity(float deltaTime, float terrainHeight)
			{
				if (!noClip)
				{
					// Check whether the camera is "on the ground", make sure it stays above the ground
					if (position.y < terrainHeight)
					{
						if (downVelocity != 0)
						{
							downVelocity = 0;	// reset velocity
						}
						position.y = terrainHeight;
						hitBox.origin.y = terrainHeight;
					}
					// If the camera is "in the air" apply the gravity calculation
					else
					{
						// Check whether we have reached terminal velocity
						if (downVelocity > -50.0f)
						{
							downVelocity += -9.8f * deltaTime;	// first set velocity
						}
						position.y += downVelocity * deltaTime;	// move the camera towards the terrain
						hitBox.origin.y += downVelocity * deltaTime;	// make sure to move the hitbox with the camera
					}
				}
			}

			///<summary>Apply a burst of upwards velocity</summmary>
			void jump(float deltaTime, float terrainHeight)
			{
				// Check if the camera is on the ground first, can't double jump :(
				if (position.y <= terrainHeight)
				{
					// An initial burst of upwards velocity
					downVelocity += 10.0f;
					position.y += downVelocity * deltaTime;
					hitBox.origin.y += downVelocity * deltaTime;
				}
			}

		private:
			// Width, height, and aspect ratio of window
			int width;
			int height;
			float aspect_ratio;

			// Distances to the near and far planes (for clip transform)
			float near_plane;
			float far_plane;

			// Field of view of the camera (for clip transform to simulate zooming)
			// Stored in degrees
			float fov;

			// Forward, up, and right axes of the camera (camera basis vectors)
			glm::vec3 forward;
			glm::vec3 up;
			glm::vec3 right;

			// Direction of world up
			glm::vec3 world_up;

			// Position of the camera
			glm::vec3 position;

			// Hitbox for the camera
			model::HitBox hitBox;

			// Orientation (pitch, yaw) of the camera
			// Stored in degrees
			glm::vec2 orientation;

			// Flag for handling the first mouse movement message that we receive
			bool first_mouse;

			// Last position that the mouse was in
			glm::vec2 last_mouse_pos;

			// Sensitivity values for camera rotations and motions
			float rotation_sensitivity;
			float movement_sensitivity;

			// No clip flag, unbind the camera from the terrain
			bool noClip;

			// Downwards velocity to the terrain, gravity
			float downVelocity;

			// Functions 
			// Detect any collisions with the given camera position and a models hitbox
			bool collisionDetected(std::vector<model::HitBox> hitBoxes, glm::vec3 cameraOrigin, glm::vec3 cameraSize)
			{
				// Loop over the vector of hitboxes in the world
				for (int i = 0; i < hitBoxes.size(); i++)
				{
					// Check each axis for overlap between the model hitbox and the camera hitbox
					bool xCheck = abs(cameraOrigin.x - hitBoxes[i].origin.x) < cameraSize.x + hitBoxes[i].size.x;
					bool yCheck = abs(cameraOrigin.y - hitBoxes[i].origin.y) < cameraSize.y + hitBoxes[i].size.y;
					bool zCheck = abs(cameraOrigin.z - hitBoxes[i].origin.z) < cameraSize.z + hitBoxes[i].size.z;
					if (xCheck && yCheck && zCheck) 
					{
						return true;
					}
					
				}
				return false;
			}
		};

		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! //
		// !!! BEWARE ALL YE WHO VENTURE PAST THIS POINT !!! //
		// !!!        HERE THERE BE MONSTERS             !!! //
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! //

		// Template hackiness to allow us to pass a member function to the GLFW API as a C-style function pointer
		// https://stackoverflow.com/a/39524069
		template <typename T>
		struct ActualType {
			typedef T type;
		};
		template <typename T>
		struct ActualType<T*> {
			typedef typename ActualType<T>::type type;
		};

		template <typename T, unsigned int n, typename CallerType>
		struct Callback;

		template <typename Ret, typename... Params, unsigned int n, typename CallerType>
		struct Callback<Ret(Params...), n, CallerType> {
			typedef Ret(*ret_cb)(Params...);
			template <typename... Args>
			static Ret callback(Args... args) {
				func(args...);
			}

			static ret_cb getCallback(std::function<Ret(Params...)> fn) {
				func = fn;
				return static_cast<ret_cb>(Callback<Ret(Params...), n, CallerType>::callback);
			}

			static std::function<Ret(Params...)> func;
		};

		template <typename Ret, typename... Params, unsigned int n, typename CallerType>
		std::function<Ret(Params...)> Callback<Ret(Params...), n, CallerType>::func;

		// Call this as (example for the GLFW scroll callback)
		// CCallbackWrapper(GLFWscrollfun, Camera)(std::bind(&Camera::scroll_back, &camera, _1, _2, _2))
#define CCallbackWrapper(ptrtype, callertype)                                                                          \
    utility::camera::Callback<utility::camera::ActualType<ptrtype>::type, __COUNTER__, callertype>::getCallback
	}  // namespace camera
}  // namespace utility


#endif  // UTILITY_CAMERA_HPP