#pragma once

#include <Include/Scene.h>

namespace XO {

namespace UI {
class Component_Widget;
class IDataProvider;
}

class GameScene : public Scene {
    typedef Scene Super;
public:
    virtual ~GameScene();
    virtual void Init(class Renderer* renderer) override;
    virtual float Tick() override;

private:
    class Entity* UIRoot;
    
    UI::Component_Widget* CarouselComponent;
    UI::IDataProvider* CarouselData;
    UI::IDataProvider* CarouselConfig;
    //class Entity* CubeEntity;
    //
    //class Entity* OrbitingCube;
};

}