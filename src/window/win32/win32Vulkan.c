//
// Created by Fabio on 18/10/2025.
//

#ifdef WIN32
#ifdef VGE_GRAPHICS_VULKAN

#include "win32Defs.h"

#define NUM_REQUIRED_VGE_EXTENSIONS 3
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_win32.h>
char *vgeExtensions[] = {
	VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
	VK_KHR_SURFACE_EXTENSION_NAME,
	VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME
};

char **vgeGetVulkanExtensions(uint32_t *numExtensions, uint32_t numExtra, const char** extra) {
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
	return extensions;
}

void vgeCreateVulkanWindowSurface(pVgeWindow window, VkInstance instance,VkSurfaceKHR *toCreate) {
	VkWin32SurfaceCreateInfoKHR info = {
		.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
		.pNext = nullptr,
		.flags = 0,
		.hinstance= vgeGetHInstance(),
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
