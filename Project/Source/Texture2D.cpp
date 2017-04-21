#include "../Include/Texture2D.h"

#include "../Include/Config.h"
#include "../Include/Log.h"
#include "../ThirdParty/FreeImage/FreeImage.h"

#if defined(GAME_WINDOWS)
#include "../ThirdParty/GL/glew.h"
#include "../ThirdParty/GL/wglew.h"
#include <GL/GL.h>
#endif

namespace XO {

static String AssetsBasePath;
Texture2DManager* Texture2DManager::Instance = nullptr;

Texture2DManager::Texture2DManager(class Config& engineConfig) {
    xoFatalIf(Instance, "Texture2DManager already exists");
    AssetsBasePath = engineConfig.Get("Paths", "Assets", "../../Assets");
    FreeImage_Initialise();
}

Texture2DManager::~Texture2DManager() {
    FreeImage_DeInitialise();

    for (auto* tex : Textures) {
        tex->Unload();
    }
}

void Texture2DManager::AddTexture(Texture2D* texture) {
    Textures.push_back(texture);
}

void Texture2DManager::RemoveTexture(Texture2D* texture) {
    auto found = std::find(Textures.begin(), Textures.end(), texture);
    if (found != Textures.end()) {
        Textures.erase(found);
    }
}

Texture2D::Texture2D()
    : TextureID(-1) {

}

Texture2D::~Texture2D() {
    Unload();
}

void Texture2D::Load(const char* path) {
    String fullPath = AssetsBasePath + "/" + path;

    FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
    FIBITMAP *dib(0);
    BYTE* bits(0);
    uint32 
        width = 0, 
        height = 0;

    fif = FreeImage_GetFileType(fullPath.c_str(), 0);
    if (fif == FIF_UNKNOWN)
        fif = FreeImage_GetFIFFromFilename(fullPath.c_str());
    if (fif == FIF_UNKNOWN)
        return;

    if (FreeImage_FIFSupportsReading(fif))
        dib = FreeImage_Load(fif, fullPath.c_str());

    if (!dib)
        return;

    bits = FreeImage_GetBits(dib);
    width = FreeImage_GetWidth(dib);
    height = FreeImage_GetHeight(dib);
    if ((bits == 0) || (width == 0) || (height == 0))
        return;

    glGenTextures(1, &TextureID);
    glBindTexture(GL_TEXTURE_2D, TextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, bits);

    FreeImage_Unload(dib);
}

void Texture2D::Unload() {
    if (TextureID != -1) {
        glDeleteTextures(1, (GLuint*)&TextureID);
        TextureID = -1;
    }
}

bool Texture2D::GetIsValid() const {
    return TextureID != -1;
}

uint32 Texture2D::GetTextureID() const {
    return TextureID;
}

}