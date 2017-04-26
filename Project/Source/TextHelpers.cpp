#include <Include/TextHelpers.h>

namespace XO {

namespace {
bool GetFunctionContents(const String& input, const String& functionName, String& outContents) {
    auto idx = input.find(functionName);
    if (idx != String::npos) {
        idx += functionName.length();
        auto parenLeft = input.find("(", idx)+1;
        if (parenLeft == String::npos) {
            return false;
        }
        auto parenRight = input.find(")", parenLeft);
        if (parenRight == String::npos) {
            return false;
        }
        outContents = input.substr(parenLeft, parenRight - parenLeft);
        return true;
    }
    return false;
}
}

bool TryConvertStringToRGBA(String input, float32 outColor[4]) {
    static String HexRGBFunc = "rgb_hex";
    static String HexRGBAFunc = "rgba_hex";

    String contents;

    if (GetFunctionContents(input, HexRGBAFunc, contents)) {
        int32 hex = strtol(contents.c_str(), nullptr, 16);
        outColor[0] = (float32)(hex & 0xff000000 >> 24) / 256.0f;
        outColor[1] = (float32)(hex & 0x00ff0000 >> 16) / 256.0f;
        outColor[2] = (float32)(hex & 0x0000ff00 >> 8) / 256.0f;
        outColor[3] = (float32)(hex & 0x000000ff >> 0) / 256.0f;
        return true;
    }
    else if (GetFunctionContents(input, HexRGBFunc, contents)) {
        int32 hex = strtol(contents.c_str(), nullptr, 16);
        outColor[0] = (float32)(hex & 0xff0000 >> 16) / 256.0f;
        outColor[1] = (float32)(hex & 0x00ff00 >> 8) / 256.0f;
        outColor[2] = (float32)(hex & 0x0000ff >> 0) / 256.0f;
        outColor[3] = 1.0f;
        return true;
    }

    return false;
}

}