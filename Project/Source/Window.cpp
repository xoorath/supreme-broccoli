#include "../Include/Window.h"

#include "../Include/Log.h"
#include "../Include/TextHelpers.h"

#if defined(GAME_WINDOWS)
#include "../ThirdParty/GL/glew.h"
#include "../ThirdParty/GL/wglew.h"
#include <GL/GL.h>
#include <windowsx.h>
#include <Commdlg.h>
#include <shellapi.h>
#endif

#define ensure(cnd, msg) {if(!(cnd)) { xoFatal(msg); __debugbreak(); Close(); return false; }}

namespace XO {
class WindowImpl {
public:
    static WindowImpl* Self;
    Window& Owner;
    Config& EngineConfig;

    WindowImpl(Window& owner, Config& config) 
        : Owner(owner)
        , EngineConfig(config) {
        Self = this;
    }

#if defined(GAME_WINDOWS)
    HWND Hwnd;
    HDC Hdc;
    HINSTANCE Hinstance;
    HGLRC Hglrc;

    LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        switch (msg) {
            case WM_CLOSE:
            case WM_QUIT:
                Close();
                break;
        }
        return DefWindowProc(hwnd, msg, wParam, lParam);;
    }

    static LRESULT CALLBACK StaticWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        return Self->WndProc(hwnd, msg, wParam, lParam);
    }

    bool ConfigBool(const char* name, bool defaultVal) const {
        return strcmp("true", EngineConfig.Get("Window", name, defaultVal ? "true" : "false"))  == 0;
    }

    int32 ConfigInt(const char* name, int32 defaultVal) const {
        auto confValue = EngineConfig.Get("Window", name);
        return confValue ? atoi(confValue) : defaultVal;
    }

    const char* ConfigStr(const char* name, const char* defaultVal) const {
        return EngineConfig.Get("Window", name, defaultVal);
    }

    ////////////////////////////////////////////////////////////////////////// API layer
    bool Create() {
        //////////////////////////////////////////////////////////////////////////
        // Window Setup
        //////////////////////////////////////////////////////////////////////////
        DWORD style, styleEx;
        if (ConfigBool("Fullscreen", false)) {
            style = WS_POPUP;
            styleEx = WS_EX_APPWINDOW;
            ensure(false, "fullscreen not properly implemented");
        }
        else {
            if (ConfigBool("Resizable", false)) {
                style = WS_OVERLAPPEDWINDOW;
            }
            else {
                style = WS_POPUP | WS_BORDER | WS_CAPTION | WS_SYSMENU;
            }
            styleEx  = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
        }
        style |= WS_CLIPCHILDREN | WS_CLIPSIBLINGS;

        Hinstance = ::GetModuleHandle(NULL);
        ensure(Hinstance != HINSTANCE(0), "GetModuleHandle Failed");

        WNDCLASSEX wc;
        wc.cbSize = sizeof(WNDCLASSEX);
        wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
        wc.lpfnWndProc = (WNDPROC)StaticWndProc;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = Hinstance;
        wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        wc.hIconSm = NULL;
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = NULL;  // No brush - we are going to paint our own background
        wc.lpszMenuName = NULL;  // No default menu
        wc.lpszClassName = L"game";
        ensure(::RegisterClassEx(&wc) != 0, "RegisterClassEx failed.");

        HWND hwndDesktop = GetDesktopWindow();
        RECT desktop;
        GetWindowRect(hwndDesktop, &desktop);

        int32 
            x = desktop.right / 2,
            y = desktop.bottom / 2,
            w = ConfigInt("Width", 1280), 
            h = ConfigInt("Height", 720);
        x -= w / 2;
        y -= h / 2;
        // HACK, REMOVE ME
        // HACK, REMOVE ME
        // HACK, REMOVE ME
        // HACK, REMOVE ME
        // HACK, REMOVE ME
        // HACK, REMOVE ME
        // HACK, REMOVE ME
        // HACK, REMOVE ME
        x -= desktop.right;
        // End HACK


        RECT sysRect = { x, y, w, h };

        ensure(AdjustWindowRectEx(&sysRect, style, FALSE, styleEx), "AdjustWindowRectEx failed.");

        WString title = StringToWString(ConfigStr("Title", "Game"));
        Hwnd = CreateWindowEx(styleEx, L"game", title.c_str(), style, x, y, w, h, NULL, NULL, Hinstance, NULL);
        ensure(Hwnd != HWND(0), "CreateWindowEx failed.");
        
        Hdc = GetDC(Hwnd);
        ensure(Hdc != HDC(0), "GetDC failed.");

        // return value is not referring to success/failure. Don't use ensure here.
        // see: https://msdn.microsoft.com/en-us/library/windows/desktop/ms633548(v=vs.85).aspx
        ShowWindow(Hwnd, SW_SHOW);

        //////////////////////////////////////////////////////////////////////////
        // OpenGL setup
        //////////////////////////////////////////////////////////////////////////
        PIXELFORMATDESCRIPTOR pfd;
        memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
        pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
        pfd.nVersion = 1;
        pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
        pfd.iPixelType = PFD_TYPE_RGBA;
        pfd.cColorBits = 32;
        pfd.cDepthBits = 24;
        pfd.cStencilBits = 8;
        pfd.iLayerType = PFD_MAIN_PLANE;

        int32 pixelFormat = ChoosePixelFormat(Hdc, &pfd);
        ensure(pixelFormat != 0, "ChoosePixelFormat failed.");

        ensure(SetPixelFormat(Hdc, pixelFormat, &pfd), "SetPixelFormat failed.");

        HGLRC tempHglrc = wglCreateContext(Hdc);
        ensure(tempHglrc != HGLRC(0), "wglCreateContext failed.");

        ensure(wglMakeCurrent(Hdc, tempHglrc), "wglMakeCurrent failed.");

        ensure(glewInit() == GLEW_OK, "glewInit failed.");

        if (wglChoosePixelFormatARB && wglCreateContextAttribsARB)
        {
            int32 samples = ConfigInt("Samples", 0);
            int32 attribList[] = {
                WGL_SAMPLES_ARB, samples,
                WGL_SAMPLE_BUFFERS_ARB, samples > 0 ? 1 : 0,
                WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
                WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
                WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
                WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
                WGL_COLOR_BITS_ARB, pfd.cColorBits,
                WGL_DEPTH_BITS_ARB, pfd.cDepthBits,
                WGL_STENCIL_BITS_ARB, pfd.cStencilBits,
                0
            };
            bool multisampling = samples > 0;

            uint32 numFormats;
            if (!wglChoosePixelFormatARB(Hdc, attribList, NULL, 1, &pixelFormat, &numFormats) || numFormats == 0)
            {
                bool valid = false;
                if (samples > 0)
                {
                    xoLog("Failed to choose pixel format with WGL_SAMPLES_ARB == " << samples << ". Attempting to fallback to lower samples setting.");
                    while (samples > 0)
                    {
                        samples /= 2;
                        attribList[1] = samples;
                        attribList[3] = samples > 0 ? 1 : 0;
                        if (wglChoosePixelFormatARB(Hdc, attribList, NULL, 1, &pixelFormat, &numFormats) && numFormats > 0)
                        {
                            valid = true;
                            xoLog("Found pixel format with WGL_SAMPLES_ARB == " << samples);
                            break;
                        }
                    }

                    multisampling = samples > 0;
                }

                ensure(valid, "Failed to choose a pixel format.");
            }

            ensure(SetPixelFormat(Hdc, pixelFormat, &pfd), "SetPixelFormat failed.");

            int32 attribs[] =
            {
                WGL_CONTEXT_MAJOR_VERSION_ARB, ConfigInt("GLMajor", 3),
                WGL_CONTEXT_MINOR_VERSION_ARB, ConfigInt("GLMajor", 2),
                0
            };
            Hglrc = wglCreateContextAttribsARB(Hdc, 0, attribs);
            ensure(Hglrc != HGLRC(0), "wglCreateContextAttribsARB failed.");
            
            ensure(wglDeleteContext(tempHglrc), "wglDeleteContext failed.");

            ensure(wglMakeCurrent(Hdc, Hglrc), "wglMakeCurrent failed.");
        }
        else {
            xoLog("Falling back to OpenGL 2.0");
            Hglrc = tempHglrc;
        }

        if (wglSwapIntervalEXT) {
            wglSwapIntervalEXT(ConfigBool("Vsync", true) ? 0 : 1);
        }

        // Some old graphics cards support EXT_framebuffer_object instead of ARB_framebuffer_object.
        // Patch ARB_framebuffer_object functions to EXT_framebuffer_object ones since semantic is same.
        if (!GLEW_ARB_framebuffer_object && GLEW_EXT_framebuffer_object)
        {
            glBindFramebuffer = glBindFramebufferEXT;
            glBindRenderbuffer = glBindRenderbufferEXT;
            glBlitFramebuffer = glBlitFramebufferEXT;
            glCheckFramebufferStatus = glCheckFramebufferStatusEXT;
            glDeleteFramebuffers = glDeleteFramebuffersEXT;
            glDeleteRenderbuffers = glDeleteRenderbuffersEXT;
            glFramebufferRenderbuffer = glFramebufferRenderbufferEXT;
            glFramebufferTexture1D = glFramebufferTexture1DEXT;
            glFramebufferTexture2D = glFramebufferTexture2DEXT;
            glFramebufferTexture3D = glFramebufferTexture3DEXT;
            glFramebufferTextureLayer = glFramebufferTextureLayerEXT;
            glGenFramebuffers = glGenFramebuffersEXT;
            glGenRenderbuffers = glGenRenderbuffersEXT;
            glGenerateMipmap = glGenerateMipmapEXT;
            glGetFramebufferAttachmentParameteriv = glGetFramebufferAttachmentParameterivEXT;
            glGetRenderbufferParameteriv = glGetRenderbufferParameterivEXT;
            glIsFramebuffer = glIsFramebufferEXT;
            glIsRenderbuffer = glIsRenderbufferEXT;
            glRenderbufferStorage = glRenderbufferStorageEXT;
            glRenderbufferStorageMultisample = glRenderbufferStorageMultisampleEXT;
        }

        Owner.OnWindowCreated.Execute();
        return true;
    }

    void Close() {
        if (Hwnd != HWND(0)) {
            CloseWindow(Hwnd);
            Hwnd = HWND(0);
        }
        if (Hglrc != HGLRC(0)) {
            wglDeleteContext(Hglrc);
            Hglrc = HGLRC();
        }
        Owner.OnWindowClosed.Execute();
    }

    void SetTitle(String title) {
        WString wtitle = StringToWString(title);
        SetWindowText(Hwnd, wtitle.c_str());
    }

    void SetSize(uint32 width, uint32 height) {
        SetWindowPos(Hwnd, 0, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER);
    }

    void Update() {
        MSG msg;
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        SwapBuffers(Hdc);
    }
#endif
};

WindowImpl* WindowImpl::Self = nullptr;


Window::Window(Config& engineConfig)
{
    Impl = new WindowImpl(*this, engineConfig);
}

Window::~Window() {
    delete Impl;
}


bool Window::Create() {
    return Impl->Create();
}

void Window::Close() {
    Impl->Close();
}
void Window::SetTitle(String title) {
    Impl->SetTitle(title);
}

void Window::SetSize(uint32 width, uint32 height) {
    Impl->SetSize(width, height);
}

void Window::Update() {
    Impl->Update();
}

}