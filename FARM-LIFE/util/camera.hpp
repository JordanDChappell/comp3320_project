#ifndef A1_CAMERA_HPP
#define A1_CAMERA_HPP

#include <functional>
#include <algorithm>

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
				position = glm::vec3(0.0f, 0.0f, 3.0f);
				right = glm::normalize(glm::cross(forward, up));

				orientation = glm::vec2(-90.0f, 0.0f);
				update_camera_basis();

				first_mouse = true;
				last_mouse_pos = glm::vec2(static_cast<float>(width) * 0.5f, static_cast<float>(height) * 0.5f);
				rotation_sensitivity = 0.05f;
				movement_sensitivity = 0.005f;
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
				// calculate pitch and yaw
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

			void move_y_position(float distance) {
				position.y += distance;
			}

			void invert_pitch() {
				orientation.y = -orientation.y;
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
			// -----------
			void move_left() {
				position -= right * movement_sensitivity;
			}

			// Strafe right
			// ------------
			void move_right() {
				position += right * movement_sensitivity;
			}

			// Move forward
			// ------------
			void move_forward() {
				position += forward * movement_sensitivity;
			}

			// Move backward
			// -------------
			void move_backward() {
				position -= forward * movement_sensitivity;
			}

			// Move up
			// -------
			void move_up() {
				position += up * movement_sensitivity;
			}

			// Move down
			// ---------
			void move_down() {
				position -= up * movement_sensitivity;
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