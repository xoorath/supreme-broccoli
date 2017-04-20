#include "../Include/Window_Windows.h"
#include "../ThirdParty/ini/ini.h"
#include <iostream>

int main(int argsc, char** argsv)
{
    ini_t *engineSettings = ini_load("./Engine.ini");

    const char* assetsPath = ini_get(engineSettings, "paths", "Assets");

    XO::Window window;
    window.OnWindowCreated.Add([] ()
    {
        std::cout << "Window created!";
    });

    window.OnWindowClosed.Add([] ()
    {
        std::cout << "Window closed!";
    });


    window.Create("aw yeah", 1280, 720);
    window.Create("aw yeah", 1280, 720);
    return 0;
}