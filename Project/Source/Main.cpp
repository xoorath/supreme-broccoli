#include "../Include/Config.h"
#include "../Include/Log.h"
#include "../Include/Renderer.h"
#include "../Include/Shader.h"
#include "../Include/Window.h"
#include <iostream>

#if defined(_MSC_VER)
#define VISUAL_STUDIO_LEAK_DETECTION 1
#endif

int main(int argsc, char** argsv)
{
#ifdef VISUAL_STUDIO_LEAK_DETECTION
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    XO::Config engineConfig("./Engine.ini");
    XO::LogManager logManager(engineConfig.Get("Paths", "EasyLoggingCppConfig", "./EasyLog.config"));

    XO::ShaderProgram::Prepare(engineConfig);

    XO::Window window(engineConfig);
    XO::Renderer renderer;
    bool windowIsOpen = true;

    window.OnWindowCreated.Add([] ()
    {
        xoLog("Window opened.");
    });

    window.OnWindowClosed.Add([&windowIsOpen] ()
    {
        windowIsOpen = false;
        xoLog("Window closed.");
    });

    if (window.Create())
    {

        xoLog("Carousel Application Starting.");
        renderer.Init();
        renderer.DebugLog();

        while (windowIsOpen) {
            renderer.Render();
            window.Update();
        }
    }
    else
    {
        xoLog("Carousel Application unable to start.");
    }

    xoLog("Carousel Application Shutting Down.");
    return 0;
}