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

#define checkgl() if(glGetError() != GL_NONE) { static bool once = false; if(!once) { __debugbreak(); once = true; } }

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
        glGenBuffers(2, buffers);
        VBO = buffers[0];
        IBO = buffers[1];

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, mesh.VertexBufferSize, mesh.VertexBuffer, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.IndexBufferSize, mesh.IndexBuffer, GL_STATIC_DRAW);

        Texture = mesh.Texture;
    }
    
    void Render(const RenderJobParams& params) {
        if (IBO != -1) {
            glBindTexture(GL_TEXTURE_2D, Texture.GetTextureID());

            constexpr uint32 stride = sizeof(float32) * (3 + 3 + 2);

            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0); 

            glEnableVertexAttribArray(1);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glVertexAttribPointer(1,3, GL_FLOAT, GL_FALSE, stride, (void*)12);


            glEnableVertexAttribArray(2);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)24);

            // Index buffer
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

            glDrawElements(GL_TRIANGLES, NumTriangles*3, GL_UNSIGNED_INT, (void*)0);

            glDisableVertexAttribArray(0);
            glDisableVertexAttribArray(1);
            glDisableVertexAttribArray(2);
        }
    }

    ~Renderable() {
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &IBO);
    }
};

_XOSIMDALIGN struct RenderJob {
    Renderable* RenderableAsset;
    RenderJobParams Params;
};

_XOSIMDALIGN class RendererImpl {
public:
    _XO_OVERLOAD_NEW_DELETE();

    ShaderProgram SimpleShader;

    ShaderProgram::Uniform UModelProjectionMatrix;
    ShaderProgram::Uniform ULightDirection;
    ShaderProgram::Uniform UTint;
    ShaderProgram::Uniform USampler;

    Array<Renderable> Renderables;

    Matrix4x4 ProjectionMatrix;

    RendererImpl() {
        CurrentJobs.reserve(1024);
     }

    void Init() {
        SimpleShader.Load("Shaders/Simple.vert", "Shaders/Simple.frag");
        //23, 27, 36
        glClearColor(23.f / 255.f, 27.f / 255.f, 36.f / 255.f, 1.0);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glEnable(GL_CULL_FACE);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        GLuint VertexArrayID;
        glGenVertexArrays(1, &VertexArrayID);
        glBindVertexArray(VertexArrayID);

        checkgl();

        SimpleShader.Use(); 

        UModelProjectionMatrix = SimpleShader["MP"];
        ULightDirection = SimpleShader["LightDirection"];;
        UTint = SimpleShader["Tint"];
        USampler = SimpleShader["Sampler"];

        Vector3 LightDirection(1.0f, 1.0f, 1.0f);
        LightDirection.Normalize();
        ULightDirection.SetAsVector3(LightDirection); ;

        ProjectionMatrix = Matrix4x4::PerspectiveProjectionDegrees(60, 1280.0f / 720.0f, 0.1f, 100);

        checkgl();
    }

    void RenderScene() {
        
        Matrix4x4 MP;
        for (auto & job : CurrentJobs) {

            MP = job.Params.Transform * ProjectionMatrix;
            UModelProjectionMatrix.SetAsMatrix4x4(MP);
            UTint.SetAsVector4(job.Params.RGBA);

            job.RenderableAsset->Render(job.Params);
        }

        CurrentJobs.clear();
    }

    void Render() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        SimpleShader.Use();

        glActiveTexture(GL_TEXTURE0);
        USampler.SetTextureUnit(0);

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

    void SubmitJob(const RenderJobParams& params) {
        CurrentJobs.push_back(RenderJob());
        RenderJob& thisJob = CurrentJobs[CurrentJobs.size() - 1];
        thisJob.RenderableAsset = RenderableJobs[params.JobId];
        thisJob.Params = params;
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

void Renderer::SubmitJob(const RenderJobParams& params) {
    Impl->SubmitJob(params);
}

}