#include <Include/Component.h>

#include <Include/Entity.h>
#include <ThirdParty/xo-math/xo-math.h>

namespace XO {
const Matrix4x4& Component::GetTransform() const {
    xoFatalIf(Owner == nullptr, "Owner not assigned.");
    return Owner->GetTransform();
}

void Component::GetWorldTransform(Matrix4x4& outMatrix) const {
    xoFatalIf(Owner == nullptr, "Owner not assigned.");
    Owner->GetWorldTransform(outMatrix);
}
}