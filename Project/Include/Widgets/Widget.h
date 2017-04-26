#pragma once

#include <Include/Types.h>

namespace XO {
class Entity;
namespace UI {

enum DataRequirement {
    Required,
    Optional
};

class Widget {
protected:
    friend class Component_Widget;
    friend class WidgetImpl;

    virtual void Init(Entity* owner) { };
    virtual void Update(float dt) { };

    bool GetColor(const char* name, float outColorRGBA[4], DataRequirement requirement = Optional, float defaultValue[4] = {}) const;
    bool GetInt(const char* name, int32& outInt, DataRequirement requirement = Optional, int32 defaultValue = 0) const;
    bool GetFloat(const char* name, float32& outFloat, DataRequirement requirement = Optional, float32 defaultValue = 0.0f) const;
    bool GetBool(const char* name, bool& outBool, DataRequirement requirement = Optional, bool defaultValue = false) const;
    bool GetString(const char* name, String& outString, DataRequirement requirement = Optional, String defaultValue = "") const;

    void AddSubwidget(class SubWidget* subWidget);
    void RemoveSubwidget(class SubWidget* subWidget);

    class Entity* GetOwner() const;

protected:
    Widget(class IDataProvider* rootDataProvider);
    virtual ~Widget();

private:
    // Component_Widget API
    void InternalInit(Entity* owner);
    void InternalUpdate(float dt);
    // End Component_Widget API

    class WidgetImpl* Impl;
};

}
}