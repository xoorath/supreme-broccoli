#pragma once

#include <Include/Widgets/SubWidget.h>
#include <Include/Types.h>

namespace XO {
namespace UI {

struct CarouselNode {
    const char* ModelPath;
    uint32 Color;
};

class SubWidget_CarouselNode : public SubWidget {
public:
    SubWidget_CarouselNode();
    ~SubWidget_CarouselNode();

    // Widget_Carousel API
    void Setup(struct CarouselNodeProperties& properties);
    class Entity& GetWrapperEntity();
    void GetRGBA(float32 outRGBA[]) const;
    void SetRGBA(float32 RGBA[]);
    // End Widget_Carousel API

    // SubWidget overrides
    virtual void Init(Entity* owner) override;
    virtual void Update(float32 dt) override;
    // End SubWidget overrides

private:
    class SubWidget_CarouselNodeImpl* Impl;
};

}
}
