#include <Include/Assets/AssetManager.h>
#include <Include/Log.h>
#include <Include/Renderer.h>
#include <Include/Window.h>

#include <iostream>

#if defined(_MSC_VER)
#define VISUAL_STUDIO_LEAK_DETECTION 1
#endif

int main(int argsc, char** argsv) {
#ifdef VISUAL_STUDIO_LEAK_DETECTION
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    //////////////////////////////////////////////////////////////////////////
    // Core system initializations.
    XO::AssetManager    assetManager;
    XO::LogManager      logManager(XO::AssetManager::EngineConfig("Paths", "EasyLoggingCppConfig", "./EasyLog.config"));
    XO::Window          window;
    XO::Renderer        renderer;

    //////////////////////////////////////////////////////////////////////////
    bool windowIsOpen = true;

    window.OnWindowCreated.Add([&renderer] ()
    {
        renderer.Init(); // we don't use raii for the renderer since it depends on the window creation.
        renderer.DebugLog();
        xoLog("Window created.");
    });

    window.OnWindowClosed.Add([&windowIsOpen] ()
    {
        windowIsOpen = false;
        xoLog("Window closed.");
    });

    if (window.Create())
    {
        xoLog("Carousel Application Starting.");
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