#include "image.hpp"

void Global::Engine::Create::CreateImage(
    VkImage*  image,
    VkResult& res,
    VkData*   vkData,
    ImageSize imgSize = {},
    ImageData imgData = {}
) noexcept {
    VkImageCreateInfo imgConfig = {
        .sType                 = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .pNext                 = nil,
        .flags                 = imgData.flags,
        .imageType             = imgSize.type,
        .format                = imgData.format,
        .extent                = { imgSize.width, imgSize.height, imgSize.depth },
        .mipLevels             = imgData.mipMaps,
        .arrayLayers           = imgSize.arrayLayers,
        .samples               = imgData.samples,
        .tiling                = imgData.tiling,
        .usage                 = imgData.usage,
        .sharingMode           = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices   = nil,
        .initialLayout         = imgData.initialLayout,
    }; 
    
    res = VK_ERROR_INITIALIZATION_FAILED;
    res = vkCreateImage(
        vkData->lDevice.self,
        &imgConfig,
        &vkData->allocator,
        image
    );
}

VkResult Global::Engine::Create::CreateImageAndGetResult(
    VkData*   vkData,
    VkImage*  image,
    ImageSize imgSize,
    ImageData imgData
) noexcept {
    VkResult result = VK_ERROR_INITIALIZATION_FAILED;
    CreateImage(image, result, vkData, imgSize, imgData);
    return result;
}

void Global::Engine::Create::CreateImage_CRITICAL(
    VkImage*  image,
    VkData*   vkData,
    ImageData imgData,
    ImageSize imgSize
) {
    VkResult result = VK_ERROR_INITIALIZATION_FAILED;
    CreateImage(image, result, vkData, imgSize, imgData);
    VK_PANIC_CTX("CreateImage_CRITICAL");
}