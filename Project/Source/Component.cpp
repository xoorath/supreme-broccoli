#include <Include/Component.h>

#include <Include/Entity.h>
#include <ThirdParty/xo-math/xo-math.h>

namespace XO {
const Matrix4x4& Component::GetTransform() const {
    xoFatalIf(Owner == nullptr, "Owner not assigned.");
    return Owner->GetTransform();
}
}