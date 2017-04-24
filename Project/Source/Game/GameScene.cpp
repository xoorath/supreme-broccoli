#include <Include/Game/GameScene.h>

#include <Include/Component_Model.h>
#include <Include/Entity.h>
#include <ThirdParty/xo-math/xo-math.h>

namespace XO {
void GameScene::Init(class Renderer* renderer) {
    Super::Init(renderer);

    Entity* cubeEntity = new Entity();
    cubeEntity->AddComponent(new Component_Model("Models/Cube/cube.ini"));

    AddEntity(cubeEntity);
    cubeEntity->SetScale(Vector3(0.5f, 0.5f, 0.5f));
}

void GameScene::Tick() {
    Super::Tick();
}
}