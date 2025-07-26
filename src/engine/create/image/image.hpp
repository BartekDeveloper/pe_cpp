#pragma once
#ifndef CREATE_IMAGE_HPP
#define CREATE_IMAGE_HPP

#include "../../engine.hpp"

namespace Global::Engine::Create {    
    struct ImageSize {
        u32         width       = 0;
        u32         height      = 0;
        u32         depth       = 0;
        u32         arrayLayers = 1; 
        VkImageType type        = VK_IMAGE_TYPE_2D;
    };
    
    struct ImageData {
        VkFormat              format        = VK_FORMAT_R8G8B8A8_UNORM;  
        VkImageTiling         tiling        = VK_IMAGE_TILING_OPTIMAL;
        VkImageUsageFlags     usage         =  VK_IMAGE_USAGE_SAMPLED_BIT;
        u32                   mipMaps       = 1;
        u32                   baseMipMap    = 0;
        VkSampleCountFlagBits samples       = VK_SAMPLE_COUNT_1_BIT;
        VkImageLayout         initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        VkImageCreateFlags    flags         = 0;
    };
    
    void CreateImage(
        VkImage*  image,
        VkResult& res,
        VkData*   vkData,
        ImageSize imgSize,
        ImageData imgData
    ) noexcept;
    
    VkResult CreateImageAndGetResult(
        VkData*   vkData,
        VkImage*  image,
        ImageSize imgSize,
        ImageData imgData
    ) noexcept;
    
    void CreateImage_CRITICAL(
        VkImage*  image,
        VkData*   vkData,
        ImageData imgData,
        ImageSize imgSize
    );
};

#endif // CREATE_IMAGE_HPP