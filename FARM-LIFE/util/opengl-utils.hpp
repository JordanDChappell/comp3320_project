#ifndef UTILITY_OPENGL_UTILS_HPP
#define UTILITY_OPENGL_UTILS_HPP

#include <array>
#include <fstream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "opengl-utils-error.hpp"

namespace utility {
namespace gl {
    // A wrapper for throwing exceptions based on OpenGL error codes
    // If no error is detector (code == GL_NO_ERROR) then nothing is thrown
    // Example: throw_gl_error(glGetError(), "Error doing something important");
    // -------------------------------------------------------------------------
    inline void throw_gl_error(const int& code, const std::string& msg) {
        if (code != GL_NO_ERROR) {
            throw std::system_error(code, opengl_error_category(), msg);
        }
    }

    // Create a smart enum to wrap shader enum types
    // ---------------------------------------------
    struct ShaderType {
        enum Value {
            VERTEX_SHADER   = GL_VERTEX_SHADER,
            FRAGMENT_SHADER = GL_FRAGMENT_SHADER,
            GEOMETRY_SHADER = GL_GEOMETRY_SHADER,
            UNKNOWN
        };
        Value value;
        ShaderType() : value(Value::UNKNOWN) {}
        ShaderType(const ShaderType& shader) : value(shader.value) {}
        ShaderType(ShaderType&& shader) : value(std::exchange(shader.value, UNKNOWN)) {}
        ShaderType(const unsigned int& shader_type) {
            switch (shader_type) {
                case GL_VERTEX_SHADER: value = Value::VERTEX_SHADER; break;
                case GL_FRAGMENT_SHADER: value = Value::FRAGMENT_SHADER; break;
                case GL_GEOMETRY_SHADER: value = Value::GEOMETRY_SHADER; break;
                case UNKNOWN: value = Value::UNKNOWN; break;
                default: throw_gl_error(GL_INVALID_ENUM, "Invalid shader type");
            }
        }
        ShaderType(const std::string& shader_type) {
            if (shader_type == "GL_VERTEX_SHADER" || shader_type == "VERTEX_SHADER") {
                value = Value::VERTEX_SHADER;
            }
            else if (shader_type == "GL_FRAGMENT_SHADER" || shader_type == "FRAGMENT_SHADER") {
                value = Value::FRAGMENT_SHADER;
            }
            else if (shader_type == "GL_GEOMETRY_SHADER" || shader_type == "GEOMETRY_SHADER") {
                value = Value::GEOMETRY_SHADER;
            }
            else if (shader_type == "UNKNOWN") {
                value = Value::UNKNOWN;
            }
            else {
                throw_gl_error(GL_INVALID_ENUM, "Invalid shader type '{}'");
            }
        }

        ShaderType& operator=(const ShaderType& shader) {
            value = shader.value;
            return *this;
        }
        ShaderType& operator=(ShaderType&& shader) {
            value = std::exchange(shader.value, UNKNOWN);
            return *this;
        }
        operator unsigned int() const {
            return value;
        }
        operator std::string() const {
            switch (value) {
                case Value::VERTEX_SHADER: return "VERTEX_SHADER";
                case Value::FRAGMENT_SHADER: return "FRAGMENT_SHADER";
                case Value::GEOMETRY_SHADER: return "GEOMETRY_SHADER";
                case Value::UNKNOWN: return "UNKNOWN";
                default:
                    throw_gl_error(GL_INVALID_ENUM, "Invalid shader type '{}'");
                    return "UNKNOWN";
            }
        }
    };

    // Create a smart enum to wrap texture enum types
    // ----------------------------------------------
    struct TextureType {
        enum Value {
            TEXTURE_1D                   = GL_TEXTURE_1D,
            TEXTURE_1D_ARRAY             = GL_TEXTURE_1D_ARRAY,
            TEXTURE_2D                   = GL_TEXTURE_2D,
            TEXTURE_2D_ARRAY             = GL_TEXTURE_2D_ARRAY,
            TEXTURE_2D_MULTISAMPLE       = GL_TEXTURE_2D_MULTISAMPLE,
            TEXTURE_2D_MULTISAMPLE_ARRAY = GL_TEXTURE_2D_MULTISAMPLE_ARRAY,
            TEXTURE_3D                   = GL_TEXTURE_3D,
            UNKNOWN
        };
        Value value;
        TextureType() : value(Value::UNKNOWN) {}
        TextureType(const TextureType& tex) : value(tex.value) {}
        TextureType(TextureType&& tex) : value(std::exchange(tex.value, UNKNOWN)) {}
        TextureType(const unsigned int& texture_type) {
            switch (texture_type) {
                case GL_TEXTURE_1D: value = Value::TEXTURE_1D; break;
                case GL_TEXTURE_1D_ARRAY: value = Value::TEXTURE_1D_ARRAY; break;
                case GL_TEXTURE_2D: value = Value::TEXTURE_2D; break;
                case GL_TEXTURE_2D_ARRAY: value = Value::TEXTURE_2D_ARRAY; break;
                case GL_TEXTURE_2D_MULTISAMPLE: value = Value::TEXTURE_2D_MULTISAMPLE; break;
                case GL_TEXTURE_2D_MULTISAMPLE_ARRAY: value = Value::TEXTURE_2D_MULTISAMPLE_ARRAY; break;
                case GL_TEXTURE_3D: value = Value::TEXTURE_3D; break;
                case UNKNOWN: value = Value::UNKNOWN; break;
                default: throw_gl_error(GL_INVALID_ENUM, "Invalid texture type '{}'");
            }
        }
        TextureType(const std::string& texture_type) {
            if (texture_type == "TEXTURE_1D" || texture_type == "GL_TEXTURE_1D") {
                value = Value::TEXTURE_1D;
            }
            else if (texture_type == "TEXTURE_1D_ARRAY" || texture_type == "GL_TEXTURE_1D_ARRAY") {
                value = Value::TEXTURE_1D_ARRAY;
            }
            else if (texture_type == "TEXTURE_2D" || texture_type == "GL_TEXTURE_2D") {
                value = Value::TEXTURE_2D;
            }
            else if (texture_type == "TEXTURE_2D_ARRAY" || texture_type == "GL_TEXTURE_2D_ARRAY") {
                value = Value::TEXTURE_2D_ARRAY;
            }
            else if (texture_type == "TEXTURE_2D_MULTISAMPLE" || texture_type == "GL_TEXTURE_2D_MULTISAMPLE") {
                value = Value::TEXTURE_2D_MULTISAMPLE;
            }
            else if (texture_type == "TEXTURE_2D_MULTISAMPLE_ARRAY"
                     || texture_type == "GL_TEXTURE_2D_MULTISAMPLE_ARRAY") {
                value = Value::TEXTURE_2D_MULTISAMPLE_ARRAY;
            }
            else if (texture_type == "TEXTURE_3D" || texture_type == "GL_TEXTURE_3D") {
                value = Value::TEXTURE_3D;
            }
            else if (texture_type == "UNKNOWN") {
                value = Value::UNKNOWN;
            }
            else {
                throw_gl_error(GL_INVALID_ENUM, "Invalid texture type '{}'");
            }
        }

        TextureType& operator=(const TextureType& tex) {
            value = tex.value;
            return *this;
        }
        TextureType& operator=(TextureType&& tex) {
            value = std::exchange(tex.value, UNKNOWN);
            return *this;
        }
        operator unsigned int() const {
            return value;
        }
        operator std::string() const {
            switch (value) {
                case GL_TEXTURE_1D: return "TEXTURE_1D";
                case GL_TEXTURE_1D_ARRAY: return "TEXTURE_1D_ARRAY";
                case GL_TEXTURE_2D: return "TEXTURE_2D";
                case GL_TEXTURE_2D_ARRAY: return "TEXTURE_2D_ARRAY";
                case GL_TEXTURE_2D_MULTISAMPLE: return "TEXTURE_2D_MULTISAMPLE";
                case GL_TEXTURE_2D_MULTISAMPLE_ARRAY: return "TEXTURE_2D_MULTISAMPLE_ARRAY";
                case GL_TEXTURE_3D: return "TEXTURE_3D";
                case UNKNOWN: return "UNKNOWN";
                default:
                    throw_gl_error(GL_INVALID_ENUM, "Invalid texture type '{}'");
                    return "UNKNOWN";
            }
        }
    };

    // Create a smart enum to wrap texture style enum
    // ----------------------------------------------
    struct TextureStyle {
        enum Value { TEXTURE_DIFFUSE = 0, TEXTURE_SPECULAR = 1, UNKNOWN };
        Value value;
        TextureStyle() : value(Value::UNKNOWN) {}
        TextureStyle(const TextureStyle& tex) : value(tex.value) {}
        TextureStyle(TextureStyle&& tex) : value(std::exchange(tex.value, UNKNOWN)) {}
        TextureStyle(const unsigned int& texture_style) {
            switch (texture_style) {
                case Value::TEXTURE_DIFFUSE: value = Value::TEXTURE_DIFFUSE; break;
                case Value::TEXTURE_SPECULAR: value = Value::TEXTURE_SPECULAR; break;
                case Value::UNKNOWN: value = Value::UNKNOWN; break;
                default: throw_gl_error(GL_INVALID_ENUM, "Invalid texture style '{}'");
            }
        }
        TextureStyle(const std::string& texture_style) {
            if (texture_style == "TEXTURE_DIFFUSE") {
                value = Value::TEXTURE_DIFFUSE;
            }
            else if (texture_style == "TEXTURE_SPECULAR") {
                value = Value::TEXTURE_SPECULAR;
            }
            else if (texture_style == "UNKNOWN") {
                value = Value::UNKNOWN;
            }
            else {
                throw_gl_error(GL_INVALID_ENUM, "Invalid texture style '{}'");
            }
        }

        TextureStyle& operator=(const TextureStyle& tex) {
            value = tex.value;
            return *this;
        }
        TextureStyle& operator=(TextureStyle&& tex) {
            value = std::exchange(tex.value, UNKNOWN);
            return *this;
        }
        operator unsigned int() const {
            return value;
        }
        operator std::string() const {
            switch (value) {
                case Value::TEXTURE_DIFFUSE: return "TEXTURE_DIFFUSE";
                case Value::TEXTURE_SPECULAR: return "TEXTURE_SPECULAR";
                case Value::UNKNOWN: return "UNKNOWN";
                default:
                    throw_gl_error(GL_INVALID_ENUM, "Invalid texture style '{}'");
                    return "UNKNOWN";
            }
        }
    };

    // Create a wrapper for OpenGL shader programs
    // -------------------------------------------
    struct shader_program {
        // Create an empty program
        shader_program() {
            // Create a shader program
            program = glCreateProgram();
            throw_gl_error(glGetError(), "Failed to create shader program");
        }
        shader_program(const shader_program& prog) = delete;
        // : shaders(prog.shaders), program(prog.program), uniforms(prog.uniforms) {}
        shader_program(shader_program&& prog) noexcept
            : shaders(std::move(prog.shaders))
            , program(std::exchange(prog.program, 0))
            , uniforms(std::move(prog.uniforms)) {}
        // Clean up all references
        ~shader_program() {
            for (auto& shader : shaders) {
                if (glIsShader(program) == GL_TRUE) {
#ifndef NDEBUG
                    std::cout << "Deleting shader" << std::endl;
#endif
                    glDeleteShader(shader);

                }
            }
            shaders.clear();
            if (glIsProgram(program) == GL_TRUE) {
#ifndef NDEBUG
                std::cout << "Deleting program" << std::endl;
#endif
                glDeleteProgram(program);
                throw_gl_error(glGetError(), "Failed to delete shader program");
            }
        }
        shader_program& operator=(const shader_program& prog) = delete;  // {
        //     shaders  = prog.shaders;
        //     program  = prog.program;
        //     uniforms = prog.uniforms;
        //     return *this;
        // }
        shader_program& operator=(shader_program&& prog) noexcept {
            shaders  = std::move(prog.shaders);
            program  = std::exchange(prog.program, 0);
            uniforms = std::move(prog.uniforms);
            return *this;
        }

        // Add shader source code from a file
        // ------------------------------------------------------------------------------------
        // shader_source: Path to file that contains the shader source code
        // shader_type: The type of the shader that is being added (vertex, fragment, geometry)
        // ------------------------------------------------------------------------------------
        void add_shader(const std::string& shader_source, const ShaderType& shader_type) {
            // Load shader source code from the specified file
            // See: http://stackoverflow.com/a/116228
            std::ifstream data(shader_source, std::ios::in);
            if (!data.good()) {
                throw std::system_error(std::error_code(ENOENT, std::system_category()),
                                        "Failed to open shader file '{}'");
            }
            std::stringstream stream;
            stream << data.rdbuf();
            std::string code(stream.str());

            // Create the shader
            unsigned int shader_id = glCreateShader(shader_type);
            throw_gl_error(glGetError(), "Failed to create shader for {}");

            // glShaderSource expects an array of strings
            const char* shader_src = code.c_str();
            glShaderSource(shader_id, 1, &shader_src, nullptr);
            throw_gl_error(glGetError(), "Failed to load shader source for {}");

            // Compile the shader
            glCompileShader(shader_id);

            // Check for compile errors
            int success;
            glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
      

            if (success == GL_FALSE) {
                std::string shader_type_str(shader_type);
                int length = 0;
                glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &length);
                std::string info_log(length, 0);
                glGetShaderInfoLog(shader_id, length, nullptr, &info_log[0]);

  
            }

            // No errors, add shader to list of all shaders
            shaders.push_back(shader_id);
        }

        // Link all the shaders into a shader program
        // ------------------------------------------
        void link() {
            // Make sure we actually have some shaders to link together
            if (shaders.empty()) {
                throw std::system_error(std::error_code(EINVAL, std::system_category()),
                                        "Can't link a program with no shaders.");
            }

            // Make sure the program is valid
            if (glIsProgram(program) == GL_TRUE) {
                // Attach each of the provided shaders to the progrm
                for (auto shader : shaders) {
                    if (glIsShader(shader) == GL_TRUE) {
                        glAttachShader(program, shader);
                    }
                    else {

                    }
                }

                // Link all of the shaders together
                glLinkProgram(program);

                // Check for linking errors
                int success;
                glGetProgramiv(program, GL_LINK_STATUS, &success);

                if (success == GL_FALSE) {
                    int length = 0;
                    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
                    std::string info_log(length, 0);
                    glGetProgramInfoLog(program, length, nullptr, &info_log[0]);
 

                }

                // Program is linked, we can discard the shaders now
                for (auto& shader : shaders) {
                    glDeleteShader(shader);

                }
                shaders.clear();

#ifndef NDEBUG
                list_all_attributes();
                find_all_uniforms(true);
#endif
                find_all_uniforms(false);
            }
            else {
                throw std::system_error(std::error_code(EINVAL, std::system_category()), "Invalid Program!");
            }
        }

        void list_all_attributes() {
            GLint count;
            GLint size;                  // size of the variable
            GLenum type;                 // type of the variable (float, vec3 or mat4, etc)
            const GLsizei bufSize = 64;  // maximum name length
            GLchar name[bufSize];        // variable name in GLSL
            GLsizei length;              // name length

            glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &count);

            for (int i = 0; i < count; i++) {
                glGetActiveAttrib(program, static_cast<GLuint>(i), bufSize, &length, &size, &type, name);

            }
        }

        void find_all_uniforms(const bool& list = false) {
            GLint count;
            GLint size;                  // size of the variable
            GLenum type;                 // type of the variable (float, vec3 or mat4, etc)
            const GLsizei bufSize = 64;  // maximum name length
            GLchar name[bufSize];        // variable name in GLSL
            GLsizei length;              // name length

            glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &count);
            if (list) {
            }

            for (int i = 0; i < count; i++) {
                glGetActiveUniform(program, static_cast<GLuint>(i), bufSize, &length, &size, &type, name);
                uniforms[name] = glGetUniformLocation(program, name);
                if (list) {
                }
            }
        }

        // Make this program the currently active program
        // ----------------------------------------------
        void use() {
            glUseProgram(program);
        }
        // Deactive this program
        // ---------------------
        void release() {
            glUseProgram(0);
        }

        // Get the location of a named uniform in the program
        // --------------------------------------------------
        // uniform: The name of the uniform to find
        // --------------------------------------------------
        int get_uniform_location(const std::string& uniform) {
            glUseProgram(program);
            if (uniforms.find(uniform) == uniforms.end()) {
                uniforms[uniform] = glGetUniformLocation(program, uniform.c_str());
            }
            return uniforms[uniform];
        }
        // Different overloads for setting a named uniform to the provided value
        // ---------------------------------------------------------------------
        // uniform: The name of the uniform to set
        // value: The value to set the uniform to
        // ---------------------------------------------------------------------
        void set_uniform(const std::string& uniform, const int& value) {
            glUniform1i(get_uniform_location(uniform), value);

        }
        void set_uniform(const std::string& uniform, const float& value) {
            glUniform1f(get_uniform_location(uniform), value);

        }
        void set_uniform(const std::string& uniform, const glm::mat4& value) {
            glUniformMatrix4fv(get_uniform_location(uniform), 1, GL_FALSE, glm::value_ptr(value));

        }
        void set_uniform(const std::string& uniform, const glm::vec4& value) {
            glUniform4fv(get_uniform_location(uniform), 1, glm::value_ptr(value));

        }
        void set_uniform(const std::string& uniform, const glm::vec3& value) {
            glUniform3fv(get_uniform_location(uniform), 1, glm::value_ptr(value));
         
        }
        void set_uniform(const std::string& uniform, const std::array<float, 4>& value) {
            glUniform4f(get_uniform_location(uniform), value[0], value[1], value[2], value[3]);
            
        }

        // Allow this program wrapper to be passed OpenGL functions
        // OpenGL functions expect an unsigned int
        // --------------------------------------------------------
        operator unsigned int() {
            return program;
        }

    private:
        std::vector<unsigned int> shaders;
        unsigned int program;
        std::map<std::string, int> uniforms;
    };

    // Create a wrapper for OpenGL vertex arrays
    // -----------------------------------------
    struct vertex_array {
        // Create a single vertex array
        // ----------------------------
        vertex_array() {
            glGenVertexArrays(1, &VAO);
        }
        vertex_array(const vertex_array& va) = delete;  // : VAO(va.VAO) {}
        vertex_array(vertex_array&& va) noexcept : VAO(std::exchange(va.VAO, 0)) {}
        // Delete the vertex array
        // -----------------------
        ~vertex_array() {
            if (glIsVertexArray(VAO) == GL_TRUE) {
#ifndef NDEBUG
                std::cout << "Deleting vertex array" << std::endl;
#endif
                glDeleteVertexArrays(1, &VAO);
            }
        }
        vertex_array& operator=(const vertex_array& va) = delete;  // {
        //     VAO = va.VAO;
        //     return *this;
        // }
        vertex_array& operator=(vertex_array&& va) {
            VAO = std::exchange(va.VAO, 0);
            return *this;
        }

        // Bind the vertex array and make it active
        // ----------------------------------------
        void bind() {
            glBindVertexArray(VAO);
        }
        // Deactivate the vertex array
        // ---------------------------
        void unbind() {
            glBindVertexArray(0);
        }

        // Add a vertex attribute to the vertex array
        // ------------------------------------------
        template <typename Scalar>
        void add_vertex_attrib(const unsigned int& location,
                               const int& size,
                               const int& width,
                               const unsigned int& type,
                               const bool& normalised,
                               const unsigned int& offset) {
            bind();
            glVertexAttribPointer(location,
                                  size,
                                  type,
                                  normalised ? GL_TRUE : GL_FALSE,
                                  width * sizeof(Scalar),
                                  (void*) (offset * sizeof(Scalar)));
            glEnableVertexAttribArray(location);
        }

        // Allow this vertex array wrapper to be passed OpenGL functions
        // OpenGL functions expect an unsigned int
        // -------------------------------------------------------------
        operator unsigned int() const {
            return VAO;
        }

    private:
        unsigned int VAO;
    };

    // Create a wrapper for OpenGL vertex buffers
    // ------------------------------------------
    struct vertex_buffer {
        // Create a single vertex buffer
        // -----------------------------
        vertex_buffer() {
            glGenBuffers(1, &VBO);
        }
        vertex_buffer(const vertex_buffer& vb) = delete;  // : VBO(vb.VBO) {}
        vertex_buffer(vertex_buffer&& vb) noexcept : VBO(std::exchange(vb.VBO, 0)) {}
        // Delete the vertex buffer
        // ------------------------
        ~vertex_buffer() {
            if (glIsBuffer(VBO) == GL_TRUE) {
#ifndef NDEBUG
                std::cout << "Deleting vertex buffer" << std::endl;
#endif
                glDeleteBuffers(1, &VBO);
            }
        }
        vertex_buffer& operator=(const vertex_buffer& vb) = delete;  //{
        //     VBO = vb.VBO;
        //     return *this;
        // }
        vertex_buffer& operator=(vertex_buffer&& vb) {
            VBO = std::exchange(vb.VBO, 0);
            return *this;
        }

        // Bind the vertex buffer and make it active
        // -----------------------------------------
        void bind() {
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
        }
        // Deactivate the vertex buffer
        // ----------------------------
        void unbind() {
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

        // Copy vertex buffer data to the GPU
        // ----------------------------------
        template <int N>
        void copy_data(const std::array<float, N>& vertices, const unsigned int& draw_method) {
            bind();
            glBufferData(GL_ARRAY_BUFFER, N * sizeof(float), &vertices[0], draw_method);
        }
        template <typename T>
        void copy_data(const std::vector<T>& vertices, const unsigned int& draw_method) {
            bind();
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(T), &vertices[0], draw_method);
        }

        // Allow this vertex buffer wrapper to be passed OpenGL functions
        // OpenGL functions expect an unsigned int
        // --------------------------------------------------------------
        operator unsigned int() const {
            return VBO;
        }

    private:
        unsigned int VBO;
    };

    // Create a wrapper for OpenGL element buffers
    // -------------------------------------------
    struct element_buffer {
        // Create a single element buffer
        // ------------------------------
        element_buffer() {
            glGenBuffers(1, &EBO);
        }
        element_buffer(const element_buffer& eb) = delete;  // : EBO(eb.EBO) {}
        element_buffer(element_buffer&& eb) noexcept : EBO(std::exchange(eb.EBO, 0)) {}
        // Delete the element buffer
        // -------------------------
        ~element_buffer() {
            if (glIsBuffer(EBO) == GL_TRUE) {
#ifndef NDEBUG
                std::cout << "Deleting element buffer" << std::endl;
#endif
                glDeleteBuffers(1, &EBO);
            }
        }
        element_buffer& operator=(const element_buffer& eb) = delete;  // {
        //     EBO = eb.EBO;
        //     return *this;
        // }
        element_buffer& operator=(element_buffer&& eb) {
            EBO = std::exchange(eb.EBO, 0);
            return *this;
        }

        // Bind the element buffer and make it active
        // ------------------------------------------
        void bind() {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        }
        // Deactivate the element buffer
        // -----------------------------
        void unbind() {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }

        // Copy the element buffer data to the GPU
        // ---------------------------------------
        template <int N>
        void copy_data(const std::array<unsigned int, N>& indices, const unsigned int& draw_method) {
            bind();
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, N * sizeof(unsigned int), &indices[0], draw_method);
        }
        void copy_data(const std::vector<unsigned int>& indices, const unsigned int& draw_method) {
            bind();
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], draw_method);
        }

        // Allow this element buffer wrapper to be passed OpenGL functions
        // OpenGL functions expect an unsigned int
        // ---------------------------------------------------------------
        operator unsigned int() const {
            return EBO;
        }

    private:
        unsigned int EBO;
    };

    // Create a wrapper for OpenGL textures
    // ------------------------------------
    struct texture {
        // Create an uninitialsed texture
        // ------------------------------
        texture(const TextureType& texture_type, const TextureStyle& texture_style = TextureStyle::TEXTURE_DIFFUSE) {
            glGenTextures(1, &tex);
            this->texture_type  = texture_type;
            this->texture_style = texture_style;
            texture_data.clear();
        }
        // Create a texture and initialise it with the given image file
        // ------------------------------------------------------------
        // image: Path to the image file to load
        // texture_type: The type of the texture that we are loading
        // ------------------------------------------------------------
        texture(const std::string& image,
                const TextureType& texture_type,
                const TextureStyle& texture_style = TextureStyle::TEXTURE_DIFFUSE) {
            glGenTextures(1, &tex);
            this->texture_type  = texture_type;
            this->texture_style = texture_style;
            this->texture_path  = image;

            unsigned char* data = SOIL_load_image(image.c_str(), &width, &height, &channels, SOIL_LOAD_AUTO);
            texture_data.clear();

          
            texture_data.assign(data, data + (width * height * channels));
            SOIL_free_image_data(data);
            
        }
        texture(const texture& other_texture) = delete;
        texture(texture&& other_texture) noexcept
            : tex(std::exchange(other_texture.tex, 0))
            , texture_type(std::exchange(other_texture.texture_type, TextureType::UNKNOWN))
            , texture_style(std::exchange(other_texture.texture_style, TextureStyle::UNKNOWN))
            , texture_path(std::move(other_texture.texture_path))
            , width(std::exchange(other_texture.width, 0))
            , height(std::exchange(other_texture.height, 0))
            , channels(std::exchange(other_texture.channels, 0))
            , texture_data(std::move(other_texture.texture_data)) {}
        // Delete the texture
        // ------------------
        ~texture() {
            if (glIsTexture(tex) == GL_TRUE) {
#ifndef NDEBUG
                std::cout << "Deleting texture " << tex << std::endl;
#endif
                glDeleteTextures(1, &tex);

            }
        }
        texture& operator=(const texture& other_texture) = delete;
        texture& operator                                =(texture&& other_texture) {
            tex           = std::exchange(other_texture.tex, 0);
            texture_type  = std::exchange(other_texture.texture_type, TextureType::UNKNOWN);
            texture_style = std::exchange(other_texture.texture_style, TextureStyle::UNKNOWN);
            texture_path  = std::move(other_texture.texture_path);
            width         = std::exchange(other_texture.width, 0);
            height        = std::exchange(other_texture.height, 0);
            channels      = std::exchange(other_texture.channels, 0);
            texture_data  = std::move(other_texture.texture_data);
            return *this;
        }

        // Load texture data from the provided array
        // -------------------------------------------------
        // data: Array of bytes to load into the texture
        // width: Width of the texture data
        // height: Height of the texture data
        // channels: Number of channels in the texture data
        // -------------------------------------------------
        void load_data(const std::vector<unsigned char>& data,
                       const unsigned int& width,
                       const unsigned int& height,
                       const unsigned int& channels) {
            texture_data.clear();
            texture_data.assign(data.begin(), data.end());
            this->width    = width;
            this->height   = height;
            this->channels = channels;
        }
        // Load texture data from the provided array
        // -------------------------------------------------
        // data: Array of bytes to load into the texture
        // width: Width of the texture data
        // height: Height of the texture data
        // channels: Number of channels in the texture data
        // -------------------------------------------------
        void load_data(const unsigned char* data,
                       const unsigned int& width,
                       const unsigned int& height,
                       const unsigned int& channels) {
            texture_data.clear();
            texture_data.assign(data, data + (width * height * channels));
            this->width    = width;
            this->height   = height;
            this->channels = channels;
        }

        // Bind the texture and make it active
        // ---------------------------------------------
        // unit: The texture unit to bind the texture to
        // ---------------------------------------------
        void bind(const unsigned int& unit = GL_TEXTURE0) {
            glActiveTexture(unit);
            glBindTexture(texture_type, tex);
        }
        // Deactivate the texture
        // ----------------------
        void unbind() {
            glBindTexture(texture_type, 0);
        }

        // Load the texture data on to the GPU
        // -----------------------------------
        void generate(const unsigned int& mipmap_level, const unsigned int& pixel_type = -1) {
            unsigned int pixel_format = pixel_type;
            if (pixel_format == -1) {
                switch (channels) {
                    case 1: pixel_format = GL_RED; break;
                    case 3: pixel_format = GL_RGB; break;
                    case 4: pixel_format = GL_RGBA; break;
                }
            }
            switch (texture_type) {
                case TextureType::TEXTURE_2D:
                    bind();
                    glTexImage2D(texture_type,
                                 mipmap_level,
                                 pixel_format,
                                 width,
                                 height,
                                 0,
                                 pixel_format,
                                 GL_UNSIGNED_BYTE,
                                 texture_data.data());
                    break;
                default:
					break;
            }
        }
        // Generate mipmapped textures
        // ---------------------------
        void generate_mipmap() {
            switch (texture_type) {
                case TextureType::TEXTURE_2D:
                    bind();
                    glGenerateMipmap(GL_TEXTURE_2D);
                    break;
                default:
					break;
            }
        }

        // Tell OpenGL how to handle texture wrapping
        // ------------------------------------------
        void texture_wrap(const unsigned int& s_wrap,
                          const unsigned int& t_wrap,
                          const unsigned int& unit = GL_TEXTURE0) {
            bind(unit);
            glTexParameteri(texture_type, GL_TEXTURE_WRAP_S, s_wrap);
            glTexParameteri(texture_type, GL_TEXTURE_WRAP_T, t_wrap);
        }

        // Tell OpenGL how to handle texture minifying and magnifying
        // ----------------------------------------------------------
        void texture_filter(const unsigned int& min_filter,
                            const unsigned int& mag_filter,
                            const unsigned int& unit = GL_TEXTURE0) {
            bind(unit);
            glTexParameteri(texture_type, GL_TEXTURE_MIN_FILTER, min_filter);
            glTexParameteri(texture_type, GL_TEXTURE_MAG_FILTER, mag_filter);
        }

        // Allow this texture  wrapper to be passed OpenGL functions
        // OpenGL functions expect an unsigned int
        // ---------------------------------------------------------
        operator unsigned int() const {
            return tex;
        }

        TextureStyle style() const {
            return texture_style;
        }

        std::string path() const {
            return texture_path;
        }

    private:
        unsigned int tex;
        TextureType texture_type;
        TextureStyle texture_style;
        std::string texture_path;
        int width, height, channels;
        std::vector<unsigned char> texture_data;
    };
}  // namespace gl
}  // namespace utility


#endif  // UTILITY_OPENGL_UTILS_HPP