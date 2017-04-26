#include <Include/Component_Model.h>

#include <Include/Assets/Model.h>
#include <Include/Assets/Texture2D.h>
#include <Include/Log.h>
#include <Include/Renderer.h>
#include <ThirdParty/xo-math/xo-math.h>

namespace XO {

_XOSIMDALIGN class Component_ModelImpl {
public:
    _XO_OVERLOAD_NEW_DELETE()
    String ModelPath;
    Texture2D Texture;
    RenderJobParams RenderJob;

    Component_ModelImpl(const char* modelPath)
    : ModelPath(modelPath) {
    }

    ~Component_ModelImpl() {
        Texture.Unload();
    }

    void InitRender(Renderer& renderer) {
        Model ModelAsset;
        ModelAsset.OnLoaded.Add([&](ModelData& data) {
            Texture = data.Meshes[0].Texture;
            RenderJob.JobId = renderer.PrepareJob(data);
        });
        ModelAsset.Load(ModelPath);
    }

    void Render(Renderer& renderer, float dt, const Matrix4x4& transform) {
        RenderJob.Transform = transform;
        renderer.SubmitJob(RenderJob);
    }

    void GetRGBA(float32 outRGBA[]) const {
        memcpy(outRGBA, RenderJob.RGBA, sizeof(float32) * 4);
    }

    void SetRGBA(float32 RGBA[]) {
        memcpy(RenderJob.RGBA, RGBA, sizeof(float32) * 4);
    }

};

Component_Model::Component_Model(const char* modelPath) 
: Super() {
    Impl = new Component_ModelImpl(modelPath);
}

Component_Model::~Component_Model() {
    delete Impl;
}

void Component_Model::GetRGBA(float32 outRGBA[]) const {
    Impl->GetRGBA(outRGBA);
}

void Component_Model::SetRGBA(float32 RGBA[]) {
    Impl->SetRGBA(RGBA);
}

void Component_Model::InitRender(Renderer& renderer) {
    Impl->InitRender(renderer);
}

void Component_Model::Render(Renderer& renderer, float dt) {
    Matrix4x4 transform;
    GetWorldTransform(transform);
    Impl->Render(renderer, dt, transform);
}

}