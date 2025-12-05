//
// Created by Fabio on 28/11/2025.
//

#include "win32Defs.h"
#include "../../dataStructs/list.h"
#include <windowsx.h>
#include <stdio.h>

pVgeGlobalContext windowGlobalContext;

void freeVgeWindowCreateList(pVgeWindowCreateList list) {
	listFree(list->infoWindows);
}

LRESULT CALLBACK vgeWindowsWProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#define WINDOW_PROCEDURE vgeWindowsWProc

typedef struct VgeWindowThreadCreateInfo {
	vgeMutex mutex;
	vgeCond cond;
	uint32_t width;
	uint32_t height;
	char *windowName;
	pVgeWindow window;
} vgeWindowThreadCreateInfo, *pVgeWindowThreadCreateInfo;

void startWindow(pVgeWindowCreateInfo createInfo) {
	vgeMutexLock(&createInfo->mutex);

	pVgeWindow window = createInfo->window;

	window->mouseLeftDownCallback = nullptr;
	window->mouseMoveCallback = nullptr;
	window->keyDownCallback = nullptr;
	window->resizeCallback = nullptr;
	window->state = WINDOW_CLOSED;
	window->userData = nullptr;

	vgeMutexInit(&window->callbackMutex);
	vgeMutexInit(&window->inputMutex);
	vgeMutexInit(&window->boolMutex);

	window->captureMouse = false;

	memset(&window->keyboard, 0, sizeof(windowKeyboard));

	uint32_t nameLen = strlen(createInfo->title);
	wchar_t windowName[nameLen + 1];
	for (uint32_t i = 0; i < nameLen; ++i) {
		windowName[i] = (wchar_t)createInfo->title[i];
	}
	windowName[nameLen] = L'\0';

	window->hWindow = CreateWindowEx(
		0,
		windowGlobalContext->windowClassName,
		windowName,
		WS_OVERLAPPEDWINDOW,

		0, 0,
		(int) createInfo->width, (int) createInfo->height,

		nullptr,
		nullptr,
		windowGlobalContext->hInstance,
		window
	);

	if (window->hWindow == NULL) {
		MessageBoxW(nullptr, WINDOW_CREATE_FAIL_MESSAGE, windowName, 0);
		return ;
	}

	window->state = WINDOW_NORMAL;

	vgeCondSignal(&createInfo->cond);
	vgeMutexUnlock(&createInfo->mutex);
}

void *win32WindowHandlerThread(void *) {
	MSG message;
	while (true) {
		while (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&message);
			DispatchMessage(&message);
		}

		vgeMutexLock(&windowGlobalContext->windowsToCreate.mutex);
		foreach(vgeWindowCreateInfo, windowInfo, windowGlobalContext->windowsToCreate.infoWindows) {
			startWindow(windowInfo);
		}
		listResize(windowGlobalContext->windowsToCreate.infoWindows, 0);
		vgeMutexUnlock(&windowGlobalContext->windowsToCreate.mutex);

	}
}

double vgeTimeSinceOsStart() {
	LARGE_INTEGER frequency;
	LARGE_INTEGER counter;

	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&counter);

	return (double)counter.QuadPart / (double)frequency.QuadPart;
}


void vgeInit() {
	windowGlobalContext = malloc(sizeof(vgeGlobalContext));

	windowGlobalContext->hInstance = GetModuleHandle(nullptr);

	windowGlobalContext->windowClassName = malloc(sizeof(wchar_t) * WINDOW_CLASS_LENGTH);
	windowGlobalContext->menuName = malloc(sizeof(wchar_t) * WINDOW_MENU_LENGTH);

	for (int i = 0; i < WINDOW_CLASS_LENGTH; i++) {
		const wchar_t CLASS_NAME[] = WINDOW_CLASS_NAME;
		windowGlobalContext->windowClassName[i] = CLASS_NAME[i];
	}
	for (int i = 0; i < WINDOW_MENU_LENGTH; i++) {
		const wchar_t MENU_NAME[] = WINDOW_MENU_NAME;
		windowGlobalContext->menuName[i] = MENU_NAME[i];
	}

	WNDCLASSEX wc = {
		.cbSize = sizeof(wc),
		.hInstance = windowGlobalContext->hInstance,
		.lpfnWndProc = WINDOW_PROCEDURE,
		.lpszClassName = windowGlobalContext->windowClassName,
		.lpszMenuName = windowGlobalContext->menuName,
		.hCursor = LoadCursor(windowGlobalContext->hInstance, IDC_ARROW),
		.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1)
	};
	RegisterClassEx(&wc);

	SetProcessDPIAware();

	vgeMutexInit(&windowGlobalContext->windowEvents.mutex);
	windowGlobalContext->windowEvents.events = ListAlloc(sizeof(vgeEventList), 0);
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

	if (!window) {
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

	switch (uMsg) {
	case WM_KEYDOWN:
		vgeMutexLock(&windowGlobalContext->windowEvents.mutex);

		pVgeEventList eventKeyDown = ListExtend(windowGlobalContext->windowEvents.events, 1);
		eventKeyDown->window = window;
		eventKeyDown->eventId = WINDOW_KEYBOARD_KEY_DOWN;

		eventKeyDown->event.keyDown.key = (int8_t)wParam;

		vgeMutexUnlock(&windowGlobalContext->windowEvents.mutex);
		vgeMutexLock(&window->inputMutex);
		window->keyboard.keys[(int8_t)wParam] = true;
		vgeMutexUnlock(&window->inputMutex);
		break;
	case WM_KEYUP:
		vgeMutexLock(&window->inputMutex);
		window->keyboard.keys[(int8_t)wParam] = false;
		vgeMutexUnlock(&window->inputMutex);
		break;
	case WM_MOUSEMOVE:
		if (window->captureMouse) {
			vgeMutexLock(&windowGlobalContext->windowEvents.mutex);
			int32_t x, y, mouseX = GET_X_LPARAM(lParam), mouseY = GET_Y_LPARAM(lParam);

			vgeGetContentSize(window, &x, &y);

			x /= 2;
			y /= 2;

			if (x == mouseX && y == mouseY) {
				return 0;
			}

			listExtend(windowGlobalContext->windowEvents.events, 1);
			pVgeEventList eventMouseMove = &listGetLast(windowGlobalContext->windowEvents.events);
			eventMouseMove->window = window;
			eventMouseMove->eventId = WINDOW_MOUSE_MOVE;

			eventMouseMove->event.mouseMove.x = GET_X_LPARAM(lParam) - x;
			eventMouseMove->event.mouseMove.y = GET_Y_LPARAM(lParam) - y + GetSystemMetrics(SM_CYCAPTION) / 2;

			if (eventMouseMove->event.mouseMove.x != 0 || eventMouseMove->event.mouseMove.y != 0) {
				setMousePosWindow(window, x, y);
			}

			vgeMutexUnlock(&windowGlobalContext->windowEvents.mutex);
		}
		window->mouse.posX = (float)GET_X_LPARAM(lParam);
		window->mouse.posY = (float)GET_Y_LPARAM(lParam);
		break;
	case WM_LBUTTONDOWN:
		vgeMutexLock(&windowGlobalContext->windowEvents.mutex);

		listExtend(windowGlobalContext->windowEvents.events, 1);
		pVgeEventList eventMouseLeftDown = &listGetLast(windowGlobalContext->windowEvents.events);

		eventMouseLeftDown->window = window;
		eventMouseLeftDown->eventId = WINDOW_MOUSE_CLICK_LEFT;

		eventMouseLeftDown->event.mouseClickLeft.x = GET_X_LPARAM(lParam);
		eventMouseLeftDown->event.mouseClickLeft.y = GET_Y_LPARAM(lParam);

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

		listExtend(windowGlobalContext->windowEvents.events, 1);
		pVgeEventList eventWindowResize = &listGetLast(windowGlobalContext->windowEvents.events);

		RECT *sizingRect = (RECT *)lParam;

		eventWindowResize->window = window;
		eventWindowResize->eventId = WINDOW_RESIZE;

		eventWindowResize->event.windowResize.x = sizingRect->right - sizingRect->left;
		eventWindowResize->event.windowResize.y = sizingRect->bottom - sizingRect->top;

		vgeMutexUnlock(&windowGlobalContext->windowEvents.mutex);
		break;
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

HINSTANCE vgeGetHInstance() {
	return windowGlobalContext->hInstance;
}

