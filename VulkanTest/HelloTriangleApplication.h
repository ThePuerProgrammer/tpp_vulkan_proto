// Jesse Rankins 2021
#pragma once

#define VK_USE_PLATFORM_WIN32_KHR           // Windowed application using Vulkan
#define GLFW_INCLUDE_VULKAN                 //
#include <GLFW/glfw3.h>                     //
#define GLFW_EXPOSE_NATIVE_WIN32            //
#include <GLFW/glfw3native.h>               //

#define STDEXCEPT_INCLUDED_IN_APPLICATION   // Prevent main from re-including
#define IOSTREAM_INCLUDED_IN_APPLICATION    //

#ifdef NDEBUG                               // Enable validation layers in debug
const bool validationLayersEnabled = false; //
#else                                       //
const bool validationLayersEnabled = true;  //
#endif                                      //

#include <stdexcept>                        // Error reporting
#include <algorithm>                        // Necessary for std::clamp
#include <iostream>                         // nullptr, cout
#include <cstdint>                          // Necessary for UINT32_MAX
#include <fstream>                          // Reading/loading shader binaries
#include <vector>                           // allAvailableExtensions
#include <map>                              // Rating GPU in scorePhysicalDevice
#include <set>                              // Queue families value set

#include "GlobalApplicationConstants.h"     // const uint32_t WINDOW_HEIGHT etc
#include "QueueFamilyIndices.h"             // Struct for vulkan detected qfams

// Declared here in order to avoid reimporting Vulkan libraries
struct SwapChainSupportDetails
{
    // MEMBERS
    VkSurfaceCapabilitiesKHR            capabilities;
    std::vector<VkSurfaceFormatKHR>     formats;
    std::vector<VkPresentModeKHR>       presentModes;

    // FUNCTIONS
    bool swapChainIsAdequate()
    {
        return !formats.empty() && !presentModes.empty();
    }
};

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
    GLFWwindow*                             m_window;
    VkInstance                              m_instance;
    uint32_t                                m_glfwExtensionCount;
    bool                                    m_requiredGLFWExtensionsEstablished;
    const char**                            m_glfwExtensions;
    const std::vector<const char*>          m_validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };
    const std::vector<const char*>          m_deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
    VkDebugUtilsMessengerEXT                m_debugMessenger;
    VkSurfaceKHR                            m_surface;
    VkPhysicalDevice                        m_physicalDevice;
    VkPhysicalDeviceProperties              m_physicalDeviceProperties;
    VkPhysicalDeviceFeatures                m_physicalDeviceFeatures;
    VkDevice                                m_logicalDevice;
    VkQueue                                 m_graphicsQueue;
    VkQueue                                 m_presentQueue;
    QueueFamilyIndices                      m_queueFamilyIndices;
    VkSwapchainKHR                          m_swapChain;
    std::vector<VkImage>                    m_swapChainImages;
    VkFormat                                m_swapChainImageFormat;
    VkExtent2D                              m_swapChainExtent;
    std::vector<VkImageView>                m_swapChainImageViews;
    VkRenderPass                            m_renderPass;
    VkPipelineLayout                        m_pipelineLayout;
    VkPipeline                              m_graphicsPipeline;
    std::vector<VkFramebuffer>              m_swapChainFramebuffers;
    VkCommandPool                           m_commandPool;
    std::vector<VkCommandBuffer>            m_commandBuffers;
    VkSemaphore                             m_imageAvailableSemaphore;
    VkSemaphore                             m_renderFinishedSemaphore;
    std::vector<VkSemaphore>                m_imageAvailableSemaphores;
    std::vector<VkSemaphore>                m_renderFinishedSemaphores;
    size_t                                  m_currentFrame;
    std::vector<VkFence>                    m_inFlightFences;
    std::vector<VkFence>                    m_imagesInFlight;
    bool                                    m_framebufferResized;
    //------------------------------------------------------------------------//

    // PRIVATE FUNCTIONS
    //------------------------------------------------------------------------//
    void mainLoop();
    void initWindow();
    void initVulkan();
    void createVkInstance();
    std::vector<const char*> getRequiredGLFWExtensions();
    void assertRequiredGLFWExtensionsAreAvailable();
    void assertRequiredValidationLayersAreAvailable();
    void setupDebugMessenger();
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT&);
    void createSurface();
    void selectPhysicalDevice();
    void scorePhysicalDevice(
        VkPhysicalDevice&, 
        std::multimap<int, VkPhysicalDevice>&
    );
    bool checkDeviceExtensionSupport(VkPhysicalDevice);
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice&);
    void createLogicalDevice();
    void getDeviceQueue();
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice);
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(
        const std::vector<VkSurfaceFormatKHR>&
    );
    VkPresentModeKHR chooseSwapPresentMode(
        const std::vector<VkPresentModeKHR>&
    );
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR&);
    void createSwapChain();
    void createImageViews();
    void createRenderPass();
    void createGraphicsPipeline();
    void createFrameBuffers();
    VkShaderModule createShaderModule(const std::vector<char>&);
    void createCommandPool();
    void createCommandBuffers();
    void drawFrame();
    void createSynchronizationObjects();
    void recreateSwapChain();
    void destructSwapChain();
    //------------------------------------------------------------------------//

    // STATIC FUNCTIONS
    //------------------------------------------------------------------------//
    ////////////////////////////////////////////////////////////////////////////
    static VkResult CreateDebugUtilsMessengerEXT(           // Proxy functions
        VkInstance,                                         //
        const VkDebugUtilsMessengerCreateInfoEXT*,          //
        const VkAllocationCallbacks*,                       //
        VkDebugUtilsMessengerEXT*                           //
    );                                                      //
    static void DestroyDebugUtilsMessengerEXT(              //
        VkInstance,                                         //
        VkDebugUtilsMessengerEXT,                           //
        const VkAllocationCallbacks*                        //
    );                                                      //
    //////////////////////////////////////////////////////////
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT,
        VkDebugUtilsMessageTypeFlagsEXT,
        const VkDebugUtilsMessengerCallbackDataEXT*,
        void*
    );
    static std::vector<char> readFile(const std::string&);
    static void framebufferResizeCallback(GLFWwindow*, int, int);
    //------------------------------------------------------------------------//
};