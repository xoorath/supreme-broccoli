#pragma once

#include <Include/Macros.h>
#include <Include/Types.h>

namespace XO {

class Renderer {
public:
    Renderer();

    void Init();
    void Render();
    void DebugLog();

    uint32 PrepareJob(struct ModelData& modelData);
    void SubmitJob(uint32 job, const class Matrix4x4& transform);

private:
    xoPimpl(RendererImpl, Impl, 352);
};

}