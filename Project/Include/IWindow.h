#pragma once

#include "Types.h"
#include "Subscription.h"

namespace XO
{
    class IWindow
    {
    public:
        virtual void Create(String title, uint32 width, uint32 height) = 0;
        virtual void Close() = 0;
        virtual void SetTitle(String title) = 0;
        virtual void SetSize(uint32 width, uint32 height) = 0;

        Subscription OnWindowCreated;
        Subscription OnWindowClosed;
    };
}