#include "../engine.hpp"

void CreateImage(
    VkImage*  image,
    VkResult& res,
    VkData*   vkData,
    u32       width      = vkData->swapchain.extent.width,
    u32       height     = vkData->swapchain.extent.height,
    u32       mipMaps    = 1,
    u32       baseMipMap = 0,
    u32       depth      = 0, 
) {       
    VkImageCreateInfo imgConfig = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = VK_FORMAT_R8G8B8A8_UNORM,
        .extent = {
            .width  = width,
            .height = height,  
            .depth  = depth,
        }, 
        .mipLevels = 1,
        .arrayLayers = 1,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = nullptr,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
    }; 
    
    res = VK_ERROR_INITIALIZATION_FAILED;
    res = vkCreateImage(
        vkData,
        &imgConfig,
        &vkData->allocator,
        image
    );
}
