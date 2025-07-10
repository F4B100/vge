//
// Created by fabio on 21-05-2025.
//

#ifndef VGEWINDOWDEFS_H
#define VGEWINDOWDEFS_H

#include <stdint.h>

#ifdef VGE_PLATFORM_WIN32
#define UNICODE
#define _UNICODE
#include <windows.h>
#include <windowsx.h>
#include "../utils/vgeThread.h"

// window State
#define WINDOW_NORMAL 0x00
#define WINDOW_CLOSED 0x10

// event Defs
#define WINDOW_MOUSE_MOVE 0x00000001
#define WINDOW_MOUSE_CLICK_LEFT 0x00000002

#define WINDOW_RESIZE 0x00000011
typedef uint32_t vgeEventID;


typedef struct VgeEventInfo {
	vgeEventID eventId;
	void *data;
} vgeEventInfo, *pVgeEventInfo;

typedef struct VgeSharedEventList {
	vgeMutex mutex;
	uint64_t count;
	pVgeEventInfo events;
} vgeSharedEventList, *pVgeSharedEventList;

typedef struct VgeGlobalContext_t {
    wchar_t *windowClassName;
    wchar_t *menuName;
    double StartTime;
    HINSTANCE hInstance;

	vgeSharedEventList windowEvents;
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
