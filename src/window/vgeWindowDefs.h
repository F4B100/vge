//
// Created by fabio on 21-05-2025.
//

#ifndef VGEWINDOWDEFS_H
#define VGEWINDOWDEFS_H

#include <stdint.h>
#include "../utils/vgeThread.h"

// window State
#define WINDOW_NORMAL 0x00
#define WINDOW_CLOSED 0x10

// event Defs
typedef struct VgeMouseMoveInfo {
	uint32_t x;
	uint32_t y;
} vgeMouseMoveInfo, *pVgeMouseMoveInfo;
#define WINDOW_MOUSE_MOVE 0x00000001
typedef struct VgeMouseClickLeftInfo {
	uint32_t x;
	uint32_t y;
} vgeMouseClickLeftInfo, *pVgeMouseClickLeftInfo;
#define WINDOW_MOUSE_CLICK_LEFT 0x00000002

typedef struct VgeWindowResizeInfo {
	uint32_t x;
	uint32_t y;
} vgeWindowResizeInfo, *pVgeWindowResizeInfo;
#define WINDOW_RESIZE 0x00000011
typedef uint32_t vgeEventID;

typedef struct VgeEventInfo {
	vgeEventID eventId;
	pVgeWindow window;
	void *data;
} vgeEventInfo, *pVgeEventInfo;

#define MAX_EVENTS 10000

typedef struct VgeSharedEventList {
	vgeMutex mutex;
	vgeCond isFull;
	uint64_t numEvents;
	vgeEventInfo events[MAX_EVENTS];
} vgeSharedEventList, *pVgeSharedEventList;

#ifdef VGE_PLATFORM_WIN32
#define UNICODE
#define _UNICODE
#include <windows.h>
#include <windowsx.h>

typedef struct VgeWindow_t {
	HWND hWindow;
	uint32_t state;
	vgeThread thread;
	void (*mouseMoveCallback)(struct VgeWindow_t *window, uint32_t x, uint32_t y);
	void (*mouseLeftDownCallback)(struct VgeWindow_t *window, uint32_t x, uint32_t y);
	void (*resizeCallback)(struct VgeWindow_t *window, uint32_t x, uint32_t y);
} vgeWindow, *pVgeWindow;

typedef struct VgeGlobalContext_t {
    wchar_t *windowClassName;
    wchar_t *menuName;
    double StartTime;
    HINSTANCE hInstance;

	vgeSharedEventList windowEvents;
} vgeGlobalContext, *pVgeGlobalContext;

#elifdef VGE_PLATFORM_WAYLAND

#include <wayland-client.h>
#include <wayland-client-protocol.h>
#include "xdg-shell-client-protocol.h"

typedef struct wl_display wlDisplay,* pWlDisplay;
typedef struct wl_registry wlRegistry ,* pWlRegistry;
typedef struct wl_compositor wlCompositor,* pWlCompositor;
typedef struct wl_surface wlSurface,* pWlSurface;
typedef struct wl_keyboard wlKeyboard,* pWlKeyboard;
typedef struct wl_pointer wlPointer,* pWlPointer;
typedef struct wl_shm wlShm,* pWlShm;
typedef struct wl_shm_pool wlShmPool,* pWlShmPool;
typedef struct xdg_surface xdgSurface,* pXdgSurface;
typedef struct xdg_toplevel xdgTopLevel,* pXdgToplevel;
typedef struct xdg_wm_base xdgWmBase,* pXdgWmBase;

typedef struct VgeWindow {
    uint32_t state;
    pWlDisplay display;
    pWlRegistry registry;
    pWlCompositor compositor;
    pWlShm shm;
    pWlShmPool shm_pool;
    pXdgWmBase xdg_wm_base;
    pWlSurface surface;
    pXdgSurface xdg_surface;
    pXdgToplevel xdg_toplevel;

	void (*mouseMoveCallback)(struct VgeWindow *window, uint32_t x, uint32_t y);
	void (*mouseLeftDownCallback)(struct VgeWindow *window, uint32_t x, uint32_t y);
	void (*resizeCallback)(struct VgeWindow *window, uint32_t x, uint32_t y);
} vgeWindow, *pVgeWindow;

typedef struct VgeGlobalContext_t {
	uint32_t numWindows;
	pVgeWindow *windows;
} vgeGlobalContext, *pVgeGlobalContext;

#else
#include <GLFW/glfw3.h>
typedef struct VgeWindow {
    GLFWwindow* window;
} vgeWindow;
#endif

#endif //VGEWINDOWDEFS_H
