#pragma once

#include <Include/Types.h>

namespace XO
{
class Window
{
public:
    Window();
    ~Window();

    static Window& Get();

    bool Create();
    void Close();
    void SetTitle(String title);
    void SetSize(uint32 width, uint32 height);

    void Update();

    class Subscription& OnWindowCreated();
    class Subscription& OnWindowClosed();

private:
    static Window* WindowInstance;
    class WindowImpl* Impl;
};
}