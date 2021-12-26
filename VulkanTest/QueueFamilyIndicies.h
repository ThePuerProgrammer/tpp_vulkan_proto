#pragma once
#include <cstdint>
#include <optional>

struct QueueFamilyIndices
{
    std::optional<uint32_t> graphicsFamily;

    bool graphicsFamilyIsInitialized() 
    {
        return graphicsFamily.has_value();
    }
};