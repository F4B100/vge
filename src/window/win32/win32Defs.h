//
// Created by Fabio on 18/10/2025.
//

#ifndef VGE_WIN32DEFS_H
#define VGE_WIN32DEFS_H

#include <stdint.h>
#define UNICODE
#include <stdlib.h>
#include <windows.h>

#include "../../utils/vgeThread.h"

// window State
#define WINDOW_NORMAL 0x00
#define WINDOW_CLOSED 0x10

// event Defs
typedef struct VgeMouseMoveInfo {
	int32_t x;
	int32_t y;
} vgeEventMouseMove, *pVgeEventMouseMove;
#define WINDOW_MOUSE_MOVE			0x00000001
typedef struct VgeMouseClickLeftInfo {
	int32_t x;
	int32_t y;
} vgeEventMouseClickLeft, *pVgeEventMouseClickLeft;
#define WINDOW_MOUSE_CLICK_LEFT		0x00000002

typedef struct VgeWindowResizeInfo {
	int32_t x;
	int32_t y;
} vgeEventWindowResize, *pVgeEventWindowResize;
#define WINDOW_RESIZE				0x00000010

typedef struct VgeKeyDownInfo {
	int8_t key;
} vgeEventKeyDown, *pVgeEventKeyDown;
#define WINDOW_KEYBOARD_KEY_DOWN	0x00000020
typedef struct VgeKeyUpInfo {
	int8_t key;
} vgeEventKeyUp, *pVgeEventKeyUp;
#define WINDOW_KEYBOARD_KEY_UP		0x00000021

#define UNICODE
typedef uint32_t vgeEventID;

#define KEY_A 0x00
#define KEY_B 0x01
#define KEY_C 0x02
#define KEY_D 0x03
#define KEY_E 0x04
#define KEY_F 0x05
#define KEY_G 0x06
#define KEY_H 0x07
#define KEY_I 0x08
#define KEY_J 0x09
#define KEY_K 0x0A
#define KEY_L 0x0B
#define KEY_M 0x0C
#define KEY_N 0x0D
#define KEY_O 0x0E
#define KEY_P 0x0F
#define KEY_Q 0x10
#define KEY_R 0x11
#define KEY_S 0x12
#define KEY_T 0x13
#define KEY_U 0x14
#define KEY_V 0x15
#define KEY_W 0x16
#define KEY_X 0x17
#define KEY_Y 0x18
#define KEY_Z 0x18
#define KEY_SPACE 0x19
#define KEY_1 0x1A
#define KEY_2 0x1B
#define KEY_3 0x1C
#define KEY_4 0x1D
#define KEY_5 0x1E
#define KEY_6 0x1F
#define KEY_7 0x20
#define KEY_8 0x21
#define KEY_9 0x22
#define KEY_0 0x23
#define KEY_MINUS 0x24
#define KEY_EQUALS 0x25
#define KEY_LEFTBRACKET 0x26

typedef struct WindowKeyboard {
	uint8_t keys[256];
} windowKeyboard;

#define MOUSE_BUTTON_LEFT 0b1
#define MOUSE_BUTTON_MIDDLE 0b10
#define MOUSE_BUTTON_RIGHT 0b100
#define MOUSE_BUTTON_X1 0b1000
#define MOUSE_BUTTON_X2 0b10000

#define GET_MOUSE_BUTTON_LEFT(button) (button & MOUSE_BUTTON_LEFT)
#define GET_MOUSE_BUTTON_MIDDLE(button) (button & MOUSE_BUTTON_MIDDLE)
#define GET_MOUSE_BUTTON_RIGHT(button) (button & MOUSE_BUTTON_RIGHT)
#define GET_MOUSE_BUTTON_X1(button) (button & MOUSE_BUTTON_X1)
#define GET_MOUSE_BUTTON_X2(button) (button & MOUSE_BUTTON_X2)

typedef struct WindowMouse {
	float posX, posY;
	uint8_t buttons;
} windowMouse;

typedef union VgeWindowEvent {
	vgeEventMouseMove mouseMove;
	vgeEventMouseClickLeft mouseClickLeft;
	vgeEventKeyDown keyDown;
	vgeEventKeyUp keyUp;
	vgeEventWindowResize windowResize;
} vgeWindowEvent, pVgeWindowEvent;

typedef struct VgeWindow {
	HWND hWindow;
	uint32_t state;
	vgeMutex callbackMutex;
	void *userData;
	void (*mouseMoveCallback)(struct VgeWindow *window, void *data, int32_t x, int32_t y);
	void (*mouseLeftDownCallback)(struct VgeWindow *window, void *data, int32_t x, int32_t y);
	void (*resizeCallback)(struct VgeWindow *window, void *data, int32_t x, int32_t y);
	void (*keyDownCallback)(struct VgeWindow *window, void *data, int8_t key);

	vgeMutex boolMutex;
	uint8_t captureMouse;

	vgeMutex inputMutex;
	windowKeyboard keyboard;
	windowMouse mouse;
} vgeWindow, *pVgeWindow;

typedef struct VgeEventInfo {
	vgeEventID eventId;
	pVgeWindow window;
	vgeWindowEvent event;
} vgeEventList, *pVgeEventList;

typedef struct VgeSharedEventList {
	vgeMutex mutex;
	pVgeEventList events;
} vgeSharedEventList, *pVgeSharedEventList;

typedef struct VgeWindowCreateInfo {
	vgeMutex mutex;
	vgeCond cond;
	char *title;
	int32 x,y;
	int32 width, height;
	pVgeWindow window;
} vgeWindowCreateInfo, *pVgeWindowCreateInfo;

typedef struct VgeWindowCreateList {
	vgeMutex mutex;
	pVgeWindowCreateInfo infoWindows;
} vgeWindowCreateList, *pVgeWindowCreateList;
void freeVgeWindowCreateInfo(void *data);

typedef struct VgeGlobalContext_t {
	wchar_t *windowClassName;
	wchar_t *menuName;
	double StartTime;
	HINSTANCE hInstance;
	vgeWindowCreateList windowsToCreate;
	vgeSharedEventList windowEvents;
} vgeGlobalContext, *pVgeGlobalContext;

HINSTANCE vgeGetHInstance();
double vgeTimeSinceOsStart();

void vgeGetContentSize(pVgeWindow window, int32_t *width, int32_t *height);
void vgeGetWindowSize(pVgeWindow window, int32_t *width, int32_t *height);

void setMousePos(int32_t x, int32_t y);
void setMousePosWindow(pVgeWindow window, int32_t x, int32_t y);

extern pVgeGlobalContext windowGlobalContext;

#define WINDOW_CLASS_LENGTH 12
#define WINDOW_CLASS_NAME L"WindowClass"
#define WINDOW_MENU_LENGTH 10
#define WINDOW_MENU_NAME L"Menu_name"

#define WINDOW_CREATE_FAIL_MESSAGE L"Create Window failed!"

#endif // VGE_WIN32DEFS_H
