#include <Include/Game/GameScene.h>

//#include <Include/Component_Model.h>
#include <Include/Entity.h>
#include <ThirdParty/xo-math/xo-math.h>

#include <Include/Widgets/Component_Widget.h>
#include <Include/Widgets/DataProvider_Code.h>
#include <Include/Widgets/DataProvider_Config.h>

namespace XO {

class GameSceneImpl {
public:
    GameSceneImpl(GameScene* owner)
        : Owner(owner)
        , CarouselConfig("Widgets/Carousel/DemoCarousel.ini")
        , CarouselComponent(nullptr) {
    }

    ~GameSceneImpl() {
        if (CarouselComponent) {
            delete CarouselComponent;
        }
    }

    void Init(class Renderer* renderer) {
        CarouselData.LinkFallback(&CarouselConfig);

        // As part of the demo, I'm showing here that the code can provide data
        // to the widgets that overrides what the config has.
        CarouselData["CarouselNodesOnScreen"] = 7;
        CarouselComponent = UI::Component_Widget::CreateWidget_Carousel(&CarouselData);

        UIRoot.AddComponent(CarouselComponent);
        Owner->AddEntity(&UIRoot);
    }

    void Tick(float dt) {

    }

private:
    GameScene* Owner;

    Entity UIRoot;

    UI::Component_Widget* CarouselComponent;

    UI::DataProvider_Code CarouselData;
    UI::DataProvider_Config CarouselConfig;
};

GameScene::GameScene() {
    Impl = new GameSceneImpl(this);
}

GameScene::~GameScene() {
    delete Impl;

    //if (CubeEntity) {
    //    delete CubeEntity;
    //}
    //if (OrbitingCube) {
    //    delete OrbitingCube;
    //}
}

void GameScene::Init(class Renderer* renderer) {
    Super::Init(renderer);

    Impl->Init(renderer);

    //CubeEntity = new Entity();
    //CubeEntity->AddComponent(new Component_Model("Models/Cube/cube.ini"));
    //
    //AddEntity(CubeEntity);
    //CubeEntity->SetPosition(Vector3(0.0f, 0.0f, -3.0f));
    //
    //OrbitingCube = new Entity();
    //OrbitingCube->AddComponent(new Component_Model("Models/Cube/cube.ini"));
    //
    //CubeEntity->AddChild(OrbitingCube);
    //OrbitingCube->SetPosition(Vector3(0.0f, 0.0f, -2.0f));
}

float GameScene::Tick() {
    float dt = Super::Tick();
    Impl->Tick(dt);
    //auto rotation = CubeEntity->GetRotation();
    //
    //rotation += dt;
    //
    //CubeEntity->SetRotation(rotation);
    //
    //auto orbitingRotation = OrbitingCube->GetRotation();
    //orbitingRotation.y += dt;
    //OrbitingCube->SetRotation(orbitingRotation);
    

    return dt;
}
}