#pragma once

#include <Include/Hash.h>

namespace XO {

enum ComponentFlags {
    None = 0,
    Init = 1,
    Update = 2,
    Render = 4,
    InitRender = 8,

    All = Init | Update | Render | InitRender
};

#define DECLARE_COMPONENT_TYPE(name) \
    typedef Component Super;\
    static constexpr unsigned ComponentTypeID = CRC32::Get(#name); \
    virtual unsigned GetComponentTypeId() const override { return ComponentTypeID; }

class Component {
public:
    virtual unsigned GetComponentTypeId() const = 0;
    virtual ComponentFlags GetComponentFlags() const = 0;

    virtual void Init() { };
    virtual void InitRender(class Renderer& renderer) { };
    virtual void Update(float dt) { };
    virtual void Render(class Renderer& renderer, float dt) { };

    const class Matrix4x4& GetTransform() const;

private:
    friend class EntityImpl; // assigns our owner.
    class Entity* Owner;
};

}