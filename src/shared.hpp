#pragma once
#ifndef SHARED_HPP
#define SHARED_HPP

// std
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include <string>
#include <string_view>

#include <vector>
#include <array>

#include <queue>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>

#include <algorithm>
#include <functional>
#include <memory>

#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

#include <stdexcept>
#include <exception>
#include <cassert>
#include <optional>

#include <limits>

// VULKAN
#include "../include/vulkan/vulkan.hpp"
#include "../include/vulkan/vk_enum_string_helper.h"

// SDL
#include "../include/SDL3/SDL.h"
#include "../include/SDL3/SDL_vulkan.h"

#define LOG(msg) do { \
    puts(msg); \
} while(0)

#define LOGF(msg, ...) do { \
    printf(msg, __VA_ARGS__); \
} while(0)

#define LOGE(msg) do { \
    cerr << msg << endl; \
} while(0)

#define ASSERT(cond, msg) assert(#cond && #msg)

#define INPUT(x)   cin  >> x
#define PRINT(msg) cout << msg
#define ERROR(msg) cerr << msg

#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define LERP(x, y, a) ((x) * (1 - (a)) + (y) * (a))
#define CLAMP(x, min, max) MAX(MIN(x, max), min)
#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

#define RUNTIME_ERROR(msg) do { \
    throw std::runtime_error(msg); \
} while(0)

#define VK_PANIC_CTX(msg) \
    if(result != VK_SUCCESS) { \
        LOGF("%s failed: %s\n", #msg, string_VkResult(result)); \
        RUNTIME_ERROR("Failed"); \
    }

#define VK_CHECK_CTX(msg) \
    if(result != VK_SUCCESS) { \
        LOGF("%s result is: %s\n", #msg, string_VkResult(result)); \
    }

#define VK_PANIC(res, msg) \
    if(res != VK_SUCCESS) { \
        LOGF("%s failed: %s\n", #msg, string_VkResult(res)); \
        RUNTIME_ERROR("Failed"); \
    }

#define VK_CHECK(res, msg) \
    if(res != VK_SUCCESS) { \
        LOGF("%s result is: %s\n", #msg, string_VkResult(res)); \
    }

#define nil  nullptr
#define null NULL

using namespace std;

#ifndef NDEBUG
#define IFDEBUG if(true)
const bool DEBUG = true;
#else
#define IFDEBUG if(false)
const bool DEBUG = false;
#endif

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

// #ifdef __fp16
// typedef __fp16 f16;
// #else
// #warning "No __fp16 defined - defaulting to float"
// typedef float f16;
// #endif
typedef float  f16;
typedef float  f32;
typedef double f64;

typedef bool  b8;
typedef bool  b16;
typedef bool  b32;
typedef bool  b64;

typedef string      str;
typedef const char* cstr;

typedef u8 byte;
typedef u8 bytes[];

typedef void* ptr;

typedef vector<u8>  u8vec;
typedef vector<u16> u16vec;
typedef vector<u32> u32vec;
typedef vector<u64> u64vec;

// Byte-Sized Uints
typedef u8  $1;
typedef u16 $2;
typedef u32 $4;
typedef u64 $8;

template<typename T>
using l = std::numeric_limits<T>;

template<typename T>
static void UniqueAppend(vector<T>& v, T x) {
    if(std::find(v.begin(), v.end(), x) == v.end()) {
        v.push_back(x);
    }
}

template<typename T>
static const u64 VecGetSizeU64(vector<T>& v) {
    return static_cast<u64>(v.size());
}

template<typename T>
static const u32 VecGetSizeU32(vector<T>& v) {
    return static_cast<u32>(v.size());
}

template<typename T>
static const u16 VecGetSizeU16(vector<T>& v) {
    return static_cast<u16>(v.size());
}

template<typename T>
static const u8 VecGetSizeU8(vector<T>& v) {
    return static_cast<u8>(v.size());
}

// IMMUTABLE
template<typename T>
static const T* VecGetData(vector<T>& v) {
    return v.data();
}

// MUTABLE
#define VEC_DATA(v) v.data()
#define VEC_SIZE(v) u32(v.size())

#endif // SHARED_HPP

