#pragma once
#include <cstdint>
#include <optional>

struct QueueFamilyIndices
{
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool graphicsFamilyIsInitialized() 
    {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};