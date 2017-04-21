#include <Include/Assets/Config.h>

#include <ThirdParty/ini/ini.h>

namespace XO
{

class ConfigImpl {
public:
    ConfigImpl(const char* path) 
    : ini(ini_load(path)) {
    }

    ~ConfigImpl() {
        if (ini) {
            ini_free(ini);
        }
    }

    bool IsValid() const {
        return ini != nullptr;
    }

    const char* Get(const char* group, const char* name, const char* defaultValue) {
        if (ini)
        {
            auto val = ini_get(ini, group, name);
            return val ? val : defaultValue;
        }
        return defaultValue;
    }

    ini_t* ini;
};

Config::Config(const char* path) {
    Impl = new ConfigImpl(path);
}

Config::~Config() {
    delete Impl;
}

const char* Config::Get(const char* group, const char* name, const char* defaultValue/*=nullptr*/) const {
    return Impl->Get(group, name, defaultValue);
}

bool Config::IsValid() const {
    return Impl->IsValid();
}

}

