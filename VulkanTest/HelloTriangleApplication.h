// Jesse Rankins 2021
#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>                     // Windowed application using Vulkan
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#define STDEXCEPT_INCLUDED_IN_APPLICATION   // Prevent main from re-including
#define IOSTREAM_INCLUDED_IN_APPLICATION    // Prevent main from re-including

#ifdef NDEBUG                               // Enable validation layers in debug
const bool validationLayersEnabled = false;
#else
const bool validationLayersEnabled = true;
#endif

#include <stdexcept>                        // Error reporting
#include <iostream>                         // nullptr, cout
#include <vector>                           // allAvailableExtensions
#include <map>                              // Rating GPU in scorePhysicalDevice
#include <set>                              // Queue families value set
#include "GlobalApplicationConstants.h"     // const uint32_t WINDOW_HEIGHT etc
#include "QueueFamilyIndices.h"             // Struct for vulkan detected qfams

class HelloTriangleApplication
{
public:
    // CONSTRUCTOR/DESTRUCTOR
    //------------------------------------------------------------------------//
    HelloTriangleApplication();

    ~HelloTriangleApplication();
    //------------------------------------------------------------------------//

    // PUBLIC FUNCTIONS
    //------------------------------------------------------------------------//
    void run();
    //------------------------------------------------------------------------//

private:
    // PRIVATE MEMBERS
    //------------------------------------------------------------------------//
    GLFWwindow*                             window;

    VkInstance                              instance;

    uint32_t                                glfwExtensionCount;

    bool                                    requiredGLFWExtensionsEstablished;

    const char**                            glfwExtensions;

    const std::vector<const char*>          validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };

    VkDebugUtilsMessengerEXT                debugMessenger;

    VkSurfaceKHR                            surface;

    VkPhysicalDevice                        physicalDevice;

    QueueFamilyIndices                      queueFamilyIndices;

    VkPhysicalDeviceProperties              physicalDeviceProperties;

    VkPhysicalDeviceFeatures                physicalDeviceFeatures;

    VkDevice                                logicalDevice;

    VkQueue                                 graphicsQueue;

    VkQueue                                 presentQueue;
    //------------------------------------------------------------------------//

    // PRIVATE FUNCTIONS
    //------------------------------------------------------------------------//
    void initWindow();

    void initVulkan();

    void createVkInstance();

    std::vector<const char*> getRequiredGLFWExtensions();

    void assertRequiredGLFWExtensionsAreAvailable();

    void assertRequiredValidationLayersAreAvailable();

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT, 
        VkDebugUtilsMessageTypeFlagsEXT, 
        const VkDebugUtilsMessengerCallbackDataEXT*, 
        void*
    );

    void setupDebugMessenger();

    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT&);

    // proxy create function
    static VkResult CreateDebugUtilsMessengerEXT(
        VkInstance, 
        const VkDebugUtilsMessengerCreateInfoEXT*, 
        const VkAllocationCallbacks*, 
        VkDebugUtilsMessengerEXT*
    );

    // proxy delete function
    static void DestroyDebugUtilsMessengerEXT(
        VkInstance, 
        VkDebugUtilsMessengerEXT, 
        const VkAllocationCallbacks*
    );

    void createSurface();

    void selectPhysicalDevice();

    void scorePhysicalDevice(VkPhysicalDevice&, std::multimap<int, VkPhysicalDevice>&);

    void findQueueFamilies(VkPhysicalDevice&);

    void createLogicalDevice();

    void getDeviceQueue();

    void mainLoop();
    //------------------------------------------------------------------------//
};