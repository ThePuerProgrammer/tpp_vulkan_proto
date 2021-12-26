// Jesse Rankins 2021
#include "HelloTriangleApplication.h"

HelloTriangleApplication::HelloTriangleApplication()
{
    std::cout << "HelloTriangleApplication() called\n";

    glfwExtensionCount = 0;
    requiredGLFWExtensionsEstablished = false;

    physicalDevice = VK_NULL_HANDLE;

    initWindow();
    initVulkan();
}

HelloTriangleApplication::~HelloTriangleApplication()
{
    if (validationLayersEnabled) 
    {
        DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
    }

    vkDestroyDevice(logicalDevice, nullptr);
    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyInstance(instance, nullptr);
    glfwDestroyWindow(window);
    glfwTerminate();

    std::cout << "~HelloTriangleApplication() called\n";
}

void HelloTriangleApplication::run()
{
    mainLoop();
}

void HelloTriangleApplication::initWindow()
{
    glfwInit();

    // Stop glfw from initialing with opengl
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    // Window fixed dimensions
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Vulkan", nullptr, nullptr);
}

void HelloTriangleApplication::initVulkan()
{
    createVkInstance();
    setupDebugMessenger();
    createSurface();
    selectPhysicalDevice();
    createLogicalDevice();
    getDeviceQueue();
}

void HelloTriangleApplication::createVkInstance()
{
    // Initialize application information struct
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext = nullptr;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    // Initialize the instance creation struct, providing appInfo
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    // Get a vector of all the required GLFW extensions for this instance
    std::vector<const char*> requiredExtensions = getRequiredGLFWExtensions();

    // Assert that the required glfw extensions and validation layers are all
    // available for use in this VkInstance
    assertRequiredGLFWExtensionsAreAvailable();
    assertRequiredValidationLayersAreAvailable();

    // Finish initializing the instance creation struct
    createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
    createInfo.ppEnabledExtensionNames = requiredExtensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

    // If in debug mode, include the validation layers in our initialization 
    if (validationLayersEnabled)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

        populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
    }
    else
    {
        createInfo.enabledLayerCount = 0;

        createInfo.pNext = nullptr;
    }

    // Create and validate the instance
    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create VkInstance");
    }
}

std::vector<const char*> HelloTriangleApplication::getRequiredGLFWExtensions()
{
    // This ensures that the order for calling assertRequiredGLFWExtensionsAreAvailable() is preserved
    requiredGLFWExtensionsEstablished = true;

    // Get all required glfw extensions and their count
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (validationLayersEnabled)
    {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

void HelloTriangleApplication::assertRequiredGLFWExtensionsAreAvailable()
{
    // getRequiredGLFWExtensions() must be called before this function to prevent errors
    if (!requiredGLFWExtensionsEstablished)
    {
        throw std::runtime_error("assertRequiredGLFWExtensionsAreAvailable() called before getRequiredGLFWExtensions()");
    }

    // Get a count of all the available extensions
    uint32_t numberOfAvailableExtensions = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &numberOfAvailableExtensions, nullptr);

    // Init a vector of VkExtensionProperties to the size of the number of available extensions established above
    std::vector<VkExtensionProperties> allAvailableExtensions(numberOfAvailableExtensions);

    // Find all available extensions and place them in the vector
    vkEnumerateInstanceExtensionProperties(nullptr, &numberOfAvailableExtensions, allAvailableExtensions.data());

    // Assertion loop. The ith required tested against the jth available. If not found, exception thrown
    for (unsigned int i = 0; i < glfwExtensionCount; i++)
    {
        bool extensionNotFound = true;
        
        for (int j = 0; j < allAvailableExtensions.size(); j++)
        {
            if (strcmp(glfwExtensions[i], allAvailableExtensions[j].extensionName) == 0)
            {
                extensionNotFound = false;
                break;
            }
        }

        if (extensionNotFound)
        {
            throw std::runtime_error("Required GLFW extension missing");
        }
    }
    
    // Success
    std::cout << "ASSERTION: All required extensions are available\n";
}

void HelloTriangleApplication::assertRequiredValidationLayersAreAvailable()
{
    // Get a count of all of the available validation layers
    uint32_t validationLayerCount = 0;
    vkEnumerateInstanceLayerProperties(&validationLayerCount, nullptr);

    // Init a vector of VkLayerProperties to the size of the available validation layers found above
    std::vector<VkLayerProperties> availableLayers(validationLayerCount);

    // Fill the vector with all of the available validation layers
    vkEnumerateInstanceLayerProperties(&validationLayerCount, availableLayers.data());

    // Assertion loop. The ith required tested against the jth available. If not found, exception thrown
    for (const char* validationLayerName : validationLayers)    // ith
    {
        bool layerNotFound = true;

        for (const auto& availableLayer : availableLayers)      // jth
        {
            if (strcmp(validationLayerName, availableLayer.layerName) == 0)
            {
                layerNotFound = false;
                break;
            }
        }
        
        if (layerNotFound)
        {
            throw std::runtime_error("Required validation layer not found in available layers");
        }
    }

    // Sucess
    std::cout << "ASSERTION: All required validation layers are available\n";
}

void HelloTriangleApplication::setupDebugMessenger()
{
    if (!validationLayersEnabled) return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    populateDebugMessengerCreateInfo(createInfo);
    
    if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) 
    {
        throw std::runtime_error("failed to set up debug messenger!");
    }
}

VKAPI_ATTR VkBool32 VKAPI_CALL HelloTriangleApplication::debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT          messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT                 messageType,
    const VkDebugUtilsMessengerCallbackDataEXT*     pCallbackData,
    void*                                           pUserData
)
{
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}

void HelloTriangleApplication::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) 
{
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
}

VkResult HelloTriangleApplication::CreateDebugUtilsMessengerEXT(
    VkInstance                                  instance, 
    const VkDebugUtilsMessengerCreateInfoEXT*   pCreateInfo, 
    const VkAllocationCallbacks*                pAllocator, 
    VkDebugUtilsMessengerEXT*                   pDebugMessenger
) 
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

    if (func != nullptr) 
    {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else 
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void HelloTriangleApplication::DestroyDebugUtilsMessengerEXT(
    VkInstance                      instance, 
    VkDebugUtilsMessengerEXT        debugMessenger, 
    const VkAllocationCallbacks*    pAllocator
) 
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

    if (func != nullptr) 
    {
        func(instance, debugMessenger, pAllocator);
    }
}

void HelloTriangleApplication::createSurface()
{
    if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) 
    {
        throw std::runtime_error("failed to create window surface!");
    }
}

void HelloTriangleApplication::selectPhysicalDevice()
{
    // Get a count of the available Vulkan ready physical devices on this system
    uint32_t numberOfAvailablePhysicalDevices = 0;
    vkEnumeratePhysicalDevices(instance, &numberOfAvailablePhysicalDevices, nullptr);

    if (numberOfAvailablePhysicalDevices == 0)
    {
        // This system doesn't support Vulkan
        throw std::runtime_error("There are no physical devices available in this system");
    }

    // Allocate a vector of VkPhysicalDevice the size of the number available in the system
    std::vector<VkPhysicalDevice> physicalDevices(numberOfAvailablePhysicalDevices);

    // Populate the vector with the available physical devices
    vkEnumeratePhysicalDevices(instance, &numberOfAvailablePhysicalDevices, physicalDevices.data());

    // Multimap autosorts the best scoring physicalDevices
    std::multimap<int, VkPhysicalDevice> physicalDeviceCandidatesMap;

    // Score all of the physical devices
    for (auto& physicalDevice : physicalDevices)
    {
        scorePhysicalDevice(physicalDevice, physicalDeviceCandidatesMap);
    }

    // The highest scoring physicalDeviceCandidate must have a score > 0
    if (physicalDeviceCandidatesMap.rbegin()->first > 0) 
    {
        this->physicalDevice = physicalDeviceCandidatesMap.rbegin()->second;
    }
    else
    {
        throw std::runtime_error("System does not possess a suitable GPU");
    }

    // Something went wrong, device is still null. Cannot proceed
    if (this->physicalDevice == VK_NULL_HANDLE)
    {
        throw std::runtime_error("System does not possess a suitable GPU");
    }
}

void HelloTriangleApplication::scorePhysicalDevice(VkPhysicalDevice& physicalDevice, std::multimap<int, VkPhysicalDevice>& physicalDeviceCandidatesMap)
{
    // Query for properties of the provided physical device
    vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

    // Query for features of the provided physical device
    vkGetPhysicalDeviceFeatures(physicalDevice, &physicalDeviceFeatures);

    findQueueFamilies(physicalDevice);

    int physicalDeviceScore = 0;

    // Geometry shaders are required
    if (!physicalDeviceFeatures.geometryShader || !queueFamilyIndices.graphicsFamilyIsInitialized())
    {
        physicalDeviceCandidatesMap.insert(std::make_pair(physicalDeviceScore, physicalDevice));
        return;
    }

    // Discrete GPUs have a significant performance advantage
    if (physicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) 
    {
        physicalDeviceScore += 1000;
    }

    // Maximum possible size of textures affects graphics quality
    physicalDeviceScore += physicalDeviceProperties.limits.maxImageDimension2D;

    physicalDeviceCandidatesMap.insert(std::make_pair(physicalDeviceScore, physicalDevice));
}

void HelloTriangleApplication::findQueueFamilies(VkPhysicalDevice& device) 
{
    // Find the number of available queue familie properties based upon the provided VkPhysicalDevice
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    // Initialize a vector of VkQueueFamilyProperties to the size of the number of available queue families
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);

    // Populate the vector with all availble queue families
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto& queueFamily : queueFamilies) 
    {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) 
        {
            this->queueFamilyIndices.graphicsFamily = i;
        }

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

        if (presentSupport) 
        {
            queueFamilyIndices.presentFamily = i;
        }

        if (this->queueFamilyIndices.graphicsFamilyIsInitialized()) break;

        i++;
    }
}

void HelloTriangleApplication::createLogicalDevice()
{
    std::vector<VkDeviceQueueCreateInfo>    queueCreateInfos;
    std::set<uint32_t>                      uniqueQueueFamilies = { 
        queueFamilyIndices.graphicsFamily.value(), 
        queueFamilyIndices.presentFamily.value() 
    };

    float queuePriority = 1.0f;

    for (uint32_t queueFamily : uniqueQueueFamilies) 
    {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    // Logical device info struct assignment
    VkDeviceCreateInfo logicalDeviceCreateInfo{};
    logicalDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    logicalDeviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    logicalDeviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
    logicalDeviceCreateInfo.pEnabledFeatures = &physicalDeviceFeatures;

    // Not currrently using any extensions
    logicalDeviceCreateInfo.enabledExtensionCount = 0;

    if (validationLayersEnabled) 
    {
        // Add in our validation layers
        logicalDeviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        logicalDeviceCreateInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else 
    {
        logicalDeviceCreateInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(physicalDevice, &logicalDeviceCreateInfo, nullptr, &logicalDevice) != VK_SUCCESS) 
    {
        throw std::runtime_error("Failed to create logical device");
    }
}

void HelloTriangleApplication::getDeviceQueue()
{
    vkGetDeviceQueue(logicalDevice, queueFamilyIndices.graphicsFamily.value(), 0, &graphicsQueue);
    vkGetDeviceQueue(logicalDevice, queueFamilyIndices.presentFamily.value(), 0, &presentQueue);
}

void HelloTriangleApplication::mainLoop()
{
    while (!glfwWindowShouldClose(window)) 
    {
        glfwPollEvents();
    }
}
