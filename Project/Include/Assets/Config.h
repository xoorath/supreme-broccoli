#pragma once

namespace XO {

class Config {
public:
    Config(const char* path);
    ~Config();

    const char* Get(const char* group, const char* name, const char* defaultValue = nullptr) const;
    bool IsValid() const;

private:
    class ConfigImpl* Impl;
};

}