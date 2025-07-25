/* COPYRIGHT CONTENT IN LICENSE.md FILE OR AT THE END OF THIS FILE */
#pragma once
#ifndef ENGINE_HPP
#define ENGINE_HPP

#include "../shared.hpp"
#include "../maths/maths.h"

namespace Global {
    
    struct RenderData {
        static int MAX_FRAMES_IN_FLIGHT;
        
        int currentFrame = 0;
    };
    
    struct VkData {
        VkAllocationCallbacks* allocator = null;
        VkApplicationInfo     appInfo    = {};
        
        struct DebugMessenger {
            VkDebugUtilsMessengerCreateInfoEXT info = {};
            VkDebugUtilsMessengerEXT           self = null;
        } debugMessenger;
        
        struct Instance {
            vector<cstr>            extensions = {};
            vector<cstr>            layers     = {};
            
            VkInstanceCreateInfo info       = {};
            VkInstance           self       = null;
        } instance;
        
        VkSurfaceKHR surface = null;
                
        struct PhysicalDevice {
            constexpr const static VkPhysicalDeviceFeatures requestedFeatures = {
                .samplerAnisotropy = true,
            };
            constexpr const static VkPhysicalDeviceDynamicRenderingFeatures requestedDynamicRenderingFeatures = {
                .dynamicRendering = true,
            };

            
            struct Queues {                
                bool hasGraphicsFamily = false;
                bool hasPresentFamily  = false;
                bool hasComputeFamily  = false;
                bool hasTransferFamily = false;

                u32  graphicsFamily    = 0;
                u32  presentFamily     = 0;
                u32  computeFamily     = 0;
                u32  transferFamily    = 0;

                vector<u32> indices       = {};
            } queues = {};

            VkPhysicalDeviceFeatures2        features         = {};
            VkPhysicalDeviceProperties       properties       = {};
            VkPhysicalDeviceMemoryProperties memoryProperties = {};
            VkSurfaceCapabilitiesKHR         capabilities     = {};
            
            VkPhysicalDevice                 self             = null;
        } pDevice;
        
        struct LogicalDevice {
            vector<VkDeviceQueueCreateInfo> queueCreateInfos = {};
            vector<cstr>                    extensions       = {};
            VkDeviceCreateInfo           createInfo       = {};
            
            struct Queues {
                VkQueue graphicsFamily = null;
                VkQueue presentFamily  = null;
                VkQueue computeFamily  = null;
                VkQueue transferFamily = null;
            } queues = {};
            
            VkDevice                     self             = null;
        } lDevice;

        struct SwapChain {
            VkPresentModeKHR   presentMode   = VK_PRESENT_MODE_FIFO_KHR;
            VkExtent2D         extent        = {};
            VkSurfaceFormatKHR surfaceFormat = {};
            
            u32 imageCount = 0;

            VkSwapchainCreateInfoKHR info    = {};
            VkSwapchainKHR self = null;
        } swapchain;
        
    };

    struct GBuffer {
        VkImage        image;
        VkImageView    view;
        VkDeviceMemory memory;
    };

    enum class MSAA {
        OFF,
        X2,
        X4,
        X8,
        X16,
        X32
    };

    struct GBufferProperties {
        VkImageUsageFlags usage         = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        VkFormat          format        = VK_FORMAT_R8G8B8A8_UNORM; 
        vec3              size          = {};
        u32               mipLevels     = 1;
        u32               baseLevel     = 1;
        u32               arrayLayers   = 1;

        // Flags
        MSAA              multisampling = MSAA::OFF;
    };
    
    class VulkanRenderer {
        public:
            VulkanRenderer() = default;
            ~VulkanRenderer() = default;
        
        public:
            void Init(VkData* vulkan_data);
            void Clean(VkData* vulkan_data);
        
        private:
            void ChoosePhysicalDevice(VkData* vulkan_data);
            void CreateLogicalDevice(VkData* vulkan_data);
            void CreateQueues(VkData* vulkan_data);
            void ChooseExtent(VkData* vulkan_data);
            void ChoosePresentMode(VkData* vulkan_data);
            void ChooseSurfaceFormat(VkData* vulkan_data);
            void CreateSwapchain(VkData* vulkan_data);
            void CreateResources(VkData* vulkan_data);
        
        private:
            void CreateGBuffer(
                const VkData*            vulkan_data,
                GBuffer*                 gBuffer,
                const GBufferProperties& gBufferProperties
            );

        private:
            void DestroySwapchain(VkData* vulkan_data);
            void DestroyQueues(VkData* vulkan_data);
            void DestroyLogicalDevice(VkData* vulkan_data);
            void DestroyInstance(VkData* vulkan_data);
        
        private:
            VkData data{};
    };
    
    class Window {
        public:
            Window();
            ~Window();
            
            enum class WindowType {
                Windowed   = 0,
                Fullscreen = 1,
                Borderless = 2,
            };
            
            enum class Resizable {
                Locked   = 0,
                Unlocked = 1,
            };
        
        public:
            void     Create();
            void     Destroy();
            void     DestroySurface(VkData* vulkan_data);
            void     GetExtent(int* width, int* height);
            void     GetSize(int* width, int* height);
            VkResult CreateSurface(VkData* vulkan_data);

        private:
            SDL_Window* ptr        = nil;
            u32         width      = 1280;
            u32         height     = 720;
            cstr        title      = "PE";
            WindowType  fullscreen = WindowType::Windowed;
            Resizable   resizable  = Resizable::Locked;
    };
    
    void Init();
    void NextFrame(RenderData* rData);
    void Clean();
    
    static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanMessageCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData
    ) {
        LOGF("Validation Layer: %s\n", pCallbackData->pMessage);
        return VK_FALSE;
    }
};

using namespace Global;

#endif // ENGINE_HPP

