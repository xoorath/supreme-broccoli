#include <Include/Component.h>
#include <Include/Macros.h>
#include <Include/Types.h>

namespace XO {

class Component_Model : public Component {
public:
    DECLARE_COMPONENT_TYPE(Component_Model);

    Component_Model(const char* modelPath);
    ~Component_Model();

    // Component Overrides
    virtual ComponentFlags GetComponentFlags() const override { 
        return (ComponentFlags)(ComponentFlags::Update | ComponentFlags::Render | ComponentFlags::InitRender);
    }
    virtual void InitRender(class Renderer& renderer) override;
    virtual void Update(float dt) override;
    virtual void Render(class Renderer& renderer, float dt) override;
    // End Component Overrides

private:
    class Component_ModelImpl* Impl;
};

}