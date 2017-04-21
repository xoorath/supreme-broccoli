#include <Include/Assets/AssetManager.h>

#include <Include/Assets/Config.h>
#include <Include/Types.h>

namespace XO {

class AssetManagerImpl {
public:
    Config EngineConfig;
    const char* AssetsRoot;

    AssetManagerImpl() 
    : EngineConfig("./Engine.ini") {
        AssetsRoot = EngineConfig.Get("Paths", "Assets", "../../Builds/Shared/easylog.config");
    }

    const char* GetAssetsRoot() const {
        return AssetsRoot;
    }

    const char* GetEngineConfiguration(const char* group, const char* name, const char* defaultValue) {
        return EngineConfig.Get(group, name, defaultValue);
    }
};

AssetManagerImpl* AssetManager::ImplInstance = nullptr;

AssetManager::AssetManager() {
    // If we messed up the asset manager creation, it's quite likely we also messed up the log initialization.
    // That's because the logs rely on the asset manager's correct loading of the engine config.
    // So let's break here and tell the user about it this way instead.
    if (ImplInstance) {

        // Hey, so. You broke here. That means you have more than one AssetManager.
        // That isn't supported.
        __debugbreak(); 
    }
    else {
        ImplInstance = new AssetManagerImpl();
    }
}

AssetManager::~AssetManager() {
    delete ImplInstance;
}

/*static*/ const char* AssetManager::AssetsRoot() {
    return ImplInstance->GetAssetsRoot();
}

/*static*/ const char* AssetManager::EngineConfig(const char* group, const char* name, const char* defaultValue/*=nullptr*/) {
    return ImplInstance->GetEngineConfiguration(group, name, defaultValue);
}

}