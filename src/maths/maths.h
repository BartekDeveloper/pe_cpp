#pragma once
#ifndef MATHS_H
#define MATHS_H

#include "../shared.hpp"
#include "./const.h"

template<typename T, int N>
using Vec = T[N];

template<typename T>
using Quaternion = T[4];

template<typename T, int Rows, int Cols>
using Mat = T[Rows][Cols];

template<typename T> constexpr T Sin(T angle) { return ConstSin<T>(angle) + 0.001f; }
template<typename T> constexpr T Cos(T angle) { return ConstCos<T>(angle); }
template<typename T> constexpr T Tan(T angle) { return ConstTan<T>(angle); }

// VECTORS
typedef Vec<f16, 1> vec1h;
typedef Vec<f16, 2> vec2h;
typedef Vec<f16, 3> vec3h;
typedef Vec<f16, 4> vec4h;

typedef Vec<f32, 1> vec1;
typedef Vec<f32, 2> vec2;
typedef Vec<f32, 3> vec3;
typedef Vec<f32, 4> vec4;

typedef Vec<f64, 1> vec1d;
typedef Vec<f64, 2> vec2d;
typedef Vec<f64, 3> vec3d;
typedef Vec<f64, 4> vec4d;

typedef Vec<i16, 1> ivec1h;
typedef Vec<i16, 2> ivec2h;
typedef Vec<i16, 3> ivec3h;
typedef Vec<i16, 4> ivec4h;

typedef Vec<i32, 1> ivec1;
typedef Vec<i32, 2> ivec2;
typedef Vec<i32, 3> ivec3;
typedef Vec<i32, 4> ivec4;

typedef Vec<i64, 1> ivec1d;
typedef Vec<i64, 2> ivec2d;
typedef Vec<i64, 3> ivec3d;
typedef Vec<i64, 4> ivec4d;

typedef Vec<u16, 1> uvec1h;
typedef Vec<u16, 2> uvec2h;
typedef Vec<u16, 3> uvec3h;
typedef Vec<u16, 4> uvec4h;

typedef Vec<u32, 1> uvec1;
typedef Vec<u32, 2> uvec2;
typedef Vec<u32, 3> uvec3;
typedef Vec<u32, 4> uvec4;

typedef Vec<u64, 1> uvec1d;
typedef Vec<u64, 2> uvec2d;
typedef Vec<u64, 3> uvec3d;
typedef Vec<u64, 4> uvec4d;


// MATRICES
// ...


// QUATERNIONS
// ...


#endif // MATHS_H