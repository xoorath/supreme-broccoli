#pragma once

namespace XO {
class Entity;
namespace UI {

class SubWidget {
public:
    SubWidget() { };
    virtual ~SubWidget() { };

    virtual void Init(Entity* owner) { };
    virtual void Update(float dt) { };


};

}
}