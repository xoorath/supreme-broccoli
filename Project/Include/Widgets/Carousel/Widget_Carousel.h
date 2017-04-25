#pragma once

#include <Include/Widgets/Widget.h>

namespace XO {
namespace UI {

class Widget_Carousel : public Widget {
    typedef Widget Super;
public:
    virtual void Init(class Entity* owner) override;
    virtual void Update(float dt) override;

protected:
    friend class Component_Widget;
    Widget_Carousel(class IDataProvider* rootDataProvider);
    ~Widget_Carousel();

private:
    friend class Widget_CarouselImpl;
    class Widget_CarouselImpl* Impl;
};

}
}