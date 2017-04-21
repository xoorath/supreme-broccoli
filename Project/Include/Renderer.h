#pragma once

namespace XO {

class Renderer {
public:
    Renderer();
    ~Renderer();

    void Init();
    void Render();
    void DebugLog();

private:
    class RendererImpl* Impl;
};

}