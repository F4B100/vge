//
// Created by fabio on 21-05-2025.
//

#include "vgeWindow.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef VGE_PLATFORM_WIN32
#include <stdio.h>
#include <string.h>
#include <vulkan/vulkan_win32.h>

pVgeGlobalContext windowGlobalContext;

void vgeInit() {
	windowGlobalContext = malloc(sizeof(vgeGlobalContext));

	windowGlobalContext->hInstance = GetModuleHandle(nullptr);

	const wchar_t CLASS_NAME[] = WINDOW_CLASS_NAME;
	const wchar_t MENU_NAME[] = WINDOW_MENU_NAME;

	windowGlobalContext->windowClassName = malloc(sizeof(wchar_t) * 12);
	windowGlobalContext->menuName = malloc(sizeof(wchar_t) * 10);

	for (int i = 0; i < 12; i++) {
		windowGlobalContext->windowClassName[i] = CLASS_NAME[i];
	}
	for (int i = 0; i < 10; i++) {
		windowGlobalContext->menuName[i] = MENU_NAME[i];
	}

	WNDCLASSEX wc = {};
	wc.cbSize = sizeof(wc);
	wc.hInstance = windowGlobalContext->hInstance;
	wc.lpfnWndProc = WINDOW_PROCEDURE;
	wc.lpszClassName = windowGlobalContext->windowClassName;
	wc.lpszMenuName = windowGlobalContext->menuName;
	wc.hCursor = LoadCursor(windowGlobalContext->hInstance, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

	RegisterClassEx(&wc);

	vgeSetStartTime();

	vgeMutexInit(&windowGlobalContext->windowEvents.mutex);
	windowGlobalContext->windowEvents.numEvents = 0;
}

typedef struct VgeWindowThreadCreateInfo {
	vgeMutex mutex;
	vgeCond cond;
	uint32_t width;
	uint32_t height;
	char *windowName;
	pVgeWindow window;
} vgeWindowThreadCreateInfo, *pVgeWindowThreadCreateInfo;

pVgeWindow vgeWindowInit(int32_t width, int32_t height, char *title) {
	pVgeWindow window = malloc(sizeof(vgeWindow));

	pVgeWindowThreadCreateInfo info = malloc(sizeof(struct VgeWindowThreadCreateInfo));

	info->window = window;
	info->width = width;
	info->height = height;
	info->windowName = title;

	vgeCondInit(&info->cond);
	vgeMutexInit(&info->mutex);
	vgeMutexLock(&info->mutex);

	vgeThreadCreate(&window->thread, vgeWindowThreadFunc, info);

	vgeCondWait(&info->cond, &info->mutex);
	free(info);
	return window;
}

void *vgeWindowThreadFunc(void *arg) {
	pVgeWindowThreadCreateInfo info = (pVgeWindowThreadCreateInfo)arg;

	vgeMutexLock(&info->mutex);

	pVgeWindow window = info->window;

	window->mouseLeftDownCallback = nullptr;
	window->mouseMoveCallback = nullptr;
	window->state = WINDOW_CLOSED;

	const uint32_t nameLen = strlen(info->windowName);
	wchar_t windowName[nameLen + 1];
	for (int i = 0; i < nameLen; ++i) {
		windowName[i] = (wchar_t)info->windowName[i];
	}
	windowName[nameLen] = L'\0';

	window->hWindow = CreateWindowEx(
		0,
		windowGlobalContext->windowClassName,
		windowName,
		WS_OVERLAPPEDWINDOW,

		0, 0,
		info->width, info->height,

		nullptr,
		nullptr,
		windowGlobalContext->hInstance,
		window
	);

	if (window->hWindow == NULL) {
		MessageBox(nullptr, WINDOW_CREATE_FAIL_MESSAGE, windowName, 0);
		return nullptr;
	}

	ShowWindow(window->hWindow, 10);

	window->state = WINDOW_NORMAL;

	MSG message;

	vgeCondSignal(&info->cond);
	vgeMutexUnlock(&info->mutex);

	/*
	 * WARNING: after this point info will be freed by the calling thread, and it should not be used anymore
	 */

	while (window->state != WINDOW_CLOSED) {
		GetMessage(&message, window->hWindow, 0, 0);
		TranslateMessage(&message);
		DispatchMessage(&message);
	}
	return nullptr;
}

void vgeSetStartTime() {
	LARGE_INTEGER frequency;
	LARGE_INTEGER counter;

	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&counter);

	windowGlobalContext->StartTime = (double)counter.QuadPart / frequency.QuadPart;
}

double vgeGetTimeSinceStart() {
	LARGE_INTEGER frequency;
	LARGE_INTEGER counter;

	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&counter);

	const double newTime = (double)counter.QuadPart / frequency.QuadPart;

	return newTime - windowGlobalContext->StartTime;
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
	RECT dim;
	if (GetClientRect(window->hWindow, &dim) == 0) {
		*width = -1;
		*height = -1;
		return;
	}
	*width = dim.right - dim.left;
	*height = dim.bottom - dim.top;
}

LRESULT CALLBACK vgeWindowsWProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	pVgeWindow window;

	if (uMsg == WM_NCCREATE) {
		CREATESTRUCT* cs = (CREATESTRUCT*)lParam;
		window = (pVgeWindow)cs->lpCreateParams;
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)window);
	} else {
		window = (pVgeWindow)GetWindowLongPtr(hwnd, GWLP_USERDATA);
	}

	uint64_t eventNum;
	switch (uMsg) {
		case WM_MOUSEMOVE:
			vgeMutexLock(&windowGlobalContext->windowEvents.mutex);

			if (windowGlobalContext->windowEvents.numEvents == MAX_EVENTS) {
				vgeCondWait(&windowGlobalContext->windowEvents.isFull, &windowGlobalContext->windowEvents.mutex);
			}

			eventNum = windowGlobalContext->windowEvents.numEvents;
			windowGlobalContext->windowEvents.numEvents++;

			pVgeEventInfo eventMouseMove = &windowGlobalContext->windowEvents.events[eventNum];
			eventMouseMove->window = window;
			eventMouseMove->eventId = WINDOW_MOUSE_MOVE;

			pVgeMouseMoveInfo eventDataMouseMove = malloc(sizeof(vgeMouseMoveInfo));
			eventDataMouseMove->x = GET_X_LPARAM(lParam);
			eventDataMouseMove->y = GET_Y_LPARAM(lParam);

			eventMouseMove->data = eventDataMouseMove;

			vgeMutexUnlock(&windowGlobalContext->windowEvents.mutex);
			break;
		case WM_LBUTTONDOWN:
			vgeMutexLock(&windowGlobalContext->windowEvents.mutex);

			if (windowGlobalContext->windowEvents.numEvents == MAX_EVENTS) {
				vgeCondWait(&windowGlobalContext->windowEvents.isFull, &windowGlobalContext->windowEvents.mutex);
			}

			eventNum = windowGlobalContext->windowEvents.numEvents;
			windowGlobalContext->windowEvents.numEvents++;

			pVgeEventInfo eventMouseLeftDown = &windowGlobalContext->windowEvents.events[eventNum];
			eventMouseLeftDown->window = window;
			eventMouseLeftDown->eventId = WINDOW_MOUSE_CLICK_LEFT;

			pVgeMouseClickLeftInfo eventDataMouseClickLeft = malloc(sizeof(vgeMouseClickLeftInfo));
			eventDataMouseClickLeft->x = GET_X_LPARAM(lParam);
			eventDataMouseClickLeft->y = GET_Y_LPARAM(lParam);

			eventMouseLeftDown->data = eventDataMouseClickLeft;

			vgeMutexUnlock(&windowGlobalContext->windowEvents.mutex);
			break;
		case WM_CLOSE:
			DestroyWindow(hwnd);
			window->state = WINDOW_CLOSED;
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_SIZING:
			vgeMutexLock(&windowGlobalContext->windowEvents.mutex);

			if (windowGlobalContext->windowEvents.numEvents == MAX_EVENTS) {
				vgeCondWait(&windowGlobalContext->windowEvents.isFull, &windowGlobalContext->windowEvents.mutex);
			}

			eventNum = windowGlobalContext->windowEvents.numEvents;

			RECT *sizingRect = (RECT *)lParam;

			pVgeEventInfo eventWindowResize = &windowGlobalContext->windowEvents.events[eventNum];
			eventWindowResize->window = window;
			eventWindowResize->eventId = WINDOW_RESIZE;

			pVgeWindowResizeInfo eventData = malloc(sizeof(vgeWindowResizeInfo));
			eventData->x = sizingRect->right - sizingRect->left;
			eventData->y = sizingRect->bottom - sizingRect->top;

			eventWindowResize->data = eventData;

			windowGlobalContext->windowEvents.numEvents++;

			vgeMutexUnlock(&windowGlobalContext->windowEvents.mutex);
			break;
		default:
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void vgeGetWindowName(pVgeWindow window, char **name) {
	if (!IsWindow(window->hWindow))
		return;
	int32_t length = GetWindowTextLength(window->hWindow);
	LPWSTR str = malloc(sizeof(WCHAR) * (length + 1));
	GetWindowText(window->hWindow, str, length + 1);


	int32_t size = WideCharToMultiByte(
		CP_ACP,
		WC_NO_BEST_FIT_CHARS,
		str,
		-1,
		nullptr,
		0,
		nullptr,
		nullptr
	);

	*name = malloc(size);

	WideCharToMultiByte(
		CP_ACP,
		WC_NO_BEST_FIT_CHARS,
		str,
		-1,
		*name,
		size,
		nullptr,
		nullptr
	);
	free(str);
}

#ifdef VGE_GRAPHICS_VULKAN

#define NUM_REQUIRED_VGE_EXTENSIONS 3
char *vgeExtensions[] = {
	VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
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

void vgeCreateVulkanWindowSurface(vgeWindow *window, VkInstance instance,VkSurfaceKHR *toCreate) {
	VkWin32SurfaceCreateInfoKHR info = {
		.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
		.pNext = nullptr,
		.flags = 0,
		.hinstance= windowGlobalContext->hInstance,
		.hwnd = window->hWindow
	};

	if (vkCreateWin32SurfaceKHR(instance, &info, nullptr, toCreate) != VK_SUCCESS) {
		fprintf(stderr, "Failed to create window surface.\n");
	}
}

uint8_t physicalDeviceSupportsPresentation(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex) {
	const VkBool32 presentSupport = vkGetPhysicalDeviceWin32PresentationSupportKHR(physicalDevice, queueFamilyIndex);

	return presentSupport == VK_TRUE;
}

#endif

#elifdef VGE_PLATFORM_WAYLAND

vgeGlobalContext context = {
	.numWindows = 0,
	.windows = nullptr
};

void vgeInit() {
	glfwInit();
}

pVgeWindow vgeWindowInit(int32_t width, int32_t height, char *title) {
	printf("%p\n", context.windows);
	context.windows = realloc(context.windows, (context.numWindows + 1) * sizeof(pVgeWindow));

	printf("%p\n", context.windows);

	context.windows[context.numWindows]->window = glfwCreateWindow(width, height, title, nullptr, nullptr);
	glfwShowWindow(context.windows[context.numWindows]->window);
	context.numWindows++;
	return context.windows[context.numWindows - 1];
}

void vgeGetWindowName(pVgeWindow window, char **name) {
	*name = (char *) glfwGetWindowTitle(window->window);
}

double vgeGetTimeSinceStart() {
	return glfwGetTime();
}

uint32_t vgeIsWindowClosed(pVgeWindow window) {
	return glfwWindowShouldClose(window->window);
}

void vgeHandleEvents() {
	glfwPollEvents();
}

void vgeGetContentSize(pVgeWindow window, uint32_t *width, uint32_t *height) {
	glfwGetWindowSize(window->window, width, height);
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

void vgeCreateVulkanWindowSurface(vgeWindow *window, VkInstance instance,VkSurfaceKHR *toCreate) {
	glfwCreateWindowSurface(instance, window->window, nullptr, toCreate);
}

uint8_t physicalDeviceSupportsPresentation(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex) {
	const VkBool32 presentSupport = vkGetPhysicalDeviceWaylandPresentationSupportKHR(physicalDevice, queueFamilyIndex, glfwGetWaylandDisplay());

	return presentSupport == VK_TRUE;
}

#endif

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

void vgeSetWindowSizeCallback(pVgeWindow window, void (*func)(pVgeWindow window, uint32_t width, uint32_t height)) {
	window->resizeCallback = func;
}


void vgeSetMouseMoveCallback(pVgeWindow window, void (*func)(pVgeWindow window, uint32_t width, uint32_t height)) {
	window->mouseMoveCallback = func;
}
