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

void vgeGetContentSize(pVgeWindow window, uint32_t *width, uint32_t *height);

void vgeHandleEvents();

uint32_t vgeIsWindowClosed(pVgeWindow window);

// Callback Setting

void vgeSetCallbackData(pVgeWindow window, void *data);

void vgeSetWindowSizeCallback(pVgeWindow window, void (*func)(pVgeWindow window, void *data, int32_t width, int32_t height));

void vgeSetMouseMoveCallback(pVgeWindow window, void (*func)(pVgeWindow window, void *data,int32_t width, int32_t height));

// Get section
void vgeGetWindowName(pVgeWindow window, char **name);

#ifdef VGE_GRAPHICS_VULKAN
#include <vulkan/vulkan.h>

const char **vgeGetVulkanExtensions(uint32_t *numExtensions,uint32_t numExtra, const char** extra);

void vgeCreateVulkanWindowSurface(pVgeWindow window, VkInstance instance,VkSurfaceKHR *toCreate);

uint8_t physicalDeviceSupportsPresentation(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex);
#endif

#endif //VGEWINDOW_H
