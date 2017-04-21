#pragma once

#include <Include/Types.h>
#include <Include/Subscription.h>

namespace XO
{
    class Window
    {
    public:
        Window();
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