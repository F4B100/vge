//
// Created by fabio on 21-05-2025.
//

#ifndef VGEWINDOW_H
#define VGEWINDOW_H

#include <stdint.h>

typedef void* pVgeWindow;

void vgeInit();

pVgeWindow vgeWindowInit(int32_t width, int32_t height, char *title);
void *vgeWindowThreadFunc(void *arg);

void vgeSetStartTime();

double vgeGetTimeSinceStart();

void vgeHandleEvents();

uint32_t vgeIsWindowClosed(pVgeWindow window);

// Callback Setting

void vgeSetCallbackData(pVgeWindow window, void *data);

void vgeSetWindowSizeCallback(pVgeWindow window, void (*func)(pVgeWindow window, void *data, int32_t width, int32_t height));
void vgeSetMouseMoveCallback(pVgeWindow window, void (*func)(pVgeWindow window, void *data,int32_t width, int32_t height));
void vgeSetMouseLeftDownCallback(pVgeWindow window, void (*func)(pVgeWindow window, void *data,int32_t width, int32_t height));
void vgeSetKeyDownCallback(pVgeWindow window, void (*func)(pVgeWindow window, void *data, int8_t key));

// Get section
void vgeGetWindowName(pVgeWindow window, char **name);

void vgeGetContentSize(pVgeWindow window, int32_t *width, int32_t *height);
void vgeGetWindowSize(pVgeWindow window, int32_t *width, int32_t *height);

void vgeGetWindowPos(pVgeWindow window, int32_t *x, int32_t *y);

// Set section
void vgeSetWindowCaptureMouse(pVgeWindow window, uint32_t capture);


// input section
void setMousePos(int32_t x, int32_t y);
void setMousePosWindow(pVgeWindow window, int32_t x, int32_t y);

typedef struct VgeKeyboard {
	uint8_t keys[256];
} vgeKeyboard, *pVgeKeyboard;

pVgeKeyboard vgeWindowGetKeyboard(pVgeWindow window);

#ifdef VGE_GRAPHICS_VULKAN
#include <vulkan/vulkan.h>

const char **vgeGetVulkanExtensions(uint32_t *numExtensions,uint32_t numExtra, const char** extra);

void vgeCreateVulkanWindowSurface(pVgeWindow window, VkInstance instance,VkSurfaceKHR *toCreate);

uint8_t physicalDeviceSupportsPresentation(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex);
#endif

#endif //VGEWINDOW_H
