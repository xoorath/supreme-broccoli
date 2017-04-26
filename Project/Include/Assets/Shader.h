#pragma once

#include <Include/Types.h>

namespace XO {

class ShaderProgram {
public:
    struct Uniform {
    public:
        Uniform();
        Uniform(uint32 program, uint32 location);

        void SetAsMatrix4x4(float32* data);
        void SetAsVector3(float32* data);
        void SetAsVector4(float32* data);
        void SetTextureUnit(uint32 unit);

    private:
        uint32 ProgramID;
        uint32 UniformLocation;
    };


    ShaderProgram();
    ShaderProgram(const char* vertShaderPath, const char* fragShaderPath);
    ~ShaderProgram();

    void Use();
    bool IsValid() const;

    void Load(const char* vertShaderPath, const char* fragShaderPath);
    void Unload();

    // Retrieves a uniform location
    Uniform operator[](String UniformLocation) const;

private:
    static uint32 LastUsedProgramID;
    uint32 ProgramID;
};

}