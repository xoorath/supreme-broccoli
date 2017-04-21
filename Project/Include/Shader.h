#pragma once

#include "Types.h"

namespace XO {

class ShaderProgram {
public:
    struct Uniform {
    public:
        Uniform();
        Uniform(uint32 program, uint32 location);

        void SetAsMatrix4x4(float32* data);
        void SetTextureUnit(uint32 unit);

    private:
        uint32 ProgramID;
        uint32 UniformLocation;
    };


    ShaderProgram(const char* vertShaderPath, const char* fragShaderPath);
    ~ShaderProgram();

    void Use();
    bool IsValid() const;

    // Retrieves a uniform location
    Uniform operator[](String UniformLocation) const;

    // TODO: centralize with other asset prepping.
    static void Prepare(class Config& engineConfig);

private:
    static uint32 LastUsedProgramID;
    uint32 ProgramID;
};

}