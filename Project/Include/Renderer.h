#pragma once

#include <Include/Types.h>

#include <ThirdParty/xo-math/xo-math.h>

namespace XO {

_XOSIMDALIGN struct RenderJobParams {
    _XO_OVERLOAD_NEW_DELETE();
    Matrix4x4 Transform = Matrix4x4::Identity;
    uint32 JobId = -1;
    float32 RGBA[4] = {1.0f, 1.0f, 1.0f, 1.0f};
};

class Renderer {
public:
    Renderer();
    ~Renderer();

    void Init();
    void Render();
    void DebugLog();

    uint32 PrepareJob(struct ModelData& modelData);
    void SubmitJob(const RenderJobParams& params);

private:
    class RendererImpl* Impl;
};

}