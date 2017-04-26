#pragma once

#include <Include/Types.h>

namespace XO {

inline
WString StringToWString(String input) {
    WString output(input.length(), L' ');
    std::copy(input.begin(), input.end(), output.begin());
    return output;
}

// Only hex_rgb and hex_rgba functions are supported currently.
// Usage:
//      float outColor[4]
//      TryConvertStringToRGBA("hex_rgb(ff0000)", outColor);
//      // Now:
//      // outColor[0] == 1.0f;
//      // outColor[1] == 0.0f;
//      // outColor[2] == 0.0f;
//      // outColor[3] == 1.0f;
//      TryConvertStringToRGBA("hex_rgba(ff000080)", outColor);
//      // Now:
//      // outColor[0] == 1.0f;
//      // outColor[1] == 0.0f;
//      // outColor[2] == 0.0f;
//      // outColor[3] == 0.5f;
bool TryConvertStringToRGBA(String input, float32 outColor[4]);

}