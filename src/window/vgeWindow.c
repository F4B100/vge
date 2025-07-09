//
// Created by fabio on 21-05-2025.
//

#include "vgeWindow.h"

#ifdef VGE_PLATFORM_WIN32
#include <string.h>
#include <vulkan/vulkan_win32.h>

#include "../../cmake-build-debug-event-trace/_deps/glfw-src/src/win32_platform.h"

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
}

pVgeWindow vgeWindowInit(const int32_t width, const int32_t height, const char *title) {
	pVgeWindow window = malloc(sizeof(vgeWindow));
	window->mouseLeftDownCallback = nullptr;
	window->mouseMoveCallback = nullptr;
	window->state = WINDOW_CLOSED;

	const uint32_t nameLen = strlen(title);
	wchar_t windowName[nameLen];
	for (int i = 0; i < nameLen; ++i) {
		windowName[i] = (wchar_t)title[i];
	}

	window->hWindow = CreateWindowEx(
		0,
		windowGlobalContext->windowClassName,
		windowName,
		WS_OVERLAPPEDWINDOW,

		0, 0,
		width, height,

		nullptr,
		nullptr,
		windowGlobalContext->hInstance,
		NULL
	);

	if (window->hWindow == NULL) {
		MessageBox(nullptr, WINDOW_CREATE_FAIL_MESSAGE, windowName, 0);
		return nullptr;
	}

	ShowWindow(window->hWindow, 10);

	window->state = WINDOW_NORMAL;
	return window;
}

LRESULT CALLBACK vgeWindowsWProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void vgeGetWindowName(pVgeWindow window, char **name) {
	uint32_t length = GetWindowTextLength(window->hWindow);
	const LPWSTR str = malloc(sizeof(WCHAR) * (length + 1));
	GetWindowText(window->hWindow, str, length + 1);



	uint32_t size = WideCharToMultiByte(
		CP_ACP,
		WC_NO_BEST_FIT_CHARS,
		str,
		-1,
		NULL,
		0,
		NULL,
		NULL
	);

	*name = malloc(size);

	WideCharToMultiByte(
		CP_ACP,
		WC_NO_BEST_FIT_CHARS,
		str,
		-1,
		*name,
		size,
		NULL,
		NULL
	);
	free(str);
}

#ifdef VGE_GRAPHICS_VULKAN

#define NUM_REQUIRED_VGE_EXTENSIONS 3
char *extensions[] = {
	VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
	VK_KHR_SURFACE_EXTENSION_NAME,
	VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME
};

char **vgeGetVulkanExtensions(uint32_t *numExtensions,uint32_t numExtra,char** extra) {
	char **extensions = malloc(sizeof(char **) * (NUM_REQUIRED_VGE_EXTENSIONS + numExtra));

	for (int i = 0; i < NUM_REQUIRED_VGE_EXTENSIONS; ++i) {
		extensions[i] = malloc(sizeof(char) * strlen(extensions[i]));
		strcpy(extensions[i], extensions[i]);
	}
	for (int i = 0; i < numExtra; ++i) {
		extensions[NUM_REQUIRED_VGE_EXTENSIONS + i] = malloc(sizeof(char) * strlen(extra[i]));
		strcpy(extensions[NUM_REQUIRED_VGE_EXTENSIONS + i], extra[i]);
	}
	*numExtensions = NUM_REQUIRED_VGE_EXTENSIONS + numExtra;
	return extensions;
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

	}
}

#endif

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

// Callbacks are inherently platform-agnostic so no ifdef's are needed
void vgeSetWindowSizeCallback(pVgeWindow window, void *(*func)(pVgeWindow window, uint32_t width, uint32_t height)) {
window->resizeCallback = (void(*)(struct VgeWindow_t*, uint32_t, uint32_t))func;
}
