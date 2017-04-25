#include <Include/Game/GameScene.h>

//#include <Include/Component_Model.h>
#include <Include/Entity.h>
#include <ThirdParty/xo-math/xo-math.h>

#include <Include/Widgets/Component_Widget.h>
#include <Include/Widgets/DataProvider_Code.h>
#include <Include/Widgets/DataProvider_Config.h>

namespace XO {
GameScene::~GameScene() {
    if (UIRoot) {
        delete UIRoot;
    }
    if (CarouselData) {
        delete CarouselData;
    }
    if (CarouselConfig) {
        delete CarouselConfig;
    }
    if (CarouselComponent) {
        delete CarouselComponent;
    }
    //if (CubeEntity) {
    //    delete CubeEntity;
    //}
    //if (OrbitingCube) {
    //    delete OrbitingCube;
    //}
}

void GameScene::Init(class Renderer* renderer) {
    Super::Init(renderer);

    UIRoot = new Entity();
    
    auto data = new UI::DataProvider_Code();
    auto conf = new UI::DataProvider_Config("Widgets/Carousel/DemoCarousel.ini");

    CarouselData = data;
    CarouselConfig = conf;
    CarouselData->LinkFallback(CarouselConfig);

    (*data)["Beta"] = 6;
    CarouselComponent = UI::Component_Widget::CreateWidget_Carousel(CarouselData);

    UIRoot->AddComponent(CarouselComponent);
    AddEntity(UIRoot);

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