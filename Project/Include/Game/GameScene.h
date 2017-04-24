#pragma once

#include <Include/Scene.h>

namespace XO {

class GameScene : public Scene {
    typedef Scene Super;
public:
    virtual void Init(class Renderer* renderer) override;
    virtual void Tick() override;
};

}