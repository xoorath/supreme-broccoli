#include <Include/Entity.h>

#include <Include/Component.h>
#include <Include/Scene.h>
#include <Include/Types.h>
#include <ThirdParty/xo-math/xo-math.h>

namespace XO {

_XOSIMDALIGN class EntityImpl {
public:
    _XO_OVERLOAD_NEW_DELETE();

    Matrix4x4 Transform;
    Matrix4x4 ScaleMatrix;
    Matrix4x4 PositionMatrix;
    Matrix4x4 RotationMatrix;
    Vector3 Scale;
    Vector3 Position;
    Vector3 Rotation;
    bool ScaleDirty : 1;
    bool PositionDirty : 1;
    bool RotationDirty : 1;
    bool IsInitialized : 1;

    Array<Component*> Components;

    Entity* Owner;
    Entity* Parent;
    Array<Entity*> Children;

    EntityImpl(Entity* owner)
        : Transform(Matrix4x4::Identity)
        , ScaleMatrix(Matrix4x4::Identity)
        , PositionMatrix(Matrix4x4::Identity)
        , RotationMatrix(Matrix4x4::Identity)
        , Scale(Vector3::One)
        , Position(Vector3::Zero)
        , Rotation(Vector3::Zero)
        , ScaleDirty(false)
        , PositionDirty(false)
        , RotationDirty(false)
        , IsInitialized(false)
        , Owner(owner)
        , Parent(nullptr) {

    }

    ~EntityImpl() {
        Scene* scene = Scene::GetCurrentScene();
        if (scene != nullptr && IsInitialized) {
            for (auto* comp : Components) {
                scene->OnComponentRemoved(Owner, comp);
            }
        }
    }


    void GetWorldTransform(Matrix4x4& outMatrix) {
        if (Parent) {
            Matrix4x4 parentTransform;
            Parent->GetWorldTransform(parentTransform);
            outMatrix = GetTransform() * parentTransform;
        }
        else {
            outMatrix = GetTransform();
        }
    }

    const Matrix4x4& GetTransform() {
        if (PositionDirty || RotationDirty || ScaleDirty) {
            if (PositionDirty) {
                Matrix4x4::Translation(Position, PositionMatrix);
                PositionDirty = false;
            }
            if (RotationDirty) {
                Matrix4x4::RotationRadians(Rotation, RotationMatrix);
                RotationDirty = false;
            }
            if (ScaleDirty) {
                Matrix4x4::Scale(Scale, ScaleMatrix);
                ScaleDirty = false;
            }
            Transform = ScaleMatrix * RotationMatrix * PositionMatrix;
        }
        return Transform;
    }

    void AddComponent(Component* component) {
        xoFatalIf(component->Owner != nullptr, "Reassigning a components owning entity. Remove this component from its prior owner first.");
        component->Owner = Owner;
        Components.push_back(component);
        if (IsInitialized) {
            Scene* scene = Scene::GetCurrentScene();
            xoFatalIf(scene == nullptr, "No current scene");
            scene->OnComponentAdded(Owner, component);
        }
    }
    void RemoveComponent(Component* component) {
        component->Owner = nullptr;
        xoErrIf(component == nullptr, "Trying to remove a null component.");
        auto found = std::find(Components.begin(), Components.end(), component);
        xoErrIf(found == Components.end(), "Component not found on entity in RemoveComponent.");
        Components.erase(found);
        if (IsInitialized) {
            Scene* scene = Scene::GetCurrentScene();
            xoFatalIf(scene == nullptr, "No current scene");
            scene->OnComponentRemoved(Owner, component);
        }
    }

    void AddChild(Entity* child) {
        child->Impl->Parent = Owner;
        child->Initialize();
    }

    void Initialize() {
        xoFatalIf(IsInitialized, "Double initialization of an entity is not allowed.");
        Scene* scene = Scene::GetCurrentScene();
        xoFatalIf(scene == nullptr, "No current scene");
        for (auto* comp : Components) {
            scene->OnComponentAdded(Owner, comp);
        }
        IsInitialized = true;
    }

};

Entity::Entity() {
    Impl = new EntityImpl(this);
}

Entity::~Entity() {
    delete Impl;
}

const Matrix4x4& Entity::GetTransform() {
    return Impl->GetTransform();
}

void Entity::GetWorldTransform(Matrix4x4& outMatrix) {
    Impl->GetWorldTransform(outMatrix);
}

void Entity::AddComponent(Component* component) {
    Impl->AddComponent(component);
}

void Entity::RemoveComponent(Component* component) {
    Impl->RemoveComponent(component);
}

void Entity::AddChild(Entity* child) {
    Impl->AddChild(child);
}

const Vector3& Entity::GetPosition() const {
    return Impl->Position;
}

const Vector3& Entity::GetScale() const {
    return Impl->Scale;
}

const Vector3& Entity::GetRotation() const {
    return Impl->Rotation;
}

void Entity::SetPosition(const Vector3& position) {
    Impl->Position = position;
    Impl->PositionDirty = true;
}

void Entity::SetScale(const Vector3& scale) {
    Impl->Scale = scale;
    Impl->ScaleDirty = true;
}

void Entity::SetRotation(const Vector3& rotation) {
    Impl->Rotation = rotation;
    Impl->RotationDirty = true;
}

// Scene API
void Entity::Initialize() {
    Impl->Initialize();
}
// End Scene API

}