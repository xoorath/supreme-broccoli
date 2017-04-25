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
    GameScene();
    virtual ~GameScene();
    virtual void Init(class Renderer* renderer) override;
    virtual float Tick() override;

private:
    class GameSceneImpl* Impl;
    //class Entity* CubeEntity;
    //
    //class Entity* OrbitingCube;
};

}