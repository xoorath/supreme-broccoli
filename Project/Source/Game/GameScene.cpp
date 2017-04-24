#include <Include/Game/GameScene.h>

#include <Include/Component_Model.h>
#include <Include/Entity.h>
#include <ThirdParty/xo-math/xo-math.h>

namespace XO {
void GameScene::Init(class Renderer* renderer) {
    Super::Init(renderer);

    CubeEntity = new Entity();
    CubeEntity->AddComponent(new Component_Model("Models/Cube/cube.ini"));

    AddEntity(CubeEntity);
    CubeEntity->SetScale(Vector3(0.5f, 0.5f, 0.5f));
    CubeEntity->SetPosition(Vector3(0.0f, 0.0f, -3.0f));
}

float GameScene::Tick() {
    float dt = Super::Tick();

    auto rotation = CubeEntity->GetRotation();
    
    rotation += dt;

    CubeEntity->SetRotation(rotation);

    return dt;
}
}