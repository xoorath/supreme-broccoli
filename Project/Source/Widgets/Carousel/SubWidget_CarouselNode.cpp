#include <Include/Widgets/Carousel/SubWidget_CarouselNode.h>

#include <Include/Entity.h>
#include <Include/Component_Model.h>
#include <Include/Log.h>
#include <Include/Widgets/Carousel/CarouselNodeProperties.h>
#include <ThirdParty/xo-math/xo-math.h>

namespace XO {
namespace UI {

class SubWidget_CarouselNodeImpl {
public:
    SubWidget_CarouselNodeImpl() 
        : OwningEntity(nullptr)
        , ModelComponent(nullptr)
        , time(0.0f) {
    }

    ~SubWidget_CarouselNodeImpl() {
        if (ModelComponent) {
            delete ModelComponent;
        }
    }

    void Setup(CarouselNodeProperties& properties) {
        xoFatalIf(OwningEntity == nullptr, "Carousel node subwidget needs to be initialized before setup is called.");
        xoFatalIf(ModelComponent != nullptr, "Carousel node is being setup twice. This isn't supported.");

        ModelComponent = new Component_Model(properties.ModelPath.c_str());
        NodeWidget.AddComponent(ModelComponent);

        WrapperWidget.AddChild(&NodeWidget);
        OwningEntity->AddChild(&WrapperWidget);
    }

    Entity& GetWrapperEntity() {
        return WrapperWidget;
    }

    void GetRGBA(float32 outRGBA[]) const {
        ModelComponent->GetRGBA(outRGBA);
    }

    void SetRGBA(float32 RGBA[]) {
        ModelComponent->SetRGBA(RGBA);
    }

    void Init(Entity* owner) {
        OwningEntity = owner;
    }

    void Update(float dt) {
        time += dt;
        auto p = WrapperWidget.GetPosition();
        NodeWidget.SetPosition(Vector3(0.0f, Sin(time + (p.x * 0.5f)) * 0.125f, 0.0f));
    }

private:
    Entity* OwningEntity;
    Component_Model* ModelComponent;
    Entity NodeWidget;
    Entity WrapperWidget;

    float time;
};



SubWidget_CarouselNode::SubWidget_CarouselNode() {
    Impl = new SubWidget_CarouselNodeImpl();
}

SubWidget_CarouselNode::~SubWidget_CarouselNode() {
    delete Impl;
}

// Widget_Carousel API
void SubWidget_CarouselNode::Setup(CarouselNodeProperties& properties) {
    Impl->Setup(properties);
}

Entity& SubWidget_CarouselNode::GetWrapperEntity() {
    return Impl->GetWrapperEntity();
}

void SubWidget_CarouselNode::GetRGBA(float32 outRGBA[]) const {
    Impl->GetRGBA(outRGBA);
}

void SubWidget_CarouselNode::SetRGBA(float32 RGBA[]) {
    Impl->SetRGBA(RGBA);
}
// End Widget_Carousel API

void SubWidget_CarouselNode::Init(Entity* owner) {
    Impl->Init(owner);
}

void SubWidget_CarouselNode::Update(float dt) {
    Impl->Update(dt);
}

}
}