#pragma once

#include <Include/Scene.h>

namespace XO {

class GameScene : public Scene {
    typedef Scene Super;
public:
    virtual void Init(class Renderer* renderer) override;
    virtual float Tick() override;

private:
    class Entity* CubeEntity;
};

}