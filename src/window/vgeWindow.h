//
// Created by fabio on 21-05-2025.
//

#ifndef VGEWINDOW_H
#define VGEWINDOW_H

#include <stdint.h>
#include "../utils/vgeThread.h"

// window State
#define WINDOW_NORMAL 0x00
#define WINDOW_CLOSED 0x10

// event Defs
typedef struct VgeMouseMoveInfo {
	int32_t x;
	int32_t y;
} vgeMouseMoveInfo, *pVgeMouseMoveInfo;
#define WINDOW_MOUSE_MOVE 0x00000001
typedef struct VgeMouseClickLeftInfo {
	int32_t x;
	int32_t y;
} vgeMouseClickLeftInfo, *pVgeMouseClickLeftInfo;
#define WINDOW_MOUSE_CLICK_LEFT 0x00000002

typedef struct VgeWindowResizeInfo {
	int32_t x;
	int32_t y;
} vgeWindowResizeInfo, *pVgeWindowResizeInfo;
#define WINDOW_RESIZE 0x00000011
#define UNICODE
typedef uint32_t vgeEventID;

#ifdef VGE_PLATFORM_WIN32
#include <windows.h>
#include <windowsx.h>

typedef struct VgeWindow_t {
	HWND hWindow;
	uint32_t state;
	vgeThread thread;
	void (*mouseMoveCallback)(struct VgeWindow_t *window, int32_t x, int32_t y);
	void (*mouseLeftDownCallback)(struct VgeWindow_t *window, int32_t x, int32_t y);
	void (*resizeCallback)(struct VgeWindow_t *window, int32_t x, int32_t y);
} vgeWindow, *pVgeWindow;

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
#include "wayland/xdg-shell-client-protocol.h"

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
    pWlShm shm;
    pWlShmPool shm_pool;
    pXdgWmBase xdg_wm_base;
    pWlSurface surface;
    pXdgSurface xdg_surface;
    pXdgToplevel xdg_toplevel;
	pVgeThread thread;

	void (*mouseMoveCallback)(struct VgeWindow *window, int32_t x, int32_t y);
	void (*mouseLeftDownCallback)(struct VgeWindow *window, int32_t x, int32_t y);
	void (*resizeCallback)(struct VgeWindow *window, int32_t x, int32_t y);
} vgeWindow, *pVgeWindow;

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

typedef struct VgeGlobalContext_t {
	pWlDisplay display;
	pWlRegistry registry;
	pWlCompositor compositor;

	double StartTime;

	vgeSharedEventList windowEvents;
} vgeGlobalContext, *pVgeGlobalContext;

#endif

void vgeInit();

pVgeWindow vgeWindowInit(int32_t width, int32_t height, char *title);
void *vgeWindowThreadFunc(void *arg);

void vgeSetStartTime();
double vgeGetTimeSinceStart();

void vgeGetContentSize(pVgeWindow window, uint32_t *width, uint32_t *height);

void vgeHandleEvents();

uint32_t vgeIsWindowClosed(pVgeWindow window);

#ifdef VGE_PLATFORM_WIN32
#define WINDOW_CLASS_NAME L"WindowClass"
#define WINDOW_MENU_NAME L"Menu_name"

#define WINDOW_CREATE_FAIL_MESSAGE L"Create Window failed!"

LRESULT CALLBACK vgeWindowsWProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#define WINDOW_PROCEDURE vgeWindowsWProc

#endif

// Callback Setting
void vgeSetWindowSizeCallback(pVgeWindow window, void (*func)(pVgeWindow window, int32_t width, int32_t height));

void vgeSetMouseMoveCallback(pVgeWindow window, void (*func)(pVgeWindow window, int32_t width, int32_t height));

// Get section
void vgeGetWindowName(pVgeWindow window, char **name);

#ifdef VGE_GRAPHICS_VULKAN
#include <vulkan/vulkan.h>

const char **vgeGetVulkanExtensions(uint32_t *numExtensions,uint32_t numExtra, const char** extra);

void vgeCreateVulkanWindowSurface(pVgeWindow window, VkInstance instance,VkSurfaceKHR *toCreate);

uint8_t physicalDeviceSupportsPresentation(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex);
#endif

#endif //VGEWINDOW_H
