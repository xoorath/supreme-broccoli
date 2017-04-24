#pragma once

#include <Include/Macros.h>
#include <Include/Types.h>

namespace XO {

class Entity {
public:

    Entity();
    ~Entity();

    const class Matrix4x4& GetTransform();

    void AddComponent(class Component* component);
    void RemoveComponent(class Component* component);

    void AddChild(Entity* child);

    const class Vector3& GetPosition() const;
    const class Vector3& GetScale() const;
    const class Vector3& GetRotation() const;

    void SetPosition(const class Vector3& position);
    void SetScale(const class Vector3& scale);
    void SetRotation(const class Vector3& rotation);

private:
    friend class SceneImpl;
    // SceneImpl API
    void Initialize();
    // End SceneImpl API

    xoPimpl(EntityImpl, Impl, 352);
};

}