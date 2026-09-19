#pragma once

// We only use GLFW's window/cursor API, not its OpenGL headers, so tell GLFW
// not to pull in any GL header (openFrameworks loads GLEW itself).
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <map>

// Cross-cutting helper for changing the OS mouse cursor. openFrameworks does not
// expose a cursor-shape API, so we talk to GLFW (the windowing backend used on
// desktop) directly.
namespace cursor {

enum class Type {
    Arrow,
    ResizeHorizontal, // left/right edges
    ResizeVertical,   // top/bottom edges
    ResizeNWSE,       // diagonal: top-left <-> bottom-right
    ResizeNESW        // diagonal: top-right <-> bottom-left
};

inline int toGlfwShape(Type type) {
    switch (type) {
        case Type::ResizeHorizontal: return GLFW_HRESIZE_CURSOR;
        case Type::ResizeVertical:   return GLFW_VRESIZE_CURSOR;
        case Type::ResizeNWSE:       return GLFW_RESIZE_NWSE_CURSOR;
        case Type::ResizeNESW:       return GLFW_RESIZE_NESW_CURSOR;
        case Type::Arrow:
        default:                     return GLFW_ARROW_CURSOR;
    }
}

// Sets the OS cursor for the current window. Standard cursors are created once
// and cached for the lifetime of the process.
inline void apply(Type type) {
    GLFWwindow* window = glfwGetCurrentContext();
    if (!window) {
        return;
    }

    const int shape = toGlfwShape(type);

    static std::map<int, GLFWcursor*> cache;
    auto it = cache.find(shape);
    if (it == cache.end()) {
        it = cache.emplace(shape, glfwCreateStandardCursor(shape)).first;
    }
    if (it->second) {
        glfwSetCursor(window, it->second);
    }
}

} // namespace cursor
