#include <Include/Widgets/DataProvider_Config.h>

#include <Include/Assets/AssetManager.h>
#include <Include/Assets/Config.h>
#include <sstream>

namespace XO {
namespace UI {

class DataProvider_ConfigImpl {
public:
    DataProvider_ConfigImpl(const char* configName, const char* group)
        : ConfigAsset(configName)
        , ConfigGroup(group) {
    }

    bool GetInt(const char* name, int32& outInt) const {
        if (!ConfigAsset.IsValid())
        {
            return false;
        }
        const char* found = ConfigAsset.Get(ConfigGroup.c_str(), name, nullptr);
        if (found) {
            std::stringstream convert;
            convert << found;
            convert >> outInt;
            return !convert.fail();
        }
        return false;
    }

    bool GetFloat(const char* name, float32& outFloat) const {
        if (!ConfigAsset.IsValid())
        {
            return false;
        }
        const char* found = ConfigAsset.Get(ConfigGroup.c_str(), name, nullptr);
        if (found) {
            std::stringstream convert;
            convert << found;
            convert >> outFloat;
            return !convert.fail();
        }
        return false;
    }

    bool GetBool(const char* name, bool& outBool) const {
        if (!ConfigAsset.IsValid())
        {
            return false;
        }
        const char* found = ConfigAsset.Get(ConfigGroup.c_str(), name, nullptr);
        if (found) {
            std::stringstream convert;
            convert << found;
            convert >> outBool;
            return !convert.fail();
        }
        return false;
    }

    bool GetString(const char* name, String& outString) const {
        if (!ConfigAsset.IsValid())
        {
            return false;
        }
        const char* found = ConfigAsset.Get(ConfigGroup.c_str(), name, nullptr);
        if (found) {
            outString = found;
            return true;
        }
        return false;
    }

private:
    Config ConfigAsset;
    String ConfigGroup;
};

DataProvider_Config::DataProvider_Config(const char* configName, const char* group/*="Widget"*/) {
    String configPath = AssetManager::AssetsRoot();
    configPath += String("/") + configName;
    Impl = new DataProvider_ConfigImpl(configPath.c_str(), group);
}

DataProvider_Config::~DataProvider_Config() {
    delete Impl;
}

bool DataProvider_Config::GetInt(const char* name, int32& outInt) const {
    return Impl->GetInt(name, outInt) || (Next != nullptr && Next->GetInt(name, outInt));
}

bool DataProvider_Config::GetFloat(const char* name, float32& outFloat) const {
    return Impl->GetFloat(name, outFloat) || (Next != nullptr && Next->GetFloat(name, outFloat));
}

bool DataProvider_Config::GetBool(const char* name, bool& outBool) const {
    return Impl->GetBool(name, outBool) || (Next != nullptr && Next->GetBool(name, outBool));
}

bool DataProvider_Config::GetString(const char* name, String& outString) const {
    return Impl->GetString(name, outString) || (Next != nullptr && Next->GetString(name, outString));
}

}
}