#pragma once

#include <Include/Types.h>

namespace XO {
namespace UI {

class IDataProvider {
public:
    virtual bool GetInt(const char* name, int32& outInt) const = 0;
    virtual bool GetFloat(const char* name, float32& outFloat) const = 0;
    virtual bool GetBool(const char* name, bool& outBool) const = 0;
    virtual bool GetString(const char* name, String& outString) const = 0;

    void LinkFallback(IDataProvider* fallback) { Next = fallback; }

protected:
    IDataProvider* Next = nullptr;
};

}
}