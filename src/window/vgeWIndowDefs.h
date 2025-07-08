//
// Created by fabio on 21-05-2025.
//

#ifndef VGEWINDOWDEFS_H
#define VGEWINDOWDEFS_H

#ifdef VGE_PLATFORM_WIN32
#include <GLFW/glfw3.h>
typedef struct VgeWindow {
    GLFWwindow* window;
} vgeWindow;
#elifdef VGE_PLATFORM_WAYLAND
#include <GLFW/glfw3.h>
typedef struct VgeWindow {
    GLFWwindow* window;
} vgeWindow;
#else
#include <GLFW/glfw3.h>
typedef struct VgeWindow {
    GLFWwindow* window;
} vgeWindow;
#endif

#endif //VGEWINDOWDEFS_H
