#include <Include/Widgets/Carousel/Widget_Carousel.h>

namespace XO {
namespace UI {

class Widget_CarouselImpl {
public:

    Widget_CarouselImpl(Widget_Carousel* carousel)
        : Carousel(carousel) {
    }

    void Init(class Entity* owner) {
        int32 a, b, c, d;
        Carousel->GetInt("Alpha", a, Optional, 55);
        Carousel->GetInt("Beta", b, Optional, 66);
        Carousel->GetInt("Charlie", c, Optional, 77);
        Carousel->GetInt("Delta", d, Optional, 88);
    }

    void Update(float dt) {

    }

private:
    Widget_Carousel* Carousel;
};

void Widget_Carousel::Init(class Entity* owner) {
    Impl->Init(owner);
}

void Widget_Carousel::Update(float dt) {
    Impl->Update(dt);
}


Widget_Carousel::Widget_Carousel(class IDataProvider* rootDataProvider) 
: Super(rootDataProvider) {
    Impl = new Widget_CarouselImpl(this);
}

Widget_Carousel::~Widget_Carousel() {
    delete Impl;
}


}
}