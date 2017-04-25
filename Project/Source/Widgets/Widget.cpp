#include <Include/Widgets/Widget.h>

#include <Include/Log.h>
#include <Include/Widgets/DataProvider.h>

namespace XO {
namespace UI {

class WidgetImpl {
public:
    WidgetImpl(IDataProvider* dataProvider)
        : DataProvider(dataProvider) {
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

private:
    IDataProvider* DataProvider;
};

Widget::Widget(IDataProvider* rootDataProvider) {
    Impl = new WidgetImpl(rootDataProvider);
}

Widget::~Widget() {
    delete Impl;
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

}
}