#pragma once

#include <Include/Component.h>

namespace XO {
namespace UI {

class Component_Widget : public Component {
public:
    DECLARE_COMPONENT_TYPE(Component_Widget);

    ~Component_Widget();

    // Widget factories.
    static Component_Widget* CreateWidget_Carousel(class IDataProvider* rootDataProvider);
    // End widget factories.

    // Component Overrides
    virtual ComponentFlags GetComponentFlags() const override {
        return (ComponentFlags)(ComponentFlags::Update | ComponentFlags::Init);
    }
    virtual void Init() override;
    virtual void Update(float dt) override;
    // End Component Overrides

private:
    Component_Widget(class Widget* ownedWidget);
    class Widget* OwnedWidget;
};

}
}