#include "../Include/Renderer.h"

#include "../Include/Log.h"
#include "../Include/Shader.h"

#if defined(GAME_WINDOWS)
#include "../ThirdParty/GL/glew.h"
#include "../ThirdParty/GL/wglew.h"
#include <GL/GL.h>
#endif

namespace XO {

#define BREAK_AFTER_BAD_GL() if(glGetError() != GL_NO_ERROR) __debugbreak();

class RendererImpl {
public:
    ShaderProgram SimpleShader;
    GLuint vertexbuffer;
    GLuint vao;

    RendererImpl()
    : SimpleShader("Shaders/Simple.vert", "Shaders/Simple.frag") {
        glClearColor(100.f / 255.f, 149.f / 255.f, 239.f / 255.f, 1.0);
        
        static const GLfloat g_vertex_buffer_data[] = {
            -1.0f, -1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            0.0f,  1.0f, 0.0f,
        };
        
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glGenBuffers(1, &vertexbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

        GLuint err;
        while ((err = glGetError()) != GL_NO_ERROR)
        {
            //Process/log the error.
            __debugbreak();
        }
    }

    ~RendererImpl() {
    }

    void Render() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        BREAK_AFTER_BAD_GL();
        SimpleShader.Use();
        BREAK_AFTER_BAD_GL();
        // 1rst attribute buffer : vertices
        glEnableVertexAttribArray(0);
        BREAK_AFTER_BAD_GL();
        glBindBuffer(GL_ARRAY_BUFFER, vao);
        BREAK_AFTER_BAD_GL();
        glVertexAttribPointer(
            0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
            );
        BREAK_AFTER_BAD_GL();
        // Draw the triangle !
        glDrawArrays(GL_TRIANGLES, 0, 3); // 3 indices starting at 0 -> 1 triangle
        BREAK_AFTER_BAD_GL();
        glDisableVertexAttribArray(0);
        BREAK_AFTER_BAD_GL();
    }

    void DebugLog() {
        xoLog( 
           "\n================================================================"
        << "\nGL_VENDOR                    | " << glGetString(GL_VENDOR)
        << "\nGL_RENDERER                  | " << glGetString(GL_RENDERER)
        << "\nGL_VERSION                   | " << glGetString(GL_VERSION)
        << "\nGL_SHADING_LANGUAGE_VERSION  | " << glGetString(GL_SHADING_LANGUAGE_VERSION)
        << "\n================================================================");

        GLuint err;
        while ((err = glGetError()) != GL_NO_ERROR)
        {
            //Process/log the error.
            __debugbreak();
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