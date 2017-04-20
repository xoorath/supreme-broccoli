#pragma once
#if defined(GAME_WINDOWS)

#include "IWindow.h"
#include "Macros.h"

namespace XO
{
    class Window : public IWindow
    {
    public:
        virtual void Create(String title, uint32 width, uint32 height) override
        {
            TODO_IMPLEMENT();
            OnWindowCreated.Execute();
        }
        virtual void Close() override
        {
            TODO_IMPLEMENT();
            OnWindowClosed.Execute();
        }
        virtual void SetTitle(String title) override
        {
            TODO_IMPLEMENT();
        }
        virtual void SetSize(uint32 width, uint32 height) override
        {
            TODO_IMPLEMENT();
        }
    };
}

#endif