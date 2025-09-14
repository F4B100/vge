//
// Created by fabio on 9/2/2025.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../utils/vgeThread.h"

#ifdef VGE_PLATFORM_WIN32

#include <stdint.h>
#include <windows.h>
#include <windowsx.h>

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

typedef struct VgeWindow_t {
	HWND hWindow;
	uint32_t state;
	vgeThread thread;
	vgeMutex callbackMutex;
	void *data;
	void (*mouseMoveCallback)(struct VgeWindow_t *window, void *data, int32_t x, int32_t y);
	void (*mouseLeftDownCallback)(struct VgeWindow_t *window, void *data, int32_t x, int32_t y);
	void (*resizeCallback)(struct VgeWindow_t *window, void *data, int32_t x, int32_t y);
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

#define WINDOW_CLASS_NAME L"WindowClass"
#define WINDOW_MENU_NAME L"Menu_name"

#define WINDOW_CREATE_FAIL_MESSAGE L"Create Window failed!"

LRESULT CALLBACK vgeWindowsWProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#define WINDOW_PROCEDURE vgeWindowsWProc

pVgeGlobalContext windowGlobalContext;

void vgeSetStartTime() {
	LARGE_INTEGER frequency;
	LARGE_INTEGER counter;

	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&counter);

	windowGlobalContext->StartTime = (double)counter.QuadPart / frequency.QuadPart;
}

void vgeInit() {
	windowGlobalContext = malloc(sizeof(vgeGlobalContext));

	windowGlobalContext->hInstance = GetModuleHandle(nullptr);

	windowGlobalContext->windowClassName = malloc(sizeof(wchar_t) * 12);
	windowGlobalContext->menuName = malloc(sizeof(wchar_t) * 10);

	for (int i = 0; i < 12; i++) {
		const wchar_t CLASS_NAME[] = WINDOW_CLASS_NAME;
		windowGlobalContext->windowClassName[i] = CLASS_NAME[i];
	}
	for (int i = 0; i < 10; i++) {
		const wchar_t MENU_NAME[] = WINDOW_MENU_NAME;
		windowGlobalContext->menuName[i] = MENU_NAME[i];
	}

	WNDCLASSEXW wc = {};
	wc.cbSize = sizeof(wc);
	wc.hInstance = windowGlobalContext->hInstance;
	wc.lpfnWndProc = WINDOW_PROCEDURE;
	wc.lpszClassName = windowGlobalContext->windowClassName;
	wc.lpszMenuName = windowGlobalContext->menuName;
	wc.hCursor = LoadCursor(windowGlobalContext->hInstance, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

	RegisterClassExW(&wc);

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

void *vgeWindowThreadFunc(void *arg) {
	pVgeWindowThreadCreateInfo info = (pVgeWindowThreadCreateInfo)arg;

	vgeMutexLock(&info->mutex);

	pVgeWindow window = info->window;

	window->mouseLeftDownCallback = nullptr;
	window->mouseMoveCallback = nullptr;
	window->state = WINDOW_CLOSED;
	window->data = nullptr;

	vgeMutexInit(&window->callbackMutex);

	const uint32_t nameLen = strlen(info->windowName);
	wchar_t windowName[nameLen + 1];
	for (int i = 0; i < nameLen; ++i) {
		windowName[i] = (wchar_t)info->windowName[i];
	}
	windowName[nameLen] = L'\0';

	window->hWindow = CreateWindowExW(
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
		MessageBoxW(nullptr, WINDOW_CREATE_FAIL_MESSAGE, windowName, 0);
		return nullptr;
	}

	window->state = WINDOW_NORMAL;

	MSG message;

	vgeCondSignal(&info->cond);
	vgeMutexUnlock(&info->mutex);

	/*
	 * WARNING: after this point info will be freed by the calling thread, and it should not be used anymore
	 */

	while (window->state != WINDOW_CLOSED) {
		GetMessageW(&message, window->hWindow, 0, 0);
		TranslateMessage(&message);
		DispatchMessageW(&message);
	}
	return nullptr;
}

pVgeWindow vgeWindowInit(int32_t width, int32_t height, char *title) {
	pVgeWindow window = malloc(sizeof(vgeWindow));

	pVgeWindowThreadCreateInfo info = malloc(sizeof(vgeWindowThreadCreateInfo));

	info->window = window;
	info->width = width;
	info->height = height;
	info->windowName = title;

	vgeCondInit(&info->cond);
	vgeMutexInit(&info->mutex);
	vgeMutexLock(&info->mutex);

	vgeThreadCreate(&window->thread, vgeWindowThreadFunc, info);

	vgeCondWait(&info->cond, &info->mutex);
	ShowWindow(window->hWindow, 10);
	return window;
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

void vgeHandleEvents() {
	vgeMutexLock(&windowGlobalContext->windowEvents.mutex);
	for (int i = 0; i < windowGlobalContext->windowEvents.numEvents; ++i) {
		pVgeEventInfo info = &windowGlobalContext->windowEvents.events[i];
		switch (info->eventId) {
			case WINDOW_MOUSE_MOVE:
				pVgeMouseMoveInfo infoEventMouseMove = info->data;
				vgeMutexLock(&info->window->callbackMutex);
				if (info->window->mouseMoveCallback) {
					info->window->mouseMoveCallback(info->window, info->window->data, infoEventMouseMove->x, infoEventMouseMove->y);
				}
				vgeMutexUnlock(&info->window->callbackMutex);
				free(infoEventMouseMove);
				break;
			case WINDOW_MOUSE_CLICK_LEFT:
				pVgeMouseClickLeftInfo infoEventClickLeft = info->data;
				vgeMutexLock(&info->window->callbackMutex);
				if (info->window->mouseLeftDownCallback) {
					info->window->mouseLeftDownCallback(info->window, info->window->data, infoEventClickLeft->x, infoEventClickLeft->y);
				}
				vgeMutexUnlock(&info->window->callbackMutex);
				free(infoEventClickLeft);
				break;
			case WINDOW_RESIZE:
				pVgeWindowResizeInfo infoEventWindowResize = info->data;
				vgeMutexLock(&info->window->callbackMutex);
				if (info->window->resizeCallback) {
					info->window->resizeCallback(info->window, info->window->data, infoEventWindowResize->x, infoEventWindowResize->y);
				}
				vgeMutexUnlock(&info->window->callbackMutex);
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
	GetWindowTextW(window->hWindow, str, length + 1);


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

void vgeSetCallbackData(pVgeWindow window, void *data) {
	vgeMutexLock(&window->callbackMutex);
	window->data = data;
	vgeMutexUnlock(&window->callbackMutex);
}

void vgeSetWindowSizeCallback(pVgeWindow window, void (*func)(pVgeWindow window, void *data, int32_t width, int32_t height)) {
	vgeMutexLock(&window->callbackMutex);
	window->resizeCallback = func;
	vgeMutexUnlock(&window->callbackMutex);
}

void vgeSetMouseMoveCallback(pVgeWindow window, void (*func)(pVgeWindow window, void *data, int32_t width, int32_t height)) {
	vgeMutexLock(&window->callbackMutex);
	window->mouseMoveCallback = func;
	vgeMutexUnlock(&window->callbackMutex);
}

#ifdef VGE_GRAPHICS_VULKAN

#define NUM_REQUIRED_VGE_EXTENSIONS 3
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_win32.h>
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
#endif