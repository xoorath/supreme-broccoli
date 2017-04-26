#define _CRT_SECURE_NO_WARNINGS
#include <Include/Assets/AssetManager.h>

#include <Include/Assets/Config.h>
#include <Include/Log.h>

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

    const char* RelativetoAssetsRoot(const char* childPath) const {
        constexpr uint32 BUFF_SIZE = 1024;
        xoErrIf(childPath == nullptr, "Can not resolve a null path.");
    #if !defined(GAME_FINAL)
        xoFatalIf(strlen(childPath) + strlen(AssetsRoot) + 1 > BUFF_SIZE, "Path excedes max size.");
    #endif
        static char buffer[BUFF_SIZE];


        sprintf(buffer, "%s/%s", AssetsRoot, childPath);
        return buffer;
    }

    const char* GetEngineConfiguration(const char* group, const char* name, const char* defaultValue) {
        return EngineConfig.Get(group, name, defaultValue);
    }
};

AssetManagerImpl* AssetManager::ImplInstance = nullptr;

AssetManager::AssetManager() {
    xoFatalIf(ImplInstance != nullptr, "More than one asset manager is not supported.");
    ImplInstance = new AssetManagerImpl();
}

AssetManager::~AssetManager() {
    delete ImplInstance;
}

/*static*/ const char* AssetManager::AssetsRoot() {
    return ImplInstance->GetAssetsRoot();
}

/*static*/ const char* AssetManager::RelativetoAssetsRoot(const char* childPath) {
    return ImplInstance->RelativetoAssetsRoot(childPath);
}

/*static*/ const char* AssetManager::RelativetoAssetsRoot(const String& childPath) {
    return RelativetoAssetsRoot(childPath.c_str());
}

/*static*/ const char* AssetManager::EngineConfig(const char* group, const char* name, const char* defaultValue/*=nullptr*/) {
    return ImplInstance->GetEngineConfiguration(group, name, defaultValue);
}

}