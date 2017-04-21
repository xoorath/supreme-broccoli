#include <Include/Renderer.h>"

#include <Include/Assets/Shader.h>
#include <Include/Assets/Texture2D.h>
#include <Include/Log.h>

#if defined(GAME_WINDOWS)
#include <ThirdParty/GL/glew.h>
#include <ThirdParty/GL/wglew.h>
#include <GL/GL.h>
#endif

namespace XO {

class RendererImpl {
public:
    ShaderProgram SimpleShader;
    ShaderProgram::Uniform MVP;
    ShaderProgram::Uniform Sampler;
    Texture2D Placeholder;
    GLuint vertexbuffer, uvbuffer;
    GLuint vao, uao;

    RendererImpl()
    : SimpleShader("Shaders/Simple.vert", "Shaders/Simple.frag") {
        glClearColor(100.f / 255.f, 149.f / 255.f, 239.f / 255.f, 1.0);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glEnable(GL_CULL_FACE);

        Placeholder.Load("Textures/Testing.png");
        

        MVP = SimpleShader["MVP"];
        Sampler = SimpleShader["myTextureSampler"];


        static const GLfloat g_vertex_buffer_data[] = {
            -1, 1, 0,
            -1, -1, 0,
            1, -1, 0,

            1, -1, 0,
            1, 1, 0,
            -1, 1, 0
        };
        
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glGenBuffers(1, &vertexbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

        static const GLfloat g_uv_buffer_data[] = {
            0, 1,
            0, 0,
            1, 0,

            1, 0,
            1, 1,
            0, 1
        };

        glGenVertexArrays(1, &uao);
        glBindVertexArray(uao);
        glGenBuffers(1, &uvbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(g_uv_buffer_data), g_uv_buffer_data, GL_STATIC_DRAW);


        ReportAnyGLErrors();

    }

    ~RendererImpl() {
    }

    void Render() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        SimpleShader.Use();


        static  float mvp[] = {
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
        };

        MVP.SetAsMatrix4x4(mvp);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Placeholder.GetTextureID());
        // Set our "myTextureSampler" sampler to user Texture Unit 0
        Sampler.SetTextureUnit(0);


        // 1rst attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vao);
        glVertexAttribPointer(
            0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
            );
        
        
        // 2nd attribute buffer : UVs
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, uao);
        glVertexAttribPointer(
            1,                                // attribute
            2,                                // size
            GL_FLOAT,                         // type
            GL_FALSE,                         // normalized?
            0,                                // stride
            (void*)0                          // array buffer offset
            );
        
        
        
        // Draw the triangle !
        glDrawArrays(GL_TRIANGLES, 0, 6); // 3 indices starting at 0 -> 1 triangle
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);

        ReportAnyGLErrors();
    }

    void DebugLog() {
        xoLog( 
           "\n================================================================"
        << "\nGL_VENDOR                    | " << glGetString(GL_VENDOR)
        << "\nGL_RENDERER                  | " << glGetString(GL_RENDERER)
        << "\nGL_VERSION                   | " << glGetString(GL_VERSION)
        << "\nGL_SHADING_LANGUAGE_VERSION  | " << glGetString(GL_SHADING_LANGUAGE_VERSION)
        << "\n================================================================");
    }

private:
    void ReportAnyGLErrors() {
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
        }
    }
};

Renderer::Renderer() : Impl(nullptr) {
}

Renderer::~Renderer() {
    delete Impl;
}

void Renderer::Init() {
    xoFatalIf(Impl, "Renderer should not be initialized twice.");
    Impl = new RendererImpl();
}

void Renderer::Render() {
    if (Impl) {
        Impl->Render();
    }
}

void Renderer::DebugLog() {
    if (Impl) {
        Impl->DebugLog();
    }
}


}