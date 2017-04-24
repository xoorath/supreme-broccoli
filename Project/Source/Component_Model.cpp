#include <Include/Component_Model.h>

#include <Include/Assets/Model.h>
#include <Include/Assets/Texture2D.h>
#include <Include/Log.h>
#include <Include/Renderer.h>
#include <ThirdParty/xo-math/xo-math.h>

namespace XO {

class Component_ModelImpl {
public:
    String ModelPath;
    Texture2D texture;
    uint32 renderJob;

    Component_ModelImpl(const char* modelPath)
    : ModelPath(modelPath)
    , renderJob(-1) {
    }

    void InitRender(Renderer& renderer) {
        Model ModelAsset;
        ModelAsset.OnLoaded.Add([&](ModelData& data) {
            texture = data.Meshes[0].Texture;
            renderJob = renderer.PrepareJob(data);
        });
        ModelAsset.Load(ModelPath);
    }

    void Update(float dt) {

    }

    void Render(Renderer& renderer, float dt, const Matrix4x4& transform) {
        renderer.SubmitJob(renderJob, transform);
    }

};

Component_Model::Component_Model(const char* modelPath) 
: Super() {
    Impl = new Component_ModelImpl(modelPath);
}

Component_Model::~Component_Model() {
    delete Impl;
}

void Component_Model::InitRender(Renderer& renderer) {
    Impl->InitRender(renderer);
}

void Component_Model::Update(float dt) {
    Impl->Update(dt);
}

void Component_Model::Render(Renderer& renderer, float dt) {
    Impl->Render(renderer, dt, GetTransform());
}

}