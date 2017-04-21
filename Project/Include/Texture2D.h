#pragma once

#include "Types.h"

namespace XO {

class Texture2D {
public:
    Texture2D();
    ~Texture2D();

    void Load(const char* path);
    void Unload();

    bool GetIsValid() const;
    uint32 GetTextureID() const;

private:
    uint32 TextureID;
};

class Texture2DManager {
public:
    Texture2DManager(class Config& engineConfig);
    ~Texture2DManager();
private:
    friend class Texture2D;

    void AddTexture(Texture2D* texture);
    void RemoveTexture(Texture2D* texture);

    static Texture2DManager* Instance;
    Array<Texture2D*> Textures;
};

}