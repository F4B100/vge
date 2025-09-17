//
// Created by fabio on 9/2/2025.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef VGE_PLATFORM_WAYLAND

#include <wayland-client.h>
#include <wayland-client-protocol.h>
#include "xdg-shell-client-protocol.h"
#include "xdg-activation-v1-client-protocol.h"
#include "../../utils/vgeThread.h"

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
typedef struct wl_seat wlSeat,* pWlSeat;
typedef struct xdg_surface xdgSurface,* pXdgSurface;
typedef struct xdg_toplevel xdgTopLevel,* pXdgToplevel;
typedef struct xdg_wm_base xdgWmBase,* pXdgWmBase;
typedef struct xdg_activation_token_v1 xdgActivationTokenV1,* pXdgActivationTokenV1;
typedef struct xdg_activation_v1 xdgActivationV1,* pXdgActivationV1;

typedef struct VgeWindow {
	uint32_t state;
	vgeMutex sizeMutex;
	uint32_t width, height;

	pXdgActivationTokenV1 xdgToken;

	pWlSurface surface;
	pXdgSurface xdgSurface;
	pXdgToplevel xdgToplevel;
	vgeThread thread;

	char *title;

	void *callbackData;
	void (*mouseMoveCallback)(struct VgeWindow *window, void *,int32_t x, int32_t y);
	void (*mouseLeftDownCallback)(struct VgeWindow *window, void *,int32_t x, int32_t y);
	void (*resizeCallback)(struct VgeWindow *window, void *,int32_t x, int32_t y);
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
	pXdgWmBase base;
	pXdgActivationV1 activator;
	pWlSeat seat;
	pWlKeyboard keyboard;
	pWlPointer pointer;
	uint32_t globalSerial;

	double StartTime;

	vgeSharedEventList windowEvents;
} vgeGlobalContext, *pVgeGlobalContext;

pVgeGlobalContext windowGlobalContext;

void registryGlobal(void *data, pWlRegistry wl_registry, uint32_t name, const char *interface, uint32_t version) {
	printf("interface: '%s', version: %d, name: %d\n",
			interface, version, name);
	pVgeGlobalContext context = data;
	if (strcmp(interface, wl_compositor_interface.name) == 0) {
		 context->compositor = wl_registry_bind(
			wl_registry,
			name,
			&wl_compositor_interface,
			version
		);
	}else if (strcmp(interface, xdg_wm_base_interface.name) == 0) {
		context->base = wl_registry_bind(
		   wl_registry,
		   name,
		   &xdg_wm_base_interface,
		   version
	   );
	}else if (strcmp(interface, xdg_activation_v1_interface.name) == 0) {
		context->activator = wl_registry_bind(
		   wl_registry,
		   name,
		   &xdg_activation_v1_interface,
		   version
	   );
	}else if (strcmp(interface, wl_seat_interface.name) == 0) {
		context->seat = wl_registry_bind(wl_registry, name, &wl_seat_interface, 1);
	}
};

void registryGlobalRemove(void *data, pWlRegistry wl_registry, uint32_t name) {

};

static const struct wl_registry_listener registryListener = {
	.global = registryGlobal,
	.global_remove = registryGlobalRemove
};

void handleShellPing(void* data, struct xdg_wm_base* shell, uint32_t serial)
{
	xdg_wm_base_pong(shell, serial);
}

struct xdg_wm_base_listener xdgBaseListener = {
	.ping = handleShellPing
};

void handleXdgSurfaceConfigure(void* data, struct xdg_surface* shellSurface, uint32_t serial)
{
	xdg_surface_ack_configure(shellSurface, serial);
	pVgeWindow window = data;
}


static const struct xdg_surface_listener xdgSurfaceListener = {
	.configure =  handleXdgSurfaceConfigure
};

void handleToplevelConfigure(void* data, struct xdg_toplevel* toplevel, int32_t width, int32_t height, struct wl_array* states) {
	pVgeWindow window = data;

	if (width != 0 && height != 0)
	{
		vgeMutexLock(&windowGlobalContext->windowEvents.mutex);

		if (windowGlobalContext->windowEvents.numEvents == MAX_EVENTS) {
			vgeCondWait(&windowGlobalContext->windowEvents.isFull, &windowGlobalContext->windowEvents.mutex);
		}

		 uint64_t eventNum = windowGlobalContext->windowEvents.numEvents;

		pVgeEventInfo eventWindowResize = &windowGlobalContext->windowEvents.events[eventNum];
		eventWindowResize->window = window;
		eventWindowResize->eventId = WINDOW_RESIZE;

		pVgeWindowResizeInfo eventData = malloc(sizeof(vgeWindowResizeInfo));
		eventData->x = width;
		eventData->y = height;

		eventWindowResize->data = eventData;

		windowGlobalContext->windowEvents.numEvents++;

		vgeMutexUnlock(&windowGlobalContext->windowEvents.mutex);
	}
}

void handleToplevelClose(void* data, struct xdg_toplevel* toplevel)
{
	pVgeWindow window = data;
	window->state = WINDOW_CLOSED;
}
static void handleToplevelWmCapabilities(void *data, struct xdg_toplevel *xdg_toplevel, struct wl_array *capabilities) {

}

static void handleToplevelConfigureBounds(void *data, struct xdg_toplevel *xdg_toplevel, int32_t width, int32_t height) {
	printf("Compositor suggests bounds: %dx%d\n", width, height);
}

static const struct xdg_toplevel_listener xdgTopLevelListener = {
	.configure = handleToplevelConfigure,
	.close = handleToplevelClose,
	.wm_capabilities = handleToplevelWmCapabilities,
	.configure_bounds = handleToplevelConfigureBounds
};

void tokenActivated(void *data, struct xdg_activation_token_v1 *xdg_activation_token_v1, const char *token) {
	pVgeWindow window = data;
	xdg_activation_v1_activate(windowGlobalContext->activator, token, window->surface);
	printf("activated!!!!\n");
}

const struct xdg_activation_token_v1_listener xdgActivationTokenV1Listener = {
	.done = tokenActivated
};

void pointer_button_handler(void *data, struct wl_pointer *pointer,
							uint32_t serial, uint32_t time,
							uint32_t button, uint32_t state) {
	pVgeWindow window = data;
	if (state == WL_POINTER_BUTTON_STATE_PRESSED) {
		windowGlobalContext->globalSerial = serial;

		window->xdgToken = xdg_activation_v1_get_activation_token(windowGlobalContext->activator);
		xdg_activation_token_v1_add_listener(window->xdgToken, &xdgActivationTokenV1Listener, window);
		xdg_activation_token_v1_set_surface(window->xdgToken, window->surface);
		xdg_activation_token_v1_set_serial(window->xdgToken, windowGlobalContext->globalSerial, windowGlobalContext->seat);
		xdg_activation_token_v1_commit(window->xdgToken);
	}
}

void pointerEnterHandler(void *data, struct wl_pointer *wl_pointer, uint32_t serial, struct wl_surface *surface, wl_fixed_t surface_x, wl_fixed_t surface_y) {
	printf("enter:%lf|%lf\n", wl_fixed_to_double(surface_x), wl_fixed_to_double(surface_y));
}

void pointerMotionHandler(void *data, struct wl_pointer *wl_pointer, uint32_t time, wl_fixed_t surface_x, wl_fixed_t surface_y) {
	printf("pointer:%lf|%lf\n", wl_fixed_to_double(surface_x), wl_fixed_to_double(surface_y));
}

void pointerLeaveHandler(void *data, struct wl_pointer *wl_pointer, uint32_t serial, struct wl_surface *surface) {

}

struct wl_pointer_listener wlPointerListener = {
	.enter = pointerEnterHandler,
	.leave = pointerLeaveHandler,
	.motion = pointerMotionHandler,
	.button = pointer_button_handler
};

void vgeInit() {
	windowGlobalContext = calloc(1, sizeof(vgeGlobalContext));

	windowGlobalContext->display = wl_display_connect(nullptr);
	windowGlobalContext->registry = wl_display_get_registry(windowGlobalContext->display);

	wl_registry_add_listener(windowGlobalContext->registry, &registryListener, windowGlobalContext);
	wl_display_roundtrip(windowGlobalContext->display);

	xdg_wm_base_add_listener(windowGlobalContext->base, &xdgBaseListener, windowGlobalContext);

	struct timespec time;
	clock_gettime(CLOCK_MONOTONIC, &time);
	windowGlobalContext->StartTime = time.tv_sec + time.tv_nsec / 1000000000.0f;
}

typedef struct VgeWindowThreadCreateInfo {
	vgeMutex createInfoMutex;
	vgeCond creationFinished;
	char *title;
	uint32_t width;
	uint32_t height;
	pVgeWindow window;
} vgeWindowThreadCreateInfo , *pVgeWindowThreadCreateInfo;

void *vgeWindowThreadFunc(void *data) {
	pVgeWindowThreadCreateInfo info = data;

	vgeMutexLock(&info->createInfoMutex);

	pVgeWindow window = info->window;
	window->title = info->title;
	vgeMutexLock(&window->sizeMutex);
	window->height = info->height;
	window->width = info->width;
	vgeMutexUnlock(&window->sizeMutex);

	window->surface = wl_compositor_create_surface(windowGlobalContext->compositor);

	window->xdgSurface = xdg_wm_base_get_xdg_surface(windowGlobalContext->base, window->surface);
	xdg_surface_add_listener(window->xdgSurface, &xdgSurfaceListener, window);

	window->xdgToplevel = xdg_surface_get_toplevel(window->xdgSurface);
	xdg_toplevel_add_listener(window->xdgToplevel, &xdgTopLevelListener, window);

	xdg_toplevel_set_title(window->xdgToplevel, window->title);
	xdg_toplevel_set_app_id(window->xdgToplevel, window->title);

	xdg_toplevel_set_min_size(window->xdgToplevel, window->width, info->height);
	xdg_toplevel_set_max_size(window->xdgToplevel, window->width, info->height);

	wl_surface_commit(window->surface);
	wl_display_roundtrip(windowGlobalContext->display);
	wl_surface_commit(window->surface);

	windowGlobalContext->pointer = wl_seat_get_pointer(windowGlobalContext->seat);
	wl_pointer_add_listener(windowGlobalContext->pointer, &wlPointerListener, window);

	vgeCondSignal(&info->creationFinished);
	vgeMutexUnlock(&info->createInfoMutex);

	while (window->state != WINDOW_CLOSED) {
		// Dispatch any queued events (non-blocking)
		wl_display_dispatch_pending(windowGlobalContext->display);

		// Flush outgoing requests to the compositor
		wl_display_flush(windowGlobalContext->display);

		// Block and wait for new events
		wl_display_dispatch(windowGlobalContext->display);
	}

	return nullptr;
}

pVgeWindow vgeWindowInit(int32_t width, int32_t height, char *title) {
	pVgeWindow window = (pVgeWindow)calloc(1, sizeof(vgeWindow));
	vgeMutexInit(&window->sizeMutex);

	vgeWindowThreadCreateInfo info;

	info.height = height;
	info.width = width;
	info.title = title;
	info.window = window;
	vgeMutexInit(&info.createInfoMutex);
	vgeCondInit(&info.creationFinished);

	vgeThreadCreate(&window->thread, vgeWindowThreadFunc, &info);

	vgeCondWait(&info.creationFinished, &info.createInfoMutex);
	vgeMutexUnlock(&info.createInfoMutex);
	return window;
}

void vgeGetWindowName(pVgeWindow window, char **name) {
	*name = window->title;
}

float vgeGetTimeSinceStart() {
	struct timespec time;
	clock_gettime(CLOCK_MONOTONIC, &time);
	return time.tv_sec + time.tv_nsec / 1000000000.0f - windowGlobalContext->StartTime;
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
					info.window->mouseMoveCallback(info.window, info.window->callbackData,infoEventMouseMove->x, infoEventMouseMove->y);
				}
				free(infoEventMouseMove);
				break;
			case WINDOW_MOUSE_CLICK_LEFT:
				printf("mouse Click");
				pVgeMouseClickLeftInfo infoEventClickLeft = info.data;
				if (info.window->mouseLeftDownCallback != nullptr) {
					printf("mouse click:%d|%d\n", infoEventClickLeft->x, infoEventClickLeft->y);
					info.window->mouseLeftDownCallback(info.window, info.window->callbackData, infoEventClickLeft->x, infoEventClickLeft->y);
				}
				free(infoEventClickLeft);
				break;
			case WINDOW_RESIZE:
				printf("resize\n");
				pVgeWindowResizeInfo infoEventWindowResize = info.data;
				if (info.window->resizeCallback != nullptr) {
					printf("resize:%d|%d\n", infoEventWindowResize->x, infoEventWindowResize->y);
					info.window->resizeCallback(info.window, info.window->callbackData, infoEventWindowResize->x, infoEventWindowResize->y);
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
	vgeMutexLock(&window->sizeMutex);
	*width = window->width;
	*height = window->height;
	vgeMutexUnlock(&window->sizeMutex);
}

void vgeSetCallbackData(pVgeWindow window, void *data) {
	printf("data %p", data);
	window->callbackData = data;
}

void vgeSetWindowSizeCallback(pVgeWindow window, void (*func)(pVgeWindow window, void *data, int32_t width, int32_t height)) {
	window->resizeCallback = func;
}

void vgeSetMouseMoveCallback(pVgeWindow window, void (*func)(pVgeWindow window, void *data, int32_t width, int32_t height)) {
	window->mouseMoveCallback = func;
}

#ifdef VGE_GRAPHICS_VULKAN

#include <vulkan/vulkan.h>
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