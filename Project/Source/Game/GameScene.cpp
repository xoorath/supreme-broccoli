#include <Include/Game/GameScene.h>

#include <Include/Component_Model.h>
#include <Include/Entity.h>
#include <ThirdParty/xo-math/xo-math.h>

namespace XO {
GameScene::~GameScene() {
    if (CubeEntity) {
        delete CubeEntity;
    }
    if (OrbitingCube) {
        delete OrbitingCube;
    }
}

void GameScene::Init(class Renderer* renderer) {
    Super::Init(renderer);

    CubeEntity = new Entity();
    CubeEntity->AddComponent(new Component_Model("Models/Cube/cube.ini"));

    AddEntity(CubeEntity);
    CubeEntity->SetPosition(Vector3(0.0f, 0.0f, -3.0f));

    OrbitingCube = new Entity();
    OrbitingCube->AddComponent(new Component_Model("Models/Cube/cube.ini"));

    CubeEntity->AddChild(OrbitingCube);
    OrbitingCube->SetPosition(Vector3(0.0f, 0.0f, -2.0f));
}

float GameScene::Tick() {
    float dt = Super::Tick();

    auto rotation = CubeEntity->GetRotation();
    
    rotation += dt;

    CubeEntity->SetRotation(rotation);

    auto orbitingRotation = OrbitingCube->GetRotation();
    orbitingRotation.y += dt;
    OrbitingCube->SetRotation(orbitingRotation);
    

    return dt;
}
}