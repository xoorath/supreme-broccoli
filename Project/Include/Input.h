#pragma once

#include <Include/Types.h>

namespace XO {

// Acts as a bitmask internally.
enum class MouseButton : int32 {
    Left = 1,
    Middle = 2,
    Right = 4
};

// Non exhaustive, for this demo.
enum class KeyboardKey : uint32 {
    NONE = 0,
    
    Escape,
    Enter,
    Up,
    Down,
    Left,
    Right,

    COUNT
};

enum class MouseSubscriptionType {
    MouseDrag,
    MouseClick,
    MouseMove
};

enum class KeySubscriptionType {
    Press,
    // Unsupported for this demo:
    // Hold,
    // Release
};

enum class InputConsumptionType {
    // Handle this input. Don't let lower priorities handle this input.
    Consume,
    // Don't handle this input at all. Pass on to lower priority subscriptions.
    DontConsume,
    
    // Update, but don't consider it consumed (pass onto lower priority subscriptions)
    // Note: not supported at this time.
    //UseButDontConsume,
    
    // Don't allow anyone to handle this input, not even this subscription.
    // Note: not supported at this time.
    //ConsumeButDontUse
};

enum class InputPriorities : int32 {
    InGame = 0,
    InGameUI = 100,
    InGamePopup = 200,
    InGameMenu = 300,
    InGameMenuPopup = 400,
    MainMenu = 1000,
    MainMenuPopup = 1100,
    CriticalPopup = 10000,

    RunAndGun = InGame,
    WeaponSelect = InGameUI,

    Carousel_Navigation = InGameMenu+0,
    Carousel_Orbit = Carousel_Navigation + 1,
    CarouselNode_Click = Carousel_Orbit + 1
};


class Input {
public:
    typedef Lambda<InputConsumptionType(const class Vector2&/*pos*/)> ShouldHandleMouseLambda;
    typedef Lambda<void(const class Vector2&/*start*/, const class Vector2&/*current*/, const class Vector2&/*delta pos*/, float dt/*delta time*/)> HandleMouseLambda;
    typedef Lambda0 EndHandleMouseLambda;

    typedef Lambda<InputConsumptionType(KeyboardKey key)> ShouldHandleKeyLambda;
    typedef Lambda<void(KeyboardKey key)> HandleKeyLambda;
    typedef Lambda0 EndHandleKeyLambda;

    static Input& Get();
    
    // Priority: 
    //      subscriptions with a higher priority will be processed first.
    // subscriptionType:
    //      What type of mouse event is this subscription  tracking.
    // shouldHandleMouse:
    //      A function that returns an InputConsumptionType, indicating if the input is consumed by this subscription or not.
    // handleMouse:
    //      An update function called each frame the event is fired on.
    // EndHandleMouseLambda:
    //      A function fired at the end of the event.
    // Returns:
    //      A subscription id you can use to unsubscribe.
    uint32 SubscribeToMouseEvent(InputPriorities priority,
                                 MouseSubscriptionType subscriptionType, 
                                 ShouldHandleMouseLambda shouldHandleMouse, 
                                 HandleMouseLambda handleMouse, 
                                 EndHandleMouseLambda endHandleMouse);

    void UnsubscribeFromMouseEvent(uint32 id);

    uint32 SubscribeToKeyEvent(InputPriorities priority,
                               KeySubscriptionType subscriptionType,
                               ShouldHandleKeyLambda shouldHandleKey,
                               HandleKeyLambda handleKey,
                               EndHandleKeyLambda endHandleKey);

    void UnsubscribeFromKeyEvent(uint32 id);

private:
    friend class WindowImpl;
    // WindowImpl API
    Input();
    ~Input();

    void NotifyMouseMove(float32 x, float32 y);
    void NotifyMouseDown(MouseButton button);
    void NotifyMouseUp(MouseButton button);

    void NotifyKeyDown(KeyboardKey key);
    void NotifyKeyUp(KeyboardKey key);
    // End WindowImpl

    static Input* InputInstance;
    class InputImpl* Impl;
};

}