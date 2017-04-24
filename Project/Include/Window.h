#pragma once

#include <Include/Macros.h>
#include <Include/Types.h>

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

    class Subscription& OnWindowCreated();
    class Subscription& OnWindowClosed();

private:
    class WindowImpl* Impl;
};
}