#ifndef UTILITY_OPENGL_ERROR_CATEGORY_HPP
#define UTILITY_OPENGL_ERROR_CATEGORY_HPP

#include <system_error>

enum class opengl_error_code {
    NO_ERROR                      = GL_NO_ERROR,
    INVALID_ENUM                  = GL_INVALID_ENUM,
    INVALID_VALUE                 = GL_INVALID_VALUE,
    INVALID_OPERATION             = GL_INVALID_OPERATION,
    INVALID_FRAMEBUFFER_OPERATION = GL_INVALID_FRAMEBUFFER_OPERATION,
    OUT_OF_MEMORY                 = GL_OUT_OF_MEMORY,
    UNKNOWN
};

namespace std {
template <>
struct is_error_condition_enum<opengl_error_code> : public true_type {};
}  // namespace std


class opengl_error_category_t : public std::error_category {
public:
    virtual const char* name() const noexcept;

    virtual std::error_condition default_error_condition(int code) const noexcept;

    virtual bool equivalent(const std::error_code& code, int condition) const noexcept;

    virtual std::string message(int code) const noexcept;
};

const std::error_category& opengl_error_category() {
    static opengl_error_category_t instance;
    return instance;
}

std::error_condition make_error_condition(opengl_error_code e) {
    return std::error_condition(static_cast<int>(e), opengl_error_category());
}

const char* opengl_error_category_t::name() const noexcept {
    return "opengl_error_category";
}

std::error_condition opengl_error_category_t::default_error_condition(int code) const noexcept {
    using gle = opengl_error_code;
    switch (code) {
        case GL_NO_ERROR: return std::error_condition(gle::NO_ERROR);
        case GL_INVALID_ENUM: return std::error_condition(gle::INVALID_ENUM);
        case GL_INVALID_VALUE: return std::error_condition(gle::INVALID_VALUE);
        case GL_INVALID_OPERATION: return std::error_condition(gle::INVALID_OPERATION);
        case GL_INVALID_FRAMEBUFFER_OPERATION: return std::error_condition(gle::INVALID_FRAMEBUFFER_OPERATION);
        case GL_OUT_OF_MEMORY: return std::error_condition(gle::OUT_OF_MEMORY);
        default: return std::error_condition(gle::UNKNOWN);
    }
}

bool opengl_error_category_t::equivalent(const std::error_code& code, int condition) const noexcept {
    return *this == code.category() && static_cast<int>(default_error_condition(code.value()).value()) == condition;
}

std::string opengl_error_category_t::message(int code) const noexcept {
    switch (code) {
        case GL_NO_ERROR: return "No error";
        case GL_INVALID_ENUM: return "Invalid enum";
        case GL_INVALID_VALUE: return "Invalid value";
        case GL_INVALID_OPERATION: return "Invalid operation";
        case GL_INVALID_FRAMEBUFFER_OPERATION: return "Invalid framebuffer operation";
        case GL_OUT_OF_MEMORY: return "Out of memory";
        default: return "Unknown error";
    }
}

#endif  // UTILITY_OPENGL_ERROR_CATEGORY_HPP