#include "main.hpp"

int main() {
    
    puts("Application Info");
    VkApplicationInfo appInfo = {
        .sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName   = "Hello World",
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
        .pEngineName        = "No Engine",
        .engineVersion      = VK_MAKE_VERSION(1, 0, 0),
        .apiVersion         = VK_API_VERSION_1_3,
    };

    puts("Instance Info");
    VkInstanceCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &appInfo,
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = NULL,
        .enabledExtensionCount = 0,
        .ppEnabledExtensionNames = NULL
    };

    puts("Instance Variables");
    VkInstance instance = NULL;
    VkResult   result   = VK_SUCCESS;
    
    puts("Instance Creation");
    result = vkCreateInstance(
        &createInfo,
        NULL,
        &instance
    );
    printf("Result %b\n", result);
    if(result != VK_SUCCESS) {
        printf("Error creating instance: %d\n", result);
        return 1;
    }

    puts("Instance Destroy");
    vkDestroyInstance(instance, NULL);
    
    return 0;
}

