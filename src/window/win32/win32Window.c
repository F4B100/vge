//
// Created by fabio on 9/2/2025.
//

#ifdef VGE_PLATFORM_WIN32

#include "../../dataStructs/list.h"
#include "win32Defs.h"
#include <stdio.h>

uint32_t vgeIsWindowClosed(pVgeWindow window) {
	return window->state == WINDOW_CLOSED;
}

pVgeWindow vgeWindowInit(int32_t width, int32_t height, char *title) {
	pVgeWindow window = malloc(sizeof(vgeWindow));

	vgeMutexLock(&windowGlobalContext->windowsToCreate.mutex);
	listResize(windowGlobalContext->windowsToCreate.infoWindows, 1);
	pVgeWindowCreateInfo info = &listGetLast(windowGlobalContext->windowsToCreate.infoWindows);

	info->window = window;
	info->width = width;
	info->height = height;
	info->title = title;

	vgeCondInit(&info->cond);
	vgeMutexInit(&info->mutex);
	vgeMutexLock(&info->mutex);
	vgeMutexUnlock(&windowGlobalContext->windowsToCreate.mutex);

	vgeCondWait(&info->cond, &info->mutex);
	ShowWindow(window->hWindow, 10);

	return window;
}

void vgeHandleEvents() {
	vgeMutexLock(&windowGlobalContext->windowEvents.mutex);
	foreach(vgeEventList, event,windowGlobalContext->windowEvents.events) {
		switch (event->eventId) {
			case WINDOW_KEYBOARD_KEY_DOWN:
				vgeMutexLock(&event->window->callbackMutex);
				if (event->window->keyDownCallback) {
					event->window->keyDownCallback(event->window, event->window->userData, event->event.keyDown.key);
				}
				vgeMutexUnlock(&event->window->callbackMutex);
				break;
			case WINDOW_MOUSE_MOVE:
				vgeMutexLock(&event->window->callbackMutex);
				if (event->window->mouseMoveCallback) {
					event->window->mouseMoveCallback(event->window, event->window->userData, event->event.mouseMove.x, event->event.mouseMove.y);
				}
				vgeMutexUnlock(&event->window->callbackMutex);
				break;
			case WINDOW_MOUSE_CLICK_LEFT:
				vgeMutexLock(&event->window->callbackMutex);
				if (event->window->mouseLeftDownCallback) {
					event->window->mouseLeftDownCallback(event->window, event->window->userData, event->event.mouseClickLeft.x, event->event.mouseClickLeft.y);
				}
				vgeMutexUnlock(&event->window->callbackMutex);
				break;
			case WINDOW_RESIZE:
				vgeMutexLock(&event->window->callbackMutex);
				if (event->window->resizeCallback) {
					event->window->resizeCallback(event->window, event->window->userData, event->event.windowResize.x, event->event.windowResize.y);
				}
				vgeMutexUnlock(&event->window->callbackMutex);
				break;
			default:
				break;
		}
		listResize(windowGlobalContext->windowEvents.events, 0);
		vgeMutexUnlock(&windowGlobalContext->windowEvents.mutex);
	}
}

void vgeGetContentSize(pVgeWindow window, int32_t *width, int32_t *height) {
	RECT dim;
	if (GetClientRect(window->hWindow, &dim) == 0) {
		*width = -1;
		*height = -1;
		return;
	}
	*width = dim.right - dim.left;
	*height = dim.bottom - dim.top;
}

void vgeGetWindowSize(pVgeWindow window, int32_t *width, int32_t *height) {
	RECT dim;
	if (GetWindowRect(window->hWindow, &dim) == 0) {
		*width = -1;
		*height = -1;
		return;
	}
	*width = dim.right - dim.left;
	*height = dim.bottom - dim.top;
}

void vgeGetWindowPos(pVgeWindow window, int32_t *x, int32_t *y) {
	RECT dim;
	if (GetWindowRect(window->hWindow, &dim) == 0) {
		*x = -1;
		*y = -1;
		return;
	}
	*x = dim.left;
	*y = dim.top;
}

typedef struct VgeKeyboard {
	uint8_t keys[256];
} vgeKeyboard, *pVgeKeyboard;

void vgeKeyboardGetKeys(pVgeKeyboard kb) {
	GetKeyboardState(kb->keys);
}

void setMousePosWindow(pVgeWindow window, int32_t x, int32_t y) {
	struct tagPOINT point;
	point.x = x;
	point.y = y;
	ClientToScreen(window->hWindow, &point);
	point.y -= GetSystemMetrics(SM_CYCAPTION) / 2;
	SetCursorPos(point.x, point.y);
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

void setMousePos(int32_t x, int32_t y) {
	SetCursorPos(x, y);
}

void vgeSetWindowCaptureMouse(pVgeWindow window, uint32_t capture) {
	window->captureMouse = capture;
}

pVgeKeyboard vgeWindowGetKeyboard(pVgeWindow window) {
	return (pVgeKeyboard)&window->keyboard;
}

void vgeSetCallbackData(pVgeWindow window, void *data) {
	vgeMutexLock(&window->callbackMutex);
	window->userData = data;
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

void vgeSetMouseLeftDownCallback(pVgeWindow window, void (*func)(pVgeWindow window, void *data,int32_t width, int32_t height)) {
	vgeMutexLock(&window->callbackMutex);
	window->mouseLeftDownCallback = func;
	vgeMutexUnlock(&window->callbackMutex);
}

void vgeSetKeyDownCallback(pVgeWindow window, void (*func)(pVgeWindow window, void *data, int8_t key)) {
	vgeMutexLock(&window->callbackMutex);
	window->keyDownCallback = func;
	vgeMutexUnlock(&window->callbackMutex);
}
#endif