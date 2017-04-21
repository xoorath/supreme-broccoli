#pragma once

#include "Types.h"

namespace XO {

class ShaderProgram {
public:
    ShaderProgram(const char* vertShaderPath, const char* fragShaderPath);
    ~ShaderProgram();

    void Use();
    bool IsValid() const;

    static void Prepare(class Config& engineConfig);

private:
    static uint32 LastUsedProgramID;
    uint32 ProgramID;
};

}