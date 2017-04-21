#pragma once

#include "Types.h"
#include "Subscription.h"
#include "Config.h"

namespace XO
{
    class Window
    {
    public:
        Window(Config& engineConfig);
        ~Window();

        bool Create();
        void Close();
        void SetTitle(String title);
        void SetSize(uint32 width, uint32 height);

        void Update();

        Subscription OnWindowCreated;
        Subscription OnWindowClosed;

    private:
        class WindowImpl* Impl;
    };
}