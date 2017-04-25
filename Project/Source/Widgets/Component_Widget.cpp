#include <Include/Widgets/Component_Widget.h>

#include <Include/Widgets/DataProvider.h>
#include <Include/Widgets/Widget.h>

// Specific widgets for the factory:
#include <Include/Widgets/Carousel/Widget_Carousel.h>

namespace XO {
namespace UI {

Component_Widget::~Component_Widget() {

}

////////////////////////////////////////////////////////////////////////// Widget factories.
/*static*/Component_Widget* Component_Widget::CreateWidget_Carousel(IDataProvider* rootDataProvider) {
    Widget_Carousel* widget = new Widget_Carousel(rootDataProvider);
    Component_Widget* component = new Component_Widget(widget);
    return component;
}
////////////////////////////////////////////////////////////////////////// End widget factories.

////////////////////////////////////////////////////////////////////////// Component Overrides
void Component_Widget::Init() {
    OwnedWidget->Init(Owner);
}
void Component_Widget::Update(float dt) {
    OwnedWidget->Update(dt);
}
////////////////////////////////////////////////////////////////////////// End Component Overrides

Component_Widget::Component_Widget(Widget* ownedWidget) 
    : Super()
    , OwnedWidget(ownedWidget) {
}

}
}