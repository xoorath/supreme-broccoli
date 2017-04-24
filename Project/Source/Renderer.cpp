#include <Include/Renderer.h>

#include <Include/Assets/Shader.h>
#include <Include/Assets/Texture2D.h>
#include <Include/Assets/Model.h>
#include <Include/Log.h>
#include <ThirdParty/xo-math/xo-math.h>

#include <Include/Component_Model.h>

#if defined(GAME_WINDOWS)
#include <ThirdParty/GL/glew.h>
#include <ThirdParty/GL/wglew.h>
#include <GL/GL.h>
#endif

#define checkgl() if(ReportAnyGLErrors()) { static bool once = false; if(!once) { __debugbreak(); once = true; } }

namespace XO {

class Renderable {
public:
    GLuint VBO = -1; // vertex buffer object
    GLuint IBO = -1; // index buffer object
    uint32 NumTriangles = 0;
    Texture2D Texture;

    void Init(ModelData& data) {

        xoFatalIf(data.Meshes.size() != 1, "Only one mesh per model is supported currently.");

        auto& mesh = data.Meshes[0];

        NumTriangles = data.Meshes[0].NumTriangles;

        GLuint buffers[2];
        glGenBuffers(2, buffers); checkgl();
        VBO = buffers[0];
        IBO = buffers[1];

        glBindBuffer(GL_ARRAY_BUFFER, VBO); checkgl();
        glBufferData(GL_ARRAY_BUFFER, mesh.VertexBufferSize, mesh.VertexBuffer, GL_STATIC_DRAW); checkgl();

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO); checkgl();
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.IndexBufferSize, mesh.IndexBuffer, GL_STATIC_DRAW); checkgl();

        Texture = mesh.Texture;
    }
    
    void Render(const Matrix4x4& Transform) {
        if (IBO != -1) {
            glBindTexture(GL_TEXTURE_2D, Texture.GetTextureID());

            constexpr uint32 stride = sizeof(float32) * (3 + 3 + 2);

            glEnableVertexAttribArray(0); checkgl();
            glBindBuffer(GL_ARRAY_BUFFER, VBO); checkgl();
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0); checkgl();

            glEnableVertexAttribArray(1); checkgl();
            glBindBuffer(GL_ARRAY_BUFFER, VBO); checkgl();
            glVertexAttribPointer(1,3, GL_FLOAT, GL_FALSE, stride, (void*)12); checkgl();


            glEnableVertexAttribArray(2); checkgl();
            glBindBuffer(GL_ARRAY_BUFFER, VBO); checkgl();
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)24); checkgl();

            // Index buffer
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO); checkgl();

            glDrawElements(GL_TRIANGLES, NumTriangles*3, GL_UNSIGNED_INT, (void*)0); checkgl();

            glDisableVertexAttribArray(0); checkgl();
            glDisableVertexAttribArray(1); checkgl();
            glDisableVertexAttribArray(2); checkgl();
        }
    }

    bool ReportAnyGLErrors() const {
        auto err = glGetError();
        if (err != GL_NO_ERROR) {
            switch (err) {
                case GL_INVALID_ENUM:
                    xoErr("OpenGL Error: Invalid enum. " << err);
                    break;
                case GL_INVALID_VALUE:
                    xoErr("OpenGL Error: Invalid value. " << err);
                    break;
                case GL_INVALID_OPERATION:
                    xoErr("OpenGL Error: Invalid operation. " << err);
                    break;
                case GL_STACK_OVERFLOW:
                    xoErr("OpenGL Error: Stack overflow. " << err);
                    break;
                case GL_STACK_UNDERFLOW:
                    xoErr("OpenGL Error: Stack underflow. " << err);
                    break;
                case GL_OUT_OF_MEMORY:
                    xoErr("OpenGL Error: Out of memory. " << err);
                    break;
            }
            return true;
        }
        return false;
    }

    ~Renderable() {
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &IBO);
    }
};

_XOSIMDALIGN struct RenderJob {
    Renderable* RenderableAsset;
    Matrix4x4 Transform;
};

_XOSIMDALIGN class RendererImpl {
public:
    _XO_OVERLOAD_NEW_DELETE();

    ShaderProgram SimpleShader;

    ShaderProgram::Uniform UModelViewProjectionMatrix;
    ShaderProgram::Uniform UModelViewMatrix;
    ShaderProgram::Uniform UModelMatrix;
    ShaderProgram::Uniform UViewMatrix;
    ShaderProgram::Uniform UProjectionMatrix;
    ShaderProgram::Uniform ULightPos;

    Matrix4x4 ViewMatrix;
    Matrix4x4 ProjectionMatrix;
    Vector3 LightPos;

    ShaderProgram::Uniform Sampler;

    Model Cube;
    Texture2D Placeholder;

    Array<Renderable> Renderables;
    float curr_quat[4];
    float prev_quat[4];

    RendererImpl() {
        CurrentJobs.reserve(1024);
     }

    void Init() {
        SimpleShader.Load("Shaders/Simple.vert", "Shaders/Simple.frag");
        glClearColor(100.f / 255.f, 149.f / 255.f, 239.f / 255.f, 1.0);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glEnable(GL_CULL_FACE);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        GLuint VertexArrayID;
        glGenVertexArrays(1, &VertexArrayID);
        glBindVertexArray(VertexArrayID);

        checkgl();

        Placeholder.Load("Textures/Testing.png");

        Cube.OnLoaded.Add([this](ModelData& data) {
            Renderables.push_back(Renderable());
            Renderable& model = Renderables[Renderables.size() - 1];

            model.Init(data);
        });

        Cube.Load("Models/Cube/cube.ini");

        UModelViewProjectionMatrix = SimpleShader["MVP"];
        UModelViewMatrix = SimpleShader["MV"];
        UModelMatrix = SimpleShader["M"];
        UViewMatrix = SimpleShader["V"];
        UProjectionMatrix = SimpleShader["P"];
        ULightPos = SimpleShader["LightPosition_worldspace"];

        LightPos.Set(4, 4, 4);

        Sampler = SimpleShader["myTextureSampler"];

        ProjectionMatrix = Matrix4x4::PerspectiveProjectionDegrees(60, 1280.0f / 720.0f, 0.1f, 100);


        checkgl();
    }

    void RenderScene() {
        Matrix4x4 MVP, MV;

        for (auto & job : CurrentJobs) {
            MV = ViewMatrix * job.Transform;
            MVP = ProjectionMatrix * MV;

            UModelMatrix.SetAsMatrix4x4(job.Transform);
            UModelViewProjectionMatrix.SetAsMatrix4x4(MVP);
            UModelViewMatrix.SetAsMatrix4x4(MV);

            job.RenderableAsset->Render(job.Transform);
        }

        CurrentJobs.clear();
    }

    void UpdateCamera() {
        static float gt = 0;
        gt += 0.001f;

        const float radius = 10.0f;
        Vector3 cameraPos = Vector3::Zero;
        cameraPos.x = XO::Cos(gt) * radius;
        cameraPos.z = XO::Sin(gt) * radius;

        Matrix4x4::LookAtFromPosition(cameraPos, Vector3::Zero, Vector3::Up, ViewMatrix);

        ViewMatrix = Matrix4x4::Identity;
    }

    void Render() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        SimpleShader.Use();

        glActiveTexture(GL_TEXTURE0);
        Sampler.SetTextureUnit(0);

        ULightPos.SetAsVector3(LightPos);

        UpdateCamera();
        UViewMatrix.SetAsMatrix4x4(ViewMatrix);
        UProjectionMatrix.SetAsMatrix4x4(ProjectionMatrix);

        RenderScene();
        checkgl();
        
    }

    void DebugLog() {
        xoLog( 
           "\n================================================================ OpenGL"
        << "\nGL_VENDOR                    | " << glGetString(GL_VENDOR)
        << "\nGL_RENDERER                  | " << glGetString(GL_RENDERER)
        << "\nGL_VERSION                   | " << glGetString(GL_VERSION)
        << "\nGL_SHADING_LANGUAGE_VERSION  | " << glGetString(GL_SHADING_LANGUAGE_VERSION)
        << "\n================================================================");
    }

    uint32 CurrentJobId = 0;
    Map<uint32, Renderable*> RenderableJobs;
    Array<RenderJob> CurrentJobs;

    uint32 PrepareJob(ModelData& modelData) {
        Renderable* r = new Renderable();
        r->Init(modelData);
        CurrentJobId++;
        RenderableJobs[CurrentJobId] = r;
        return CurrentJobId;
    }

    void SubmitJob(uint32 renderable, const Matrix4x4& transform) {
        CurrentJobs.push_back(RenderJob());
        RenderJob& thisJob = CurrentJobs[CurrentJobs.size() - 1];
        thisJob.Transform = transform;
        thisJob.RenderableAsset = RenderableJobs[renderable];
    }

private:
    bool ReportAnyGLErrors() const {
        auto err = glGetError();
        if (err != GL_NO_ERROR) {
            switch (err) {
                case GL_INVALID_ENUM:
                    xoErr("OpenGL Error: Invalid enum. " << err);
                    break;
                case GL_INVALID_VALUE:
                    xoErr("OpenGL Error: Invalid value. " << err);
                    break;
                case GL_INVALID_OPERATION:
                    xoErr("OpenGL Error: Invalid operation. " << err);
                    break;
                case GL_STACK_OVERFLOW:
                    xoErr("OpenGL Error: Stack overflow. " << err);
                    break;
                case GL_STACK_UNDERFLOW:
                    xoErr("OpenGL Error: Stack underflow. " << err);
                    break;
                case GL_OUT_OF_MEMORY:
                    xoErr("OpenGL Error: Out of memory. " << err);
                    break;
            }
            return true;
        }
        return false;
    }
};

Renderer::Renderer() {
    Impl = new RendererImpl();
}

Renderer::~Renderer() {
    delete Impl;
}

void Renderer::Init() {
    Impl->Init();
}

void Renderer::Render() {
    Impl->Render();
}

void Renderer::DebugLog() {
    Impl->DebugLog();
}

uint32 Renderer::PrepareJob(struct ModelData& modelData) {
    return Impl->PrepareJob(modelData);
}

void Renderer::SubmitJob(uint32 job, const class Matrix4x4& transform) {
    Impl->SubmitJob(job, transform);
}

}