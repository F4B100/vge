//
// Created by fabio on 9/2/2025.
//
#include "../vgeWindow.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef VGE_PLATFORM_WAYLAND

#include <wayland-client.h>
#include <wayland-client-protocol.h>
#include "xdg-shell-client-protocol.h"
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

pVgeGlobalContext windowGlobalContext;

void registryGlobal(void *data, pWlRegistry wl_registry, uint32_t name, const char *interface, uint32_t version) {
	printf("%d\n", name);
};

void registryGlobalRemove(void *data, pWlRegistry wl_registry, uint32_t name) {

};

struct wl_registry_listener registryListener = {
	.global = registryGlobal,
	.global_remove = registryGlobalRemove
};

void vgeInit() {
	windowGlobalContext = calloc(1, sizeof(vgeGlobalContext));

	windowGlobalContext->display = wl_display_connect(nullptr);
	windowGlobalContext->registry = wl_display_get_registry(windowGlobalContext->display);

	wl_registry_add_listener(windowGlobalContext->registry, &registryListener, windowGlobalContext);
	wl_display_roundtrip(windowGlobalContext->display);

}

pVgeWindow vgeWindowInit(int32_t width, int32_t height, char *title) {
	return nullptr;
}

void vgeGetWindowName(pVgeWindow window, char **name) {

}

double vgeGetTimeSinceStart() {
	return 0.0f;
}

uint32_t vgeIsWindowClosed(pVgeWindow window) {
	return window->state == WINDOW_CLOSED;
}

void  vgeHandleEvents() {
	vgeMutexLock(&windowGlobalContext->windowEvents.mutex);
	for (int i = 0; i < windowGlobalContext->windowEvents.numEvents; ++i) {
		vgeEventInfo info = windowGlobalContext->windowEvents.events[i];
		switch (info.eventId) {
			case WINDOW_MOUSE_MOVE:
				printf("mouse");
				pVgeMouseMoveInfo infoEventMouseMove = info.data;
				if (info.window->mouseMoveCallback != nullptr) {
					printf("mouse:%d|%d\n", infoEventMouseMove->x, infoEventMouseMove->y);
					info.window->mouseMoveCallback(info.window ,infoEventMouseMove->x, infoEventMouseMove->y);
				}
				free(infoEventMouseMove);
				break;
			case WINDOW_MOUSE_CLICK_LEFT:
				printf("mouse Click");
				pVgeMouseClickLeftInfo infoEventClickLeft = info.data;
				if (info.window->mouseLeftDownCallback != nullptr) {
					printf("mouse click:%d|%d\n", infoEventClickLeft->x, infoEventClickLeft->y);
					info.window->mouseLeftDownCallback(info.window ,infoEventClickLeft->x, infoEventClickLeft->y);
				}
				free(infoEventClickLeft);
				break;
			case WINDOW_RESIZE:
				printf("resize\n");
				pVgeWindowResizeInfo infoEventWindowResize = info.data;
				if (info.window->resizeCallback != nullptr) {
					printf("resize:%d|%d\n", infoEventWindowResize->x, infoEventWindowResize->y);
					info.window->resizeCallback(info.window ,infoEventWindowResize->x, infoEventWindowResize->y);
				}
				free(infoEventWindowResize);
				break;
			default:
				break;
		}
	}

	windowGlobalContext->windowEvents.numEvents = 0;
	vgeCondSignal(&windowGlobalContext->windowEvents.isFull);
	vgeMutexUnlock(&windowGlobalContext->windowEvents.mutex);
}

void vgeGetContentSize(pVgeWindow window, uint32_t *width, uint32_t *height) {
	return;
}

void vgeSetWindowSizeCallback(pVgeWindow window, void (*func)(pVgeWindow window, int32_t width, int32_t height)) {
	window->resizeCallback = func;
}

void vgeSetMouseMoveCallback(pVgeWindow window, void (*func)(pVgeWindow window, int32_t width, int32_t height)) {
	window->mouseMoveCallback = func;
}

#ifdef VGE_GRAPHICS_VULKAN

#include <vulkan/vulkan_wayland.h>

#define NUM_REQUIRED_VGE_EXTENSIONS 3
char *vgeExtensions[] = {
	VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME,
	VK_KHR_SURFACE_EXTENSION_NAME,
	VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME
};

const char **vgeGetVulkanExtensions(uint32_t *numExtensions, uint32_t numExtra, const char** extra) {
	char **extensions = malloc(sizeof(char **) * (NUM_REQUIRED_VGE_EXTENSIONS + numExtra));

	for (int i = 0; i < NUM_REQUIRED_VGE_EXTENSIONS; ++i) {
		extensions[i] = malloc(sizeof(char) * strlen(vgeExtensions[i]));
		strcpy(extensions[i], vgeExtensions[i]);
	}
	for (int i = 0; i < numExtra; ++i) {
		extensions[NUM_REQUIRED_VGE_EXTENSIONS + i] = malloc(sizeof(char) * strlen(extra[i]));
		strcpy(extensions[NUM_REQUIRED_VGE_EXTENSIONS + i], extra[i]);
	}
	*numExtensions = NUM_REQUIRED_VGE_EXTENSIONS + numExtra;
	return (const char **)extensions;
}

void vgeCreateVulkanWindowSurface(vgeWindow *window, VkInstance instance, VkSurfaceKHR *toCreate) {
	VkWaylandSurfaceCreateInfoKHR createInfo = {
		.sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR,
		.display = windowGlobalContext->display,
		.surface = window->surface,
		.flags = 0,
		.pNext = nullptr
	};
	vkCreateWaylandSurfaceKHR(instance, &createInfo, nullptr, toCreate);
}

uint8_t physicalDeviceSupportsPresentation(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex) {
	const VkBool32 presentSupport = vkGetPhysicalDeviceWaylandPresentationSupportKHR(physicalDevice, queueFamilyIndex, windowGlobalContext->display);

	return presentSupport == VK_TRUE;
}

#endif

#endif