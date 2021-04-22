// By Thomas Steinke

#include <algorithm>
#include <cmath>

#include "Smoothing.h"

namespace CubeWorld
{

float SmoothMin(float a, float b, float smoothness)
{
    // https://www.iquilezles.org/www/articles/smin/smin.htm
    float h = std::clamp((b - a + smoothness) / (2 * smoothness), 0.f, 1.f);
    return a * h + b * (1 - h) - smoothness * h * (1 - h);
}

float Bias01(float x, float bias)
{
    // https://shadertoy.com/view/Xd2yRd
    float k = std::powf(1 - bias, 3);
    return (x * k) / (x * k - x + 1);
}

}; // namespace CubeWorld