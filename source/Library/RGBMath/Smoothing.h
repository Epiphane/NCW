// By Thomas Steinke

#pragma once

namespace CubeWorld
{

//
// Find the minimum between a and b, smoothing in order to get a continuous
// path with no sharp edges
//
float SmoothMin(float a, float b, float smoothness);

//
// Bias a number towards 0 or 1.
// 
// A bias of 0 means that you will get x back
// Bias > 0 biases x strongly towards 0, while bias < 0 biases towards 1.
//
float Bias01(float x, float bias);

}; // namespace CubeWorld