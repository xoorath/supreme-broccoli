#pragma once

#include "Types.h"

namespace XO {

WString StringToWString(String input) {
    WString output(input.length(), L' ');
    std::copy(input.begin(), input.end(), output.begin());
    return output;
}

}