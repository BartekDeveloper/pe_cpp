#pragma once
#ifndef MATHS_H
#define MATHS_H

#include "../shared.hpp"
#include "./const.h"
#include <cmath>

template<typename T, int N>
using Vec = T[N];

template<typename T>
using Quaternion = T[4];

template<typename T, int Rows, int Cols>
using Mat = T[Rows][Cols];

template<typename T> constexpr T Sin(T angle) {
    return static_cast<T>(std::sin(static_cast<double>(angle)));
}
template<typename T> constexpr T Cos(T angle) {
    return static_cast<T>(std::cos(static_cast<double>(angle)));
}
template<typename T> constexpr T Tan(T angle) {
    return static_cast<T>(std::tan(static_cast<double>(angle)));
}

// VECTORS
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

// Normal
typedef Mat<f32, 1, 1> Mat1x1;
typedef Mat<f32, 1, 2> Mat1x2;
typedef Mat<f32, 1, 3> Mat1x3;
typedef Mat<f32, 1, 4> Mat1x4;

typedef Mat<f32, 2, 1> Mat2x1;
typedef Mat<f32, 2, 2> Mat2x2;
typedef Mat<f32, 2, 3> Mat2x3;
typedef Mat<f32, 2, 4> Mat2x4;

typedef Mat<f32, 3, 1> Mat3x1;
typedef Mat<f32, 3, 2> Mat3x2;
typedef Mat<f32, 3, 3> Mat3x3;
typedef Mat<f32, 3, 4> Mat3x4;

typedef Mat<f32, 4, 1> Mat4x1;
typedef Mat<f32, 4, 2> Mat4x2;
typedef Mat<f32, 4, 3> Mat4x3;
typedef Mat<f32, 4, 4> Mat4x4;

typedef Mat4x4 Mat4;
typedef Mat3x3 Mat3;
typedef Mat2x2 Mat2;
typedef Mat1x1 Mat1;

// Double
typedef Mat<f64, 1, 1> Mat1x1d;
typedef Mat<f64, 1, 2> Mat1x2d;
typedef Mat<f64, 1, 3> Mat1x3d;
typedef Mat<f64, 1, 4> Mat1x4d;

typedef Mat<f64, 2, 1> Mat2x1d;
typedef Mat<f64, 2, 2> Mat2x2d;
typedef Mat<f64, 2, 3> Mat2x3d;
typedef Mat<f64, 2, 4> Mat2x4d;

typedef Mat<f64, 3, 1> Mat3x1d;
typedef Mat<f64, 3, 2> Mat3x2d;
typedef Mat<f64, 3, 3> Mat3x3d;
typedef Mat<f64, 3, 4> Mat3x4d;

typedef Mat<f64, 4, 1> Mat4x1d;
typedef Mat<f64, 4, 2> Mat4x2d;
typedef Mat<f64, 4, 3> Mat4x3d;
typedef Mat<f64, 4, 4> Mat4x4d;

typedef Mat4x4d Mat4d;
typedef Mat3x3d Mat3d;
typedef Mat2x2d Mat2d;
typedef Mat1x1d Mat1d;

// QUATERNIONS
// ...


#endif // MATHS_H