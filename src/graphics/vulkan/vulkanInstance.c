//
// Created by fabio on 20-06-2025.
//

#include "vulkanInstance.h"

#ifdef NDEBUG
    #define VALIDATION_LAYERS_COUNT 0
    const char * validationLayers[] = {};
    #define VALIDATION_EXTENSIONS_COUNT 0
    const char * validationExtensions[] = {};
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
    #define VALIDATION_LAYERS_COUNT 1
    const char * validationLayers[] = {
        "VK_LAYER_KHRONOS_validation"
    };
    #define VALIDATION_EXTENSIONS_COUNT 1
    const char * validationExtensions[] = {
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME
    };
#endif

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {

    fprintf(stderr, "validation layer: %s\n", pCallbackData->pMessage);

    return VK_FALSE;
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT *createInfo) {
    createInfo->sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo->messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo->messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo->pfnUserCallback = debugCallback;
}

void createValidationMessenger(VkInstance instance, VkDebugUtilsMessengerEXT *toCreate) {
    if (!enableValidationLayers) return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    populateDebugMessengerCreateInfo(&createInfo);

    if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, toCreate) != VK_SUCCESS) {
        fprintf(stderr, "failed to set up debug messenger!");
    }
}

void createVulkanInstance(const char *windowName, VkInstance *toCreate) {

    VkApplicationInfo appInfo = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .apiVersion = VK_MAKE_VERSION(1, 4, 0),
        .engineVersion = VK_MAKE_VERSION(0, 0, 1),
        .applicationVersion = VK_MAKE_VERSION(0, 0, 1),
        .pApplicationName = windowName,
        .pEngineName = "vge"
    };

    VkInstanceCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &appInfo,
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = nullptr
    };

    uint32_t extensionCount;
    const char **extensionNames;

	if (enableValidationLayers) {
		extensionNames = vgeGetVulkanExtensions(&extensionCount, VALIDATION_EXTENSIONS_COUNT, validationExtensions);
		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
		populateDebugMessengerCreateInfo(&debugCreateInfo);
		createInfo.pNext = &debugCreateInfo;
	} else {
		extensionNames = vgeGetVulkanExtensions(&extensionCount, 0, nullptr);
		createInfo.pNext = nullptr;
	}

    createInfo.enabledExtensionCount = extensionCount;
    createInfo.ppEnabledExtensionNames = extensionNames;

    if (vkCreateInstance(&createInfo, nullptr, toCreate)) {
        fprintf(stderr, "Instance Creation Failed\n");
    }
    free(extensionNames);
}