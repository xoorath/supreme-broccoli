#pragma once

#include <Include/Types.h>

namespace XO {

class AssetManager {
public:
    static const char* AssetsRoot();
    static const char* RelativetoAssetsRoot(const char* childPath);
    static const char* RelativetoAssetsRoot(const String& childPath);
    static const char* EngineConfig(const char* group, const char* name, const char* defaultValue = nullptr);

    //////////////////////////////////////////////////////////////////////////
    // Only create an asset manager once alongside the entry point.
    AssetManager();
    ~AssetManager();
private:
    // pimpl pattern + singleton pattern.
    static class AssetManagerImpl* ImplInstance;
};

}