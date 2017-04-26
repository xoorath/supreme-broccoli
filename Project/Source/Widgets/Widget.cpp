#include <Include/Widgets/Widget.h>

#include <Include/Log.h>
#include <Include/TextHelpers.h>
#include <Include/Widgets/DataProvider.h>
#include <Include/Widgets/SubWidget.h>

namespace XO {
namespace UI {

class WidgetImpl {
public:
    WidgetImpl(Widget* owner, IDataProvider* dataProvider)
        : OwningWidget(owner)
        , DataProvider(dataProvider) {
    }

    void InternalInit(Entity* owner) {
        OwningEntity = owner;
        OwningWidget->Init(OwningEntity);
    }

    void InternalUpdate(float dt) {
        OwningWidget->Update(dt);
        for (SubWidget* subWidget : SubWidgets) {
            subWidget->Update(dt);
        }
    }

    bool GetColor(const char* name, float outColorRGBA[4], DataRequirement requirement, float defaultValue[4]) {
        if (DataProvider) {
            String tempString;
            if (!DataProvider->GetString(name, tempString)) {
                memcpy(outColorRGBA, defaultValue, sizeof(float32) * 4);
                xoFatalIf(requirement == Required, "Failed to get required widget data: (color)" << name);
                return false;
            }
            if (!TryConvertStringToRGBA(tempString, outColorRGBA)) {
                xoFatalIf(requirement == Required, "Failed to convert color from string: (color)" << name << " Value: \"" << tempString << "\"");
                return false;
            }
            return true;
        }
        return false;
    }

    bool GetInt(const char* name, int32& outInt, DataRequirement requirement, int32 defaultValue) const {
        if (DataProvider) {
            if (!DataProvider->GetInt(name, outInt)) {
                outInt = defaultValue;
                xoFatalIf(requirement == Required, "Failed to get required widget data: (int)" << name);
                return false;
            }
            return true;
        }
        return false;
    }

    bool GetFloat(const char* name, float32& outFloat, DataRequirement requirement, float32 defaultValue) const {
        if (DataProvider) {
            if (!DataProvider->GetFloat(name, outFloat)) {
                outFloat = defaultValue;
                xoFatalIf(requirement == Required, "Failed to get required widget data: (float)" << name);
                return false;
            }
            return true;
        }
        return false;
    }

    bool GetBool(const char* name, bool& outBool, DataRequirement requirement, bool defaultValue) const {
        if (DataProvider) {
            if (!DataProvider->GetBool(name, outBool)) {
                outBool = defaultValue;
                xoFatalIf(requirement == Required, "Failed to get required widget data: (bool)" << name);
                return false;
            }
            return true;
        }
        return false;
    }

    bool GetString(const char* name, String& outString, DataRequirement requirement, String defaultValue) const {
        if (DataProvider) {
            if (!DataProvider->GetString(name, outString)) {
                outString = defaultValue;
                xoFatalIf(requirement == Required, "Failed to get required widget data: (string)" << name);
                return false;
            }
            return true;
        }
        return false;
    }

    void AddSubwidget(SubWidget* subWidget) {
        xoFatalIf(subWidget == nullptr, "Subwidget can't be null.");
        SubWidgets.push_back(subWidget);
        subWidget->Init(OwningEntity);
    }

    void RemoveSubwidget(SubWidget* subWidget) {
        auto found = std::find(SubWidgets.begin(), SubWidgets.end(), subWidget);
        xoFatalIf(found == SubWidgets.end(), "Can't remove a subwidget that isn't currently attached.");
        SubWidgets.erase(found);
    }

    Entity* GetOwner() const {
        return OwningEntity;
    }

private:
    Widget* OwningWidget;
    Entity* OwningEntity;
    IDataProvider* DataProvider;
    Array<SubWidget*> SubWidgets;
};

Widget::Widget(IDataProvider* rootDataProvider) {
    Impl = new WidgetImpl(this, rootDataProvider);
}

Widget::~Widget() {
    delete Impl;
}

bool Widget::GetColor(const char* name, float outColorRGBA[4], DataRequirement requirement/*=Optional*/, float defaultValue[4]/*={}*/) const {
    return Impl->GetColor(name, outColorRGBA, requirement, defaultValue);
}

bool Widget::GetInt(const char* name, int32& outInt, DataRequirement requirement/*=Optional*/, int32 defaultValue/*=0*/) const {
    return Impl->GetInt(name, outInt, requirement, defaultValue);
}

bool Widget::GetFloat(const char* name, float32& outFloat, DataRequirement requirement/*=Optional*/, float32 defaultValue/*=0.0f*/) const {
    return Impl->GetFloat(name, outFloat, requirement, defaultValue);
}

bool Widget::GetBool(const char* name, bool& outBool, DataRequirement requirement/*=Optional*/, bool defaultValue/*=false*/) const {
    return Impl->GetBool(name, outBool, requirement, defaultValue);
}

bool Widget::GetString(const char* name, String& outString, DataRequirement requirement/*=Optional*/, String defaultValue/*=""*/) const {
    return Impl->GetString(name, outString, requirement, defaultValue);
}

void Widget::AddSubwidget(SubWidget* subWidget) {
    Impl->AddSubwidget(subWidget);
}

void Widget::RemoveSubwidget(SubWidget* subWidget) {
    Impl->RemoveSubwidget(subWidget);
}

Entity* Widget::GetOwner() const {
    return Impl->GetOwner();
}

////////////////////////////////////////////////////////////////////////// Component_Widget API
void Widget::InternalInit(Entity* owner) {
    Impl->InternalInit(owner);
}

void Widget::InternalUpdate(float dt) {
    Impl->InternalUpdate(dt);
}
// End Component_Widget API

}
}