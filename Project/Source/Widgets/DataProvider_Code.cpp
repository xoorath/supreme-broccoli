#include <Include/Widgets/DataProvider_Code.h>

#include <Include/Hash.h>
#include <Include/Log.h>
#include <Include/Types.h>

namespace XO {
namespace UI {

class DataProvider_CodeImpl {
public:
    DataProvider_CodeImpl(DataProvider_Code& owner) 
    : Assign(owner) {
    }

    DataProvider_Code::DataAssignment& GetAssign(const char* Target) {
        Assign.TargetName = Target;
        return Assign;
    }

    bool GetInt(const char* name, int32& outInt) const {
        auto found = Ints.find(name);
        if (found == Ints.end()) {
            return false;
        }
        outInt = found->second;
        return true;
    }

    bool GetFloat(const char* name, float32& outFloat) const {
        auto found = Floats.find(name);
        if (found == Floats.end()) {
            return false;
        }
        outFloat = found->second;
        return true;
    }

    bool GetBool(const char* name, bool& outBool) const {
        auto found = Bools.find(name);
        if (found == Bools.end()) {
            return false;
        }
        outBool = found->second;
        return true;
    }

    bool GetString(const char* name, String& outString) const {
        auto found = Strings.find(name);
        if (found == Strings.end()) {
            return false;
        }
        outString = found->second;
        return true;
    }

    void SetInt(const char* name, int32 value) {
        Ints[name] = value;
    }

    void SetFloat(const char* name, float32 value) {
        Floats[name] = value;
    }

    void SetBool(const char* name, bool value) {
        Bools[name] = value;
    }

    void SetString(const char* name, String value) {
        Strings[name] = value;
    }

private:
    DataProvider_Code::DataAssignment Assign;
    Map<const char*, int32> Ints;
    Map<const char*, float32> Floats;
    Map<const char*, bool> Bools;
    Map<const char*, String> Strings;
};

DataProvider_Code::DataProvider_Code() {
    Impl = new DataProvider_CodeImpl(*this);
}

DataProvider_Code::~DataProvider_Code() {
    delete Impl;
}

bool DataProvider_Code::GetInt(const char* name, int32& outInt) const {
    return Impl->GetInt(name, outInt) || (Next != nullptr && Next->GetInt(name, outInt));
}

bool DataProvider_Code::GetFloat(const char* name, float32& outFloat) const {
    return Impl->GetFloat(name, outFloat) || (Next != nullptr && Next->GetFloat(name, outFloat));
}

bool DataProvider_Code::GetBool(const char* name, bool& outBool) const {
    return Impl->GetBool(name, outBool) || (Next != nullptr && Next->GetBool(name, outBool));
}

bool DataProvider_Code::GetString(const char* name, String& outString) const {
    return Impl->GetString(name, outString) || (Next != nullptr && Next->GetString(name, outString));
}

void DataProvider_Code::SetInt(const char* name, int32 value) {
    Impl->SetInt(name, value);
}

void DataProvider_Code::SetFloat(const char* name, float32 value) {
    Impl->SetFloat(name, value);
}

void DataProvider_Code::SetBool(const char* name, bool value) {
    Impl->SetBool(name, value);
}

void DataProvider_Code::SetString(const char* name, String value) {
    Impl->SetString(name, value);
}

DataProvider_Code::DataAssignment::DataAssignment(DataProvider_Code& owner)
: Owner(owner)
, TargetName(nullptr) {
}

void DataProvider_Code::DataAssignment::operator = (int32 val) {
    Owner.SetInt(TargetName, val);
}

void DataProvider_Code::DataAssignment::operator = (float32 val) {
    Owner.SetFloat(TargetName, val);
}

void DataProvider_Code::DataAssignment::operator = (bool val) {
    Owner.SetBool(TargetName, val);
}

void DataProvider_Code::DataAssignment::operator = (String val) {
    Owner.SetString(TargetName, val);
}

DataProvider_Code::DataAssignment& DataProvider_Code::operator[](const char* name) {
    return Impl->GetAssign(name);
}

}
}