//
// Created by fabio on 21-05-2025.
//

#include "vgeWindow.h"


#ifdef VGE_PLATFORM_WIN32

void vgeInit() {

}

vgeWindow * vgeWindowInit(const int32_t width, const int32_t height, const int8_t *title, GLFWmonitor *monitor, GLFWwindow *share) {
    vgeWindow *window = calloc(1, sizeof(vgeWindow));
    if (window == NULL) {
        return NULL;
    }
    window->window = NULL;
    window->window = glfwCreateWindow(width, height, title, monitor, share);
    glfwShowWindow(window->window);
    return window;
}

#elifdef VGE_PLATFORM_WAYLAND

void vgeInit() {

}

vgeWindow * vgeWindowInit(const int32_t width, const int32_t height, const int8_t *title, GLFWmonitor *monitor, GLFWwindow *share) {
    vgeWindow *window = calloc(1, sizeof(vgeWindow));
    if (window == NULL) {
        return NULL;
    }
    window->window = NULL;
    window->window = glfwCreateWindow(width, height, title, monitor, share);
    glfwShowWindow(window->window);
    return window;
}

#else

void vgeInit() {

}

vgeWindow * vgeWindowInit(const int32_t width, const int32_t height, const int8_t *title, GLFWmonitor *monitor, GLFWwindow *share) {
    vgeWindow *window = calloc(1, sizeof(vgeWindow));
    if (window == NULL) {
        return NULL;
    }
    window->window = NULL;
    window->window = glfwCreateWindow(width, height, title, monitor, share);
    glfwShowWindow(window->window);
    return window;
}

#endif
