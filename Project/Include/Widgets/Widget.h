#pragma once

#include <Include/Types.h>

namespace XO {
namespace UI {

enum DataRequirement {
    Required,
    Optional
};

class Widget {
protected:
    friend class Component_Widget;
    virtual void Init(class Entity* owner) { };
    virtual void Update(float dt) { };

    bool GetInt(const char* name, int32& outInt, DataRequirement requirement = Optional, int32 defaultValue = 0) const;
    bool GetFloat(const char* name, float32& outFloat, DataRequirement requirement = Optional, float32 defaultValue = 0.0f) const;
    bool GetBool(const char* name, bool& outBool, DataRequirement requirement = Optional, bool defaultValue = false) const;
    bool GetString(const char* name, String& outString, DataRequirement requirement = Optional, String defaultValue = "") const;

protected:
    Widget(class IDataProvider* rootDataProvider);
    virtual ~Widget();

private:
    class WidgetImpl* Impl;
};

}
}