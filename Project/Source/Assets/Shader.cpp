#include <Include/Assets/Shader.h>

#include <Include/Assets/AssetManager.h>
#include <Include/Log.h>
#include <Include/Types.h>

#include <fstream>

#if defined(GAME_WINDOWS)
#include <ThirdParty/GL/glew.h>
#include <ThirdParty/GL/wglew.h>
#include <GL/GL.h>
#endif


namespace XO {
static_assert(sizeof(GLuint) == sizeof(uint32), "Size of GLuint is not what was expected by Shader.h");

uint32 ShaderProgram::LastUsedProgramID = -1;

String GetProgramCode(const char* programPath) {
    String fullpath(AssetManager::AssetsRoot());
    std::ifstream stream(fullpath + "/" + programPath);
    if (!stream.is_open()) {
        xoFatal("Could not open shader at path " << programPath << "!");
        return "";
    }
    stream.seekg(0, std::ios::end);
    size_t size = (size_t)stream.tellg();
    String code(size, ' ');
    stream.seekg(0);
    stream.read(&code[0], size);
    return code;
}

ShaderProgram::Uniform::Uniform(uint32 program, uint32 location) 
    : ProgramID(program)
    , UniformLocation(location) {
}

ShaderProgram::Uniform::Uniform()
    : ProgramID(-1)
    , UniformLocation(-1) {

}

void ShaderProgram::Uniform::SetAsMatrix4x4(float32* data) {
    if (UniformLocation != -1) {
        glUniformMatrix4fv(UniformLocation, 1, GL_FALSE, data);
    }
}

void ShaderProgram::Uniform::SetAsVector3(float32* data) {
    if (UniformLocation != -1) {
        glUniform3f(UniformLocation, data[0], data[1], data[2]);
    }
}

void ShaderProgram::Uniform::SetTextureUnit(uint32 unit) {
    if (UniformLocation != -1) {
        glUniform1i(UniformLocation, unit);
    }
}

ShaderProgram::ShaderProgram()
    : ProgramID(-1) {

}

ShaderProgram::ShaderProgram(const char* vertShaderPath, const char* fragShaderPath)
    : ProgramID(-1) {
    Load(vertShaderPath, fragShaderPath);
}

void ShaderProgram::Load(const char* vertShaderPath, const char* fragShaderPath) {
    xoErrIf(ProgramID != -1, "Shader program already loaded! Trying to load over top with: " << vertShaderPath << ", " << fragShaderPath);

    String vert = GetProgramCode(vertShaderPath);
    String frag = GetProgramCode(fragShaderPath);

    GLuint vid = glCreateShader(GL_VERTEX_SHADER);
    GLuint fid = glCreateShader(GL_FRAGMENT_SHADER);

    GLint result = GL_FALSE;

    int infoLength;

    xoLog("Compiling vertex shader: " << vertShaderPath);
    char const * VertexSourcePointer = vert.c_str();
    glShaderSource(vid, 1, &VertexSourcePointer, NULL);
    glCompileShader(vid);

    glGetShaderiv(vid, GL_COMPILE_STATUS, &result);
    glGetShaderiv(vid, GL_INFO_LOG_LENGTH, &infoLength);
    if (infoLength > 0) {
        char* msg = new char[infoLength + 1];
        glGetShaderInfoLog(vid, infoLength, NULL, msg);
        xoLog("Result: " << msg);
        delete[] msg;
    }

    xoLog("Compiling fragment shader: " << fragShaderPath);
    char const * FragSourcePointer = frag.c_str();
    glShaderSource(fid, 1, &FragSourcePointer, NULL);
    glCompileShader(fid);

    glGetShaderiv(fid, GL_COMPILE_STATUS, &result);
    glGetShaderiv(fid, GL_INFO_LOG_LENGTH, &infoLength);
    if (infoLength > 0) {
        char* msg = new char[infoLength + 1];
        glGetShaderInfoLog(fid, infoLength, NULL, msg);
        xoLog("Result: " << msg);
        delete[] msg;
    }

    // Link the program
    xoLog("Linking shader.");
    ProgramID = glCreateProgram();
    glAttachShader(ProgramID, vid);
    glAttachShader(ProgramID, fid);
    glLinkProgram(ProgramID);

    // Check the program
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &infoLength);
    if (infoLength > 0) {
        char* msg = new char[infoLength + 1];
        glGetProgramInfoLog(ProgramID, infoLength, NULL, msg);
        xoLog("Result: " << msg);
        delete[] msg;
    }


    glDetachShader(ProgramID, vid);
    glDetachShader(ProgramID, fid);

    glDeleteShader(vid);
    glDeleteShader(fid);
}

void ShaderProgram::Unload() {
    if (ProgramID != -1) {
        glDeleteProgram((GLuint)ProgramID);
        ProgramID = -1;
    }
}

ShaderProgram::~ShaderProgram() {
    Unload();
}

void ShaderProgram::Use() {
    //if (LastUsedProgramID != ProgramID) 
    {
        glUseProgram((GLuint)ProgramID);
        LastUsedProgramID = ProgramID;
    }
}

ShaderProgram::Uniform ShaderProgram::operator[](String UniformLocation) const {
    return Uniform(ProgramID, glGetUniformLocation(ProgramID, UniformLocation.c_str()));
}

bool ShaderProgram::IsValid() const {
    return ProgramID != -1;
}

}