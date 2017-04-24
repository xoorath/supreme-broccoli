#pragma once

#include <Include/Macros.h>
#include <Include/Types.h>

namespace XO {

class Scene {
public:
    Scene();
    virtual ~Scene();
    static Scene* GetCurrentScene();

    void AddEntity(class Entity* entity);
    void RemoveEntity(class Entity* entity);

    virtual void Init(class Renderer* renderer);
    virtual float Tick();

private:
    friend class EntityImpl;
    // EntityImpl API
    void OnComponentAdded(class Entity* owner, class Component* component);
    void OnComponentRemoved(class Entity* owner, class Component* component);
    void OnChildAdded(class Entity* parent, class Entity* child);
    void OnChildRemoved(class Entity* parent, class Entity* child);
    // End EntityImpl API

    class SceneImpl* Impl;
    
    static Scene* CurrentScene;
};

}