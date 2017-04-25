#pragma once

#include <Include/Widgets/DataProvider.h>

namespace XO {
namespace UI {

class DataProvider_Config : public IDataProvider {
public:
    DataProvider_Config(const char* configName, const char* group = "Widget");
    ~DataProvider_Config();

    virtual bool GetInt(const char* name, int32& outInt) const override;
    virtual bool GetFloat(const char* name, float32& outFloat) const override;
    virtual bool GetBool(const char* name, bool& outBool) const override;
    virtual bool GetString(const char* name, String& outString) const override;

private:
    class DataProvider_ConfigImpl* Impl;
};

}
}