//
// Created by fabio on 21-05-2025.
//

#ifndef VGEWINDOW_H
#define VGEWINDOW_H

#include "vgeWIndowDefs.h"

#define WINDOW_CLASS_NAME L"WindowClass"
#define WINDOW_MENU_NAME L"Menu_name"

#define WINDOW_CREATE_FAIL_MESSAGE L"Create Window failed!"

void vgeInit();

vgeWindow *vgeWindowInit(int32_t width, int32_t height, const char *title);

#ifdef VGE_PLATFORM_WIN32
LRESULT CALLBACK vgeWindowsWProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#define WINDOW_PROCEDURE vgeWindowsWProc

#ifdef VGE_GRAPHICS_VULKAN
#include <vulkan/vulkan.h>

char **vgeGetVulkanExtensions(uint32_t *numExtensions,uint32_t numExtra,char** extra);

void vgeCreateVulkanWindowSurface();
#endif

#endif

// Callback Setting
void vgeSetWindowSizeCallback(pVgeWindow window, void *(*func)(pVgeWindow window, uint32_t width, uint32_t height));

// Get section
void vgeGetWindowName(pVgeWindow window, char **name);

#endif //VGEWINDOW_H
