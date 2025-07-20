#pragma once
#ifndef MATHS_CONST_H
#define MATHS_CONST_H

const int TABLE_SIZE   = 8192;
const int TAYLOR_TERMS = 21;

template<typename T> constexpr T PI          = static_cast<T>(3.14159265358979323846);
template<typename T> constexpr T _2PI        = static_cast<T>(T(2.0) * PI<T>); 
template<typename T> constexpr T TAN_EPSILON = static_cast<T>(1e-6);

#endif // MATHS_CONST_H