#include "engine.hpp"

using namespace Global;

unique_ptr<Window>         window;
unique_ptr<VulkanRenderer> renderer;

int RenderData::MAX_FRAMES_IN_FLIGHT = 2;

RenderData rData;
VkData     vkData{};

void Global::Init() {
    LOG("Engine::Init()");
    
    LOG("Creating Window");
    window = make_unique<Window>();
    window->Create();
    
    LOG("Creating Vulkan Renderer");
    renderer = make_unique<VulkanRenderer>();
    renderer->Init(&vkData);
    
    LOG("Engine::Init() done");
}

void Global::NextFrame(RenderData* rData) {
    // ...
    
    rData->currentFrame++;
    rData-> currentFrame %= RenderData::MAX_FRAMES_IN_FLIGHT;
}

void Global::Clean() {
    LOG("Engine::Clean()");
    
    LOG("Destroying Window");
    window->Destroy();
    
    LOG("Destroying Vulkan Renderer");
    renderer->Clean(&vkData);
    
    LOG("Engine::Clean() done");
}


Window::Window() {
    LOG("Window::Window()");

    this->width  = 1280;
    this->height = 720;

    bool ok = SDL_Init(SDL_INIT_VIDEO);
    if(!ok) {
        LOGF("SDL_Init() failed: %s", SDL_GetError());
        RUNTIME_ERROR("SDL_Init() failed");
    }
    
    LOG("Window::Window() done");
}

void Window::Create() {
    LOG("Window::Create()");
    
    this->ptr = SDL_CreateWindow(
        this->title,
        static_cast<int>(this->width),
        static_cast<int>(this->height),
        SDL_WINDOW_VULKAN
    );
    if(this->ptr == nil) {
        LOGF("SDL_CreateWindow() failed: %s", SDL_GetError());
        RUNTIME_ERROR("SDL_CreateWindow() failed");
    }
    
    LOG("Window::Create() done");
}

VkResult Window::CreateSurface(VkData* vulkan_data) {
    LOG("Window::CreateSurface()");
    
    bool ok = SDL_Vulkan_CreateSurface(
        this->ptr,
        vulkan_data->instance.self,
        vulkan_data->allocator,
        &vulkan_data->surface
    );
    if(!ok) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    
    return VK_SUCCESS;
}

void Window::DestroySurface(VkData* vulkan_data) {
    LOG("Window::DestroySurface()");
    SDL_Vulkan_DestroySurface(
        vulkan_data->instance.self,
        vulkan_data->surface,
        vulkan_data->allocator
    );
    
    LOG("Window::DestroySurface() done");
}

void Window::Destroy() {
    LOG("Window::Destroy()");
    
    SDL_DestroyWindow(this->ptr);
    this->ptr = nil;
    
    LOG("Window::Destroy() done");
}

void Window::GetExtent(int* width, int* height) {
    SDL_GetWindowSizeInPixels(this->ptr, width, height);
}

void Window::GetSize(int* width, int* height) {
    SDL_GetWindowSize(this->ptr, width, height);
}

Window::~Window() {
    LOG("Window::~Window()");
    SDL_Quit();
    
    LOG("Window::~Window() done");
}

void VulkanRenderer::Init(VkData* vulkan_data) {
    LOG("VulkanRenderer::Init()");
    
    
    LOG("Application Info");
    vulkan_data->appInfo = {
        .sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName   = "Hello World",
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
        .pEngineName        = "No Engine",
        .engineVersion      = VK_MAKE_VERSION(1, 0, 0),
        .apiVersion         = VK_API_VERSION_1_3,
    };
    
    
    IFDEBUG {
        LOG("Debug Messenger Info");
        vulkan_data->debugMessenger.info = {
            .sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
            .pNext           = nil,
            .flags           = 0,
            .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
            .messageType     = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
            .pfnUserCallback = VulkanMessageCallback,
            .pUserData       = nil,
        };
    }
    
    
    LOG("Instance Extensions");
    u32 extensionCount   = 0;
    vector<cstr> extensions = {};
    
    SDL_Vulkan_GetInstanceExtensions(&extensionCount);
    extensions.reserve(extensionCount + 2);
    
    auto windowExtensions =  SDL_Vulkan_GetInstanceExtensions(&extensionCount);
    for(u32 i = 0; i < extensionCount; i++) {
        UniqueAppend(extensions, windowExtensions[i]);
    }
    UniqueAppend(extensions, VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    
    // Pretty Print
    LOGF("\tNumber of extensions: %d\n", u32(extensions.size()));
    LOG("|~-*-~-*-~-*-~-*-~-*-~-*-~-*-~-*-~|");
    LOG("|>     |Instance Extensions |    <|");
    LOG("|~-*-~-*-~-*-~-*-~-*-~-*-~-*-~-*-~|");
    for(auto& ext : extensions) {
        LOGF("%s\n", ext);
    }
    LOG("|~-*-~-*-~-*-~-*-~-*-~-*-~-*-~-*-~|");
    LOG("|>                               <|");
    LOG("|~-*-~-*-~-*-~-*-~-*-~-*-~-*-~-*-~|");
    
    // Moving extensions ownership to struct
    vulkan_data->instance.extensions = std::move(extensions);
    
    
    IFDEBUG {
        LOG("Validation Layers");
        u32 layerCount   = 0;
        vector<cstr> layers = {};
        
        layers = {
            "VK_LAYER_KHRONOS_validation",
        };
        layerCount = layers.size();
        
        // Pretty Print
        LOGF("\tNumber of layers: %d\n", u32(layers.size()));
        LOG("|~-*-~-*-~-*-~-*-~-*-~-*-~-*-~-*-~-*-~|");
        LOG("|>      | Validation Layers |        <|");
        LOG("|~-*-~-*-~-*-~-*-~-*-~-*-~-*-~-*-~-*-~|");
        for(auto& layer : layers) {
            LOGF("%s\n", layer);
        }
        LOG("|~-*-~-*-~-*-~-*-~-*-~-*-~-*-~-*-~-*-~|");
        LOG("|>                                   <|");
        LOG("|~-*-~-*-~-*-~-*-~-*-~-*-~-*-~-*-~-*-~|");
        
        vulkan_data->instance.layers = std::move(layers);
    }
    
    
    LOG("Instance Info");
    vulkan_data->instance.info = {
        .sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext                   = &vulkan_data->debugMessenger.info,
        .flags                   = 0,
        .pApplicationInfo        = &vulkan_data->appInfo,
        .enabledLayerCount       = 0,
        .ppEnabledLayerNames     = nil,
        .enabledExtensionCount   = VecGetSizeU32(vulkan_data->instance.extensions),
        .ppEnabledExtensionNames = VecGetData(vulkan_data->instance.extensions),
    };
    
    
    LOG("Instance Creation");
    VkResult result = vkCreateInstance(
        &vulkan_data->instance.info,
        vulkan_data->allocator,
        &vulkan_data->instance.self
    );
    VK_PANIC_CTX("vkCreateInstance");  
    
    LOG("Debug Messenger");
    IFDEBUG {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
            vulkan_data->instance.self,
            "vkCreateDebugUtilsMessengerEXT"
        );
        result = func(
            vulkan_data->instance.self,
            &vulkan_data->debugMessenger.info,
            vulkan_data->allocator,
            &vulkan_data->debugMessenger.self
        );
        VK_PANIC_CTX("vkCreateDebugUtilsMessengerEXT");
    }
    
    
    LOG("Creating Surface");
    window->CreateSurface(vulkan_data);
    
    LOG("Choosing Physical Device");
    ChoosePhysicalDevice(vulkan_data);
    
    LOG("Creating Logical Device");
    CreateLogicalDevice(vulkan_data);
    
    LOG("Creating Logical Device Queues");
    CreateQueues(vulkan_data);
    
    LOG("Choosing Present Mode");
    ChoosePresentMode(vulkan_data);
    
    LOG("Choosing Extent");
    ChooseExtent(vulkan_data);
    
    LOG("Choosing Surface Format");
    ChooseSurfaceFormat(vulkan_data);
    
    LOGF(
        "Present Mode:\t%s\n",
        string_VkPresentModeKHR(vulkan_data->swapchain.presentMode)
    );
    LOGF(
        "Extent:\t%dx%d\n",
        vulkan_data->swapchain.extent.width,
        vulkan_data->swapchain.extent.height
    );
    LOGF(
        "Chosen Format: %s @ %s\n",
         string_VkFormat(vulkan_data->swapchain.surfaceFormat.format),
         string_VkColorSpaceKHR(vulkan_data->swapchain.surfaceFormat.colorSpace)
    );
    
    LOG("Creating Swapchain");
    CreateSwapchain(vulkan_data);
    
    LOG("VulkanRenderer::Init() done");
}

void VulkanRenderer::ChoosePhysicalDevice(VkData* vulkan_data) {
    u32 pDevicesCount = 0;
    vector<VkPhysicalDevice> pDevices = {};
    
    vkEnumeratePhysicalDevices(
        vulkan_data->instance.self,
        &pDevicesCount,
        nil
    );
    if(pDevicesCount == 0) {
        LOG("VulkanRenderer::ChoosePhysicalDevice() no physical devices found");
        RUNTIME_ERROR("No physical devices found");
    }
    
    pDevices.resize(pDevicesCount);
    vkEnumeratePhysicalDevices(
        vulkan_data->instance.self,
        &pDevicesCount,
        VEC_DATA(pDevices)
    );
    
    map<VkPhysicalDevice, u32>                    scores  = {};
    map<VkPhysicalDevice, VkData::PhysicalDevice> dataMap = {};
    
    int i = 0;
    for(auto& pDevice : pDevices) {
        scores[pDevice]  = 0;
        dataMap[pDevice] = {};
        
        auto& score = scores[pDevice];
        auto& data  = dataMap[pDevice];
        
        VkPhysicalDeviceProperties pDeviceProperties = {};
        vkGetPhysicalDeviceProperties(pDevice, &pDeviceProperties);
        
        VkPhysicalDeviceMemoryProperties pDeviceMemoryProperties = {};
        vkGetPhysicalDeviceMemoryProperties(pDevice, &pDeviceMemoryProperties);
        
        VkPhysicalDeviceFeatures2 pDeviceFeatures = {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
            .pNext = (void*)&VkData::PhysicalDevice::requestedDynamicRenderingFeatures,
            .features = {}
        };

        vkGetPhysicalDeviceFeatures2(pDevice, &pDeviceFeatures);
        
        VkSurfaceCapabilities2KHR pSurfaceCapabilities = {
            .sType = VK_STRUCTURE_TYPE_SURFACE_CAPABILITIES_2_KHR,
            .pNext = nil,
            .surfaceCapabilities = {}
        };
    
        VkPhysicalDeviceSurfaceInfo2KHR pSurfaceInfo = {
            .sType   = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SURFACE_INFO_2_KHR,
            .pNext   = nil,
            .surface = vulkan_data->surface,
        };

        vkGetPhysicalDeviceSurfaceCapabilities2KHR(
            pDevice,
            &pSurfaceInfo,
            &pSurfaceCapabilities
        );

        auto& feature  = pDeviceFeatures.features;
        auto& props    = pDeviceProperties;
        auto& memProps = pDeviceMemoryProperties;
        auto& limits   = props.limits;
        
        auto maxImageDimension2D = limits.maxImageDimension2D;
        LOGF("maxImageDimension2D: %d\n", maxImageDimension2D);
        score += maxImageDimension2D;
        
        auto maxImageDimension3D = limits.maxImageDimension3D;
        LOGF("maxImageDimension3D: %d\n", maxImageDimension3D);
        score += maxImageDimension3D;
        
        auto maxPushConstantsSize = limits.maxPushConstantsSize;
        LOGF("maxPushConstantsSize: %d\n", maxPushConstantsSize);
        score += maxPushConstantsSize;
        
        if(feature.samplerAnisotropy) {
            LOG("Supports: samplerAnisotropy");
            score += 10;
        } else {
            LOG("Doesn't support: samplerAnisotropy");
            score -= 10;
        }
        
        if(feature.tessellationShader) {
            LOG("Supports: tessellationShader");
            score += 10;
        } else {
            LOG("Doesn't support: tessellationShader");
            score -= 10;
        }
        
        vector<VkQueueFamilyProperties> queueFamilyProperties = {};
        u32 queueFamilyCount = 0;
        
        vkGetPhysicalDeviceQueueFamilyProperties(
            pDevice,
            &queueFamilyCount,
            nil
        );
        queueFamilyProperties.resize(queueFamilyCount);
        
        vkGetPhysicalDeviceQueueFamilyProperties(
            pDevice,
            &queueFamilyCount,
            VEC_DATA(queueFamilyProperties)
        );
        
        VkData::PhysicalDevice::Queues& queue = dataMap[pDevice].queues;
        u32             qfi   = 0;
        for(VkQueueFamilyProperties& qF : queueFamilyProperties) {
            auto& flags   = qF.queueFlags;
            bool  flagged = false;
            
            if((flags & VK_QUEUE_GRAPHICS_BIT) == VK_QUEUE_GRAPHICS_BIT && !queue.hasGraphicsFamily) {
                LOGF("\t#%d - %s\n", qfi, "Graphics");
                queue.hasGraphicsFamily = true;
                queue.graphicsFamily = qfi;
                queue.indices.push_back(qfi);
                flagged = true;
            }
            
            if((flags & VK_QUEUE_COMPUTE_BIT) == VK_QUEUE_COMPUTE_BIT) {
                LOGF("\t#%d - %s\n", qfi, "Compute");
                queue.hasComputeFamily = true;
                queue.computeFamily = qfi;
                queue.indices.push_back(qfi);
                flagged = true;
            }
            
            if((flags & VK_QUEUE_TRANSFER_BIT) == VK_QUEUE_TRANSFER_BIT) {
                LOGF("\t#%d - %s\n", qfi, "Transfer");
                queue.hasTransferFamily = true;
                queue.transferFamily = qfi;
                queue.indices.push_back(qfi);
                flagged = true;
            }
            
            VkBool32 supportPresent = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(
                pDevice,
                qfi,
                vulkan_data->surface,
                &supportPresent
            );
            if(supportPresent) {
                LOGF("\t#%d - %s\n", qfi, "Present");
                queue.hasPresentFamily = true;
                queue.presentFamily = qfi;
                queue.indices.push_back(qfi);
                flagged = true;
            }
            
            if(flagged) queue.indices.push_back(qfi);
            
            qfi++;
        }
        LOG("Final Queues: ");
        LOGF("\t Graphics Family:\t%d\n", queue.graphicsFamily);
        LOGF( "\t Present Family:\t%d\n", queue.presentFamily);
        LOGF( "\t Compute Family:\t%d\n", queue.computeFamily);
        LOGF("\t Transfer Family:\t%d\n", queue.transferFamily);
        
        switch(props.deviceType) {
            case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
                LOG("Device Type: DiscreteGpu");
                score += 1000;
                break;
            case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
                LOG("Device Type: IntegratedGpu");
                score += 750;
                break;
            case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
                LOG("Device Type: VirtualGpu");
                score += 500;
                break;
            case VK_PHYSICAL_DEVICE_TYPE_CPU:
                LOG("Device Type: Cpu");
                score += 250;
                break;
            case VK_PHYSICAL_DEVICE_TYPE_OTHER:
                LOG("Device Type: Other");
                score += 100;
                break;
            default:
                LOG("Device Type: Unknown");
                score -= 10;
                break;
        }
        score += pDevicesCount - i;
        
        dataMap[pDevice].capabilities     = pSurfaceCapabilities.surfaceCapabilities;
        dataMap[pDevice].features         = pDeviceFeatures;
        dataMap[pDevice].properties       = pDeviceProperties;
        dataMap[pDevice].memoryProperties = pDeviceMemoryProperties;
        dataMap[pDevice].self             = pDevice;

        LOGF("Device: %s\n", pDeviceProperties.deviceName);
        LOGF("Score: %d\n\n", score);
        i++;
    }
    
    int bestScore                     = 0;
    VkData::PhysicalDevice bestDevice = {};
    for (auto& [device, score] : scores) {
        if (score > bestScore) {
            bestScore  = score;
            bestDevice = dataMap[device];
        }
    }
    if (bestDevice.self == nil) {
        LOG("VulkanRenderer::ChoosePhysicalDevice() no suitable physical device found");
        RUNTIME_ERROR("No suitable physical device found");
    }
    vulkan_data->pDevice = std::move(bestDevice);
    LOGF("Best Device: %s\t With Score: %d\n", vulkan_data->pDevice.properties.deviceName, bestScore);
    
    LOG("VulkanRenderer::ChoosePhysicalDevice() done");
}


void VulkanRenderer::CreateLogicalDevice(VkData* vulkan_data) {
    LOG("VulkanRenderer::CreateLogicalDevice()");
    
    LOG("Logical Device QueueInfos"); 
    auto& queueCreateInfos = vulkan_data->lDevice.queueCreateInfos;
    float queuePriority = 1.0f;
    
    vector<u32> queues = {
        vulkan_data->pDevice.queues.graphicsFamily,
        vulkan_data->pDevice.queues.presentFamily,
    };
    for(auto& queue : queues) {
        VkDeviceQueueCreateInfo queueCreateInfo = {
            .sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .pNext            = nil,
            .flags            = 0,
            .queueFamilyIndex = queue,
            .queueCount       = 1,
            .pQueuePriorities = &queuePriority
        };
        queueCreateInfos.push_back(queueCreateInfo);
    }
    
    LOG("Device Extensions");
    
    vector<VkExtensionProperties> availableExtensionProperties = {};
    u32 availableExtensionCount   = 0;
    
    vkEnumerateDeviceExtensionProperties(
        vulkan_data->pDevice.self,
        nil,
        &availableExtensionCount,
        nil
    );
    if(availableExtensionCount == 0) {
        LOG("VulkanRenderer::CreateLogicalDevice() no available extensions found");
        RUNTIME_ERROR("No available extensions found");
    }
    
    availableExtensionProperties.resize(availableExtensionCount);
    vkEnumerateDeviceExtensionProperties(
        vulkan_data->pDevice.self,
        nil,
        &availableExtensionCount,
        VEC_DATA(availableExtensionProperties)
    );
    
    
    // Requested extensions
    vector<cstr> requestedDeviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME 
    };
    
    
    // Pretty print
    LOGF("\tNumber of available extensions: %d\n", VEC_SIZE(availableExtensionProperties));
    LOG("|~-*-~-*-~-*-~-*-~-*-~-*-~-*-~-*-~|");
    LOG("|>| Available Device Extensions |<|");
    LOG("|~-*-~-*-~-*-~-*-~-*-~-*-~-*-~-*-~|");
    for(auto& ext : availableExtensionProperties) {
        LOGF("%s\n", ext.extensionName);
    }
    LOG("|~-*-~-*-~-*-~-*-~-*-~-*-~-*-~-*-~|");
    LOG("|>                               <|");
    LOG("|~-*-~-*-~-*-~-*-~-*-~-*-~-*-~-*-~|");
    
    
    LOGF("\n\tNumber of requested extensions: %d\n", VEC_SIZE(requestedDeviceExtensions));
    LOG("|~-*-~-*-~-*-~-*-~-*-~-*-~-*-~-*-~|");
    LOG("|>| Requested Device Extensions |<|");
    LOG("|~-*-~-*-~-*-~-*-~-*-~-*-~-*-~-*-~|");
    for(auto& ext : requestedDeviceExtensions) {
        LOGF("%s\n", ext);
    }
    LOG("|~-*-~-*-~-*-~-*-~-*-~-*-~-*-~-*-~|");
    LOG("|>                               <|");
    LOG("|~-*-~-*-~-*-~-*-~-*-~-*-~-*-~-*-~|");
    
    
    vector<cstr> failedExtensions = requestedDeviceExtensions;
    for(auto& ext : availableExtensionProperties) {
        for(auto it = failedExtensions.begin(); it != failedExtensions.end();) {
            if(strcmp(ext.extensionName, *it) == 0) {
                it = failedExtensions.erase(it);
            } else {
                ++it;
            }
        }
    } 
    LOGF("\n\tNumber of failed extensions: %d\n", VEC_SIZE(failedExtensions));
    LOG("|~-*-~-*-~-*-~-*-~-*-~-*-~-*-~-*-~|");
    LOG("|> | Failed Device Extensions |  <|");
    LOG("|~-*-~-*-~-*-~-*-~-*-~-*-~-*-~-*-~|");
    for(auto& ext : failedExtensions) {
        LOGF("%s\n", ext);
    }
    LOG("|~-*-~-*-~-*-~-*-~-*-~-*-~-*-~-*-~|");
    LOG("|>                               <|");
    LOG("|~-*-~-*-~-*-~-*-~-*-~-*-~-*-~-*-~|");
    if(VEC_SIZE(failedExtensions) > 0) {
        LOGF("Number of failed Device Extensions: %d\n", VEC_SIZE(failedExtensions));
        RUNTIME_ERROR("Failed to create logical device");
    }
    vulkan_data->lDevice.extensions = std::move(requestedDeviceExtensions);
    
    
    LOG("Logical Device Info");
    vulkan_data->lDevice.createInfo = {
        .sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext                   = nil,
        .flags                   = 0,
        .queueCreateInfoCount    = VEC_SIZE(vulkan_data->lDevice.queueCreateInfos),
        .pQueueCreateInfos       = VEC_DATA(vulkan_data->lDevice.queueCreateInfos),
        /* @deprecated */
        // .enabledLayerCount       = VEC_SIZE(vulkan_data->instance.layers),
        /* @deprecated */
        // .ppEnabledLayerNames     = VEC_DATA(vulkan_data->instance.layers),
        .enabledExtensionCount   = VEC_SIZE(vulkan_data->lDevice.extensions),
        .ppEnabledExtensionNames = VEC_DATA(vulkan_data->lDevice.extensions),
        .pEnabledFeatures        = &vulkan_data->pDevice.requestedFeatures,
    };
    
    LOG("Logical Device Creation");
    
    VkResult result = vkCreateDevice(
        vulkan_data->pDevice.self,
        &vulkan_data->lDevice.createInfo,
        vulkan_data->allocator,
        &vulkan_data->lDevice.self
    );
    VK_PANIC_CTX("vkCreateDevice");
    
    LOG("VulkanRenderer::CreateLogicalDevice() done");
}


void VulkanRenderer::CreateQueues(VkData* vulkan_data) {
    LOG("VulkanRenderer::CreateQueues()");
    
    vkGetDeviceQueue(
        vulkan_data->lDevice.self,
        vulkan_data->pDevice.queues.graphicsFamily,
        0,
        &vulkan_data->lDevice.queues.graphicsFamily
    );
    
    vkGetDeviceQueue(
        vulkan_data->lDevice.self,
        vulkan_data->pDevice.queues.presentFamily,
        0,
        &vulkan_data->lDevice.queues.presentFamily
    );
    
    vkGetDeviceQueue(
        vulkan_data->lDevice.self,
        vulkan_data->pDevice.queues.computeFamily,
        0,
        &vulkan_data->lDevice.queues.computeFamily
    );
    
    vkGetDeviceQueue(
        vulkan_data->lDevice.self,
        vulkan_data->pDevice.queues.transferFamily,
        0,
        &vulkan_data->lDevice.queues.transferFamily
    );
    
    LOG("VulkanRenderer::CreateQueues() done");
}


void VulkanRenderer::ChoosePresentMode(VkData* vulkan_data) {
    VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
    
    vector<VkPresentModeKHR> presentModes = {};
    u32 presentModeCount               = 0;
    
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        vulkan_data->pDevice.self,
        vulkan_data->surface,
        &presentModeCount,
        nil
    );
    if(presentModeCount == 0) {
        LOG("VulkanRenderer::ChoosePresentMode() no present modes found");
        RUNTIME_ERROR("No present modes found");
    }
    
    presentModes.resize(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        vulkan_data->pDevice.self,
        vulkan_data->surface,
        &presentModeCount,
        VEC_DATA(presentModes)
    );
    
    vector<VkPresentModeKHR> preferredPresentModes = {
        VK_PRESENT_MODE_FIFO_RELAXED_KHR, // Best
        VK_PRESENT_MODE_FIFO_KHR, 
        VK_PRESENT_MODE_IMMEDIATE_KHR,
        VK_PRESENT_MODE_MAILBOX_KHR,
        VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR, // Worst
    };
    
    // Pretty print
    LOGF("\tAvailable Present Modes: %d\n", u32(presentModes.size()));
    LOG("|~-*-~-*-~-*-~-*-~-*-~-*-~-*-~-*-~-*-~-*-~-*-~|");
    LOG("|>        | Available Present Modes |        <|");
    LOG("|~-*-~-*-~-*-~-*-~-*-~-*-~-*-~-*-~-*-~-*-~-*-~|");
    for(auto& mode : presentModes) {
        LOGF("%s\n", string_VkPresentModeKHR(mode));
    }
    LOG("|~-*-~-*-~-*-~-*-~-*-~-*-~-*-~-*-~-*-~-*-~-*-~|");
    LOG("|>                                           <|");
    LOG("|~-*-~-*-~-*-~-*-~-*-~-*-~-*-~-*-~-*-~-*-~-*-~|");
    
    LOGF("\n\tPreferred Present Modes: %d\n", u32(preferredPresentModes.size()));
    LOG("|~-*-~-*-~-*-~-*-~-*-~-*-~-*-~-*-~-*-~-*-~-*-~|");
    LOG("|>        | Preferred Present Modes |        <|");
    LOG("|~-*-~-*-~-*-~-*-~-*-~-*-~-*-~-*-~-*-~-*-~-*-~|");
    for(auto& mode : preferredPresentModes) {
        LOGF("%s\n", string_VkPresentModeKHR(mode));
    }
    LOG("|~-*-~-*-~-*-~-*-~-*-~-*-~-*-~-*-~-*-~-*-~-*-~|");
    LOG("|>                                           <|");
    LOG("|~-*-~-*-~-*-~-*-~-*-~-*-~-*-~-*-~-*-~-*-~-*-~|");
    
    map<VkPresentModeKHR, u32> presentModeScores = {};
    for(u32 i = 0; i < presentModeCount; i++) {
        for(u32 j = 0; j < preferredPresentModes.size(); j++) {
            if(presentModes[i] == preferredPresentModes[j]) {
                presentModeScores[presentModes[i]] = j;
                break;
            }
        }
    }
    
    u32 bestScore = l<u32>::max();
    for (auto& [mode, score] : presentModeScores) {
        if (score < bestScore) {
            bestScore = score;
            presentMode = mode;
        }
    }
    vulkan_data->swapchain.presentMode = presentMode;
}


void VulkanRenderer::ChooseExtent(VkData* vulkan_data) {
    if(vulkan_data->pDevice.capabilities.currentExtent.width != l<u32>::max()) {
        vulkan_data->swapchain.extent = vulkan_data->pDevice.capabilities.currentExtent;
    } else {
        VkExtent2D actualExtent = { 0, 0 };
        
        int width  = 0;
        int height = 0;
        window->GetExtent(&width, &height);
        
        if(width == 0 || height == 0) {
            LOG("Window size is 0, using fallback - window size");
            window->GetSize(&width, &height);
        }
        
        actualExtent.width  = static_cast<u32>(width);
        actualExtent.height = static_cast<u32>(height);
        
        vulkan_data->swapchain.extent = actualExtent;
    }
}

bool operator<(const VkSurfaceFormatKHR& a, const VkSurfaceFormatKHR& b) {
    if (a.format != b.format) {
        return a.format < b.format;
    }
    return a.colorSpace < b.colorSpace;
}

bool operator==(VkSurfaceFormatKHR& a, VkSurfaceFormatKHR& b) {
    return a.format == b.format && a.colorSpace == b.colorSpace;
}

void VulkanRenderer::ChooseSurfaceFormat(VkData* vulkan_data) {
    VkSurfaceFormatKHR surfaceFormat = {};

    vector<VkSurfaceFormatKHR> surfaceFormats = {};
    u32 surfaceFormatCount = 0;
    
    vkGetPhysicalDeviceSurfaceFormatsKHR(
        vulkan_data->pDevice.self,
        vulkan_data->surface,
        &surfaceFormatCount,
        nil
    );
    if(surfaceFormatCount == 0) {
        LOG("VulkanRenderer::ChooseSurfaceFormat() no surface formats found");
        RUNTIME_ERROR("No surface formats found");
    }
    
    surfaceFormats.resize(surfaceFormatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(
        vulkan_data->pDevice.self,
        vulkan_data->surface,
        &surfaceFormatCount,
        VEC_DATA(surfaceFormats)
    );

    vector<VkSurfaceFormatKHR> preferredSurfaceFormats = {
        {
            VK_FORMAT_B8G8R8A8_UNORM,
            VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
        },
        {
            VK_FORMAT_B8G8R8A8_SRGB,
            VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
        },
        {
            VK_FORMAT_R8G8B8A8_UNORM,
            VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
        },
        {
            VK_FORMAT_R8G8B8A8_SRGB,
            VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
        },
    };

    // Pretty print
    LOGF("\tAvailable Surface Formats: %d\n", VEC_SIZE(surfaceFormats));
    LOG("|~-*-~-*-~-*-~-*-~-*-~-*-~-*-~-*-~-*-~|");
    LOG("|>   | Available Surface Formats |   <|");
    LOG("|~-*-~-*-~-*-~-*-~-*-~-*-~-*-~-*-~-*-~|");
    for(auto& format : surfaceFormats) {
        LOGF(
            "%s @ %s\n",
            string_VkFormat(format.format),
            string_VkColorSpaceKHR(format.colorSpace)
        );
    }
    LOG("|~-*-~-*-~-*-~-*-~-*-~-*-~-*-~-*-~-*-~|");
    LOG("|>                                   <|");
    LOG("|~-*-~-*-~-*-~-*-~-*-~-*-~-*-~-*-~-*-~|");

    LOGF("\n\tPreferred Surface Formats: %d\n", VEC_SIZE(preferredSurfaceFormats));
    LOG("|~-*-~-*-~-*-~-*-~-*-~-*-~-*-~-*-~-*-~|");
    LOG("|>   | Preferred Surface Formats |   <|");
    LOG("|~-*-~-*-~-*-~-*-~-*-~-*-~-*-~-*-~-*-~|");
    for(auto& format : preferredSurfaceFormats) {
        LOGF(
            "%s @ %s\n",
            string_VkFormat(format.format),
            string_VkColorSpaceKHR(format.colorSpace)
        );
    }
    LOG("|~-*-~-*-~-*-~-*-~-*-~-*-~-*-~-*-~-*-~|");
    LOG("|>                                   <|");
    LOG("|~-*-~-*-~-*-~-*-~-*-~-*-~-*-~-*-~-*-~|");

    auto preferredSurfaceFormatsLength = VEC_SIZE(preferredSurfaceFormats);
    map<VkSurfaceFormatKHR, u32> surfaceFormatScores = {};

    for(u32 i = 0; i < surfaceFormatCount; i++) {
        for(u32 j = 0; j < preferredSurfaceFormatsLength; j++) {
            if(surfaceFormats[i] == preferredSurfaceFormats[j]) {
                surfaceFormatScores[surfaceFormats[i]] = preferredSurfaceFormatsLength - 1 - j; 
                break;
            }
        }
    }

    u32 bestScore = 0;
    for (auto& [format, score] : surfaceFormatScores) {
        if (score > bestScore) {
            bestScore     = score;
            surfaceFormat = format;
        }
    }

    vulkan_data->swapchain.surfaceFormat = surfaceFormat;
}


void VulkanRenderer::CreateSwapchain(VkData* vulkan_data) {
    LOG("VulkanRenderer::CreateSwapchain()");

    auto& pDevice = vulkan_data->pDevice;
    auto& caps    = pDevice.capabilities;
    auto& swap    = vulkan_data->swapchain;
    auto& info    = swap.info;

    swap.imageCount = caps.minImageCount + 1;
    if(swap.imageCount > caps.maxImageCount) {
        swap.imageCount = caps.maxImageCount;
    } 
    if(swap.imageCount == 0) {
        LOG("VulkanRenderer::CreateSwapchain() no image count found");
        RUNTIME_ERROR("No image count found");
    }

    vulkan_data->swapchain.info = {
        .sType                 = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext                 = nil,
        .flags                 = 0,
        .surface               = vulkan_data->surface,
        .minImageCount         = vulkan_data->swapchain.imageCount,
        .imageFormat           = vulkan_data->swapchain.surfaceFormat.format,
        .imageColorSpace       = vulkan_data->swapchain.surfaceFormat.colorSpace,
        .imageExtent           = vulkan_data->swapchain.extent,
        .imageArrayLayers      = 1,
        .imageUsage            = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices   = nil,
        .preTransform          = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
        .compositeAlpha        = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode           = vulkan_data->swapchain.presentMode,
        .clipped               = true,
        .oldSwapchain          = nil,
    };
    
    vector<u32> indices = {
        vulkan_data->pDevice.queues.graphicsFamily,
        vulkan_data->pDevice.queues.presentFamily,
    };

    if(indices[0] != indices[1]) {
        info.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
        info.queueFamilyIndexCount = VEC_SIZE(indices);
        info.pQueueFamilyIndices   = VEC_DATA(indices);
    } else {
        info.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
        info.queueFamilyIndexCount = 0;
        info.pQueueFamilyIndices   = nil;
    }

    VkResult result = vkCreateSwapchainKHR(
        vulkan_data->lDevice.self,
        &vulkan_data->swapchain.info,
        vulkan_data->allocator,
        &vulkan_data->swapchain.self
    );
    VK_PANIC_CTX("vkCreateSwapchainKHR");

    LOG("VulkanRenderer::CreateSwapchain() done");
}

void VulkanRenderer::CreateResources(VkData* vulkan_data) {
    LOG("VulkanRenderer::CreateResources()");



    LOG("VulkanRenderer::CreateResources() done");
}


void VulkanRenderer::Clean(VkData* vulkan_data) {
    LOG("VulkanRenderer::Clean()");
    
    LOG("VulkanRenderer::Clean() done");
}
