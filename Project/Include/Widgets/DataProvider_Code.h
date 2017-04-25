#pragma once

#include <Include/Widgets/DataProvider.h>

namespace XO {
namespace UI {

class DataProvider_Code : public IDataProvider {
public:
    DataProvider_Code();
    ~DataProvider_Code();

    virtual bool GetInt(const char* name, int32& outInt) const override;
    virtual bool GetFloat(const char* name, float32& outFloat) const override;
    virtual bool GetBool(const char* name, bool& outBool) const override;
    virtual bool GetString(const char* name, String& outString) const override;

    void SetInt(const char* name, int32 value);
    void SetFloat(const char* name, float32 value);
    void SetBool(const char* name, bool value);
    void SetString(const char* name, String value);

    // Helper class to allow a nicer assignment interface.
    class DataAssignment {
    public:
        void operator = (int32 val);
        void operator = (float32 val);
        void operator = (bool val);
        void operator = (String val);
    private:
        friend class DataProvider_CodeImpl;
        DataAssignment(DataProvider_Code& owner);
        DataProvider_Code& Owner;
        const char* TargetName;
    };

    // Usage:
    //      DataProvider_Code MyProvider;
    //      MyProvider["NewsfeedStories"]       = 10;
    //      MyProvider["NewsfeedScrollSpeed"]   = 1.0f;
    //      MyProvider["NewsfeedHasDate"]       = false;
    //      MyProvider["NewsfeedText"]          = "Lorem Ipsum...";
    class DataAssignment& operator[](const char* name);

private:
    class DataProvider_CodeImpl* Impl;
};

}
}