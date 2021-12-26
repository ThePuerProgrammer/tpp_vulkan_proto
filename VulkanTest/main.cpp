// Jesse Rankins 2021
#ifndef IOSTREAM_INCLUDED_IN_APPLICATION    // HelloTriangleApplication.h has this include already
#include <iostream>                         // error reporting and nullptr
#endif

#ifndef STDEXCEPT_INCLUDED_IN_APPLICATION   // HelloTriangleApplication.h has this include already
#include <stdexcept>                        // error reporting
#endif

#include <cstdlib>                          // EXIT_SUCCESS/FAILURE macros

#include "HelloTriangleApplication.h"       // The Vulkan Application
#include "GlobalApplicationConstants.h"     // const uint32_t WINDOW_HEIGHT etc

int main() 
{
    HelloTriangleApplication app;

    try 
    {
        app.run();
    }
    catch (const std::exception& e) 
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}