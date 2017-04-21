#pragma once

#include <Include/Types.h>

namespace XO {

class Texture2D {
public:
    Texture2D();
    ~Texture2D();

    void Load(String path);
    void Unload();

    bool GetIsValid() const;
    uint32 GetTextureID() const;

private:
    uint32 TextureID;
};

}