// By Thomas Steinke
// Simple noise include that silences the warnings we get from the library

#pragma warning(push, 0)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma clang diagnostic ignored "-Wreorder"
#include <noise/noise.h>
#include <noiseutils/noiseutils.h>
#pragma clang diagnostic pop
#pragma warning(pop)