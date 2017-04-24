#include <Include/Assets/AssetManager.h>
#include <Include/Log.h>
#include <Include/Renderer.h>
#include <Include/Subscription.h>
#include <Include/Window.h>
#include <ThirdParty/xo-math/xo-math.h>

#include <Include/Game/GameScene.h>

#include <sstream>

#if defined(_MSC_VER)
#define VISUAL_STUDIO_LEAK_DETECTION 1
#endif

int main(int argsc, char** argsv) {
#ifdef VISUAL_STUDIO_LEAK_DETECTION
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    //////////////////////////////////////////////////////////////////////////
    // Core systems
    XO::AssetManager    assetManager;
    XO::LogManager      logManager(XO::AssetManager::EngineConfig("Paths", "EasyLoggingCppConfig", "./EasyLog.config"));
    XO::Window          window;
    XO::Renderer        renderer;

    //////////////////////////////////////////////////////////////////////////
    // Gameplay
    XO::GameScene game;

    //////////////////////////////////////////////////////////////////////////
    // xo-math goodies
#if defined(XO_SSE)
    XO::sse::ThrowNoExceptions();
    xoLog(XO_MATH_COMPILER_INFO);
    std::stringstream ss;
    ss << "\n================================================================ MXCSR\n";
    XO::sse::GetAllMXCSRInfo(ss);
    ss << "================================================================";
    xoLog(ss.str());
#endif

    //////////////////////////////////////////////////////////////////////////
    bool windowIsOpen = true;

    window.OnWindowCreated().Add([&renderer, &game]() {
        xoLog("Window created.");
        renderer.DebugLog();
        renderer.Init(); // we don't use raii for the renderer since it depends on the window creation.
        game.Init(&renderer);
    });

    window.OnWindowClosed().Add([&windowIsOpen]() {
        windowIsOpen = false;
        xoLog("Window closed.");
    });

    if (window.Create()) {
        xoLog("Carousel Application Starting.");
        while (windowIsOpen) {
            renderer.Render();
            window.Update();
            game.Tick();
        }
    }
    else {
        xoLog("Carousel Application unable to start.");
    }

    xoLog("Carousel Application Shutting Down.");
    return 0;
}