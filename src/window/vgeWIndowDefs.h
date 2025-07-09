//
// Created by fabio on 21-05-2025.
//

#ifndef VGEWINDOWDEFS_H
#define VGEWINDOWDEFS_H

#ifdef VGE_PLATFORM_WIN32
#include <stdint.h>
#define UNICODE
#define _UNICODE
#include <windows.h>

// window State
#define WINDOW_NORMAL 0x00
#define WINDOW_CLOSED 0x10

typedef struct VgeGlobalContext_t {
    wchar_t *windowClassName;
    wchar_t *menuName;
    double timeSinceStart;
    HINSTANCE hInstance;
} vgeGlobalContext, *pVgeGlobalContext;

typedef struct VgeWindow_t {
    HWND hWindow;
    uint32_t state;
    void (*mouseMoveCallback)(struct VgeWindow_t *window, uint32_t x, uint32_t y);
    void (*mouseLeftDownCallback)(struct VgeWindow_t *window, uint32_t x, uint32_t y);
    void (*resizeCallback)(struct VgeWindow_t *window, uint32_t x, uint32_t y);
} vgeWindow, *pVgeWindow;


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
