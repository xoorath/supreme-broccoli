#include <Include/Input.h>

#include <Include/Window.h>

#include <ThirdParty/xo-math/xo-math.h>

namespace XO {

struct MouseSubscriptionDetails {
    uint32 id;
    InputPriorities priority;
    MouseSubscriptionType subscriptionType;
    Input::ShouldHandleMouseLambda shouldHandleMouse;
    Input::HandleMouseLambda handleMouse;
    Input::EndHandleMouseLambda endHandleMouse;
};

struct KeySubscriptionDetails {
    uint32 id;
    InputPriorities priority;
    KeySubscriptionType subscriptionType;
    Input::ShouldHandleKeyLambda shouldHandleKey;
    Input::HandleKeyLambda handleKey;
    Input::EndHandleKeyLambda endHandleKey;
};

_XOSIMDALIGN class InputImpl {
private:
    static uint32 IncrementingId;
    // subscriptions
    Array<MouseSubscriptionDetails*> MouseSubscriptions;
    Array<KeySubscriptionDetails*> KeySubscriptions;

    // mouse state
    int32 MouseButtonState = 0;
    Vector2 MousePosition = Vector2::Zero;

    // mouse drag state
    MouseSubscriptionDetails* CurrentSubscription = nullptr;
    Vector2 DragStartPos = Vector2::Zero;
    bool IsReadyToStartDrag = false;
    bool IsDragging = false;

    // key state
    bool KeysDown[(uint32)KeyboardKey::COUNT] = {0};

    // shared state
    float32 dt = 0.01f;

public:
    _XO_OVERLOAD_NEW_DELETE();


    ~InputImpl() {
        for (auto sub : MouseSubscriptions) {
            delete sub;
        }
    }

    void NotifyMouseMove(float32 x, float32 y) {
        Vector2 lastPosition = MousePosition;
        MousePosition.Set(x, y);
        Vector2 positionDelta = lastPosition - MousePosition;

        if (IsReadyToStartDrag) {
            for (auto sub : MouseSubscriptions) {
                if (sub->subscriptionType == MouseSubscriptionType::MouseDrag) {
                    InputConsumptionType shouldHandle = sub->shouldHandleMouse(MousePosition);
                    switch (shouldHandle) {
                        case InputConsumptionType::Consume:
                            IsDragging = true;
                            DragStartPos = lastPosition;
                            sub->handleMouse(DragStartPos, MousePosition, positionDelta, dt);
                            CurrentSubscription = sub;
                            break;
                        case InputConsumptionType::DontConsume:
                            break;
                    }
                }
                if (IsDragging) {
                    IsReadyToStartDrag = false;
                    break;
                }
            }
        }
        else if (IsDragging) {
            CurrentSubscription->handleMouse(DragStartPos, MousePosition, positionDelta, dt);
        }
        else {

        }
    }

    void NotifyMouseDown(MouseButton button) {
        MouseButtonState |= (int32)button;

        if (button == MouseButton::Left) {
            // first try to handle a click.
            bool clickHandled = false;
            for (auto sub : MouseSubscriptions) {
                if (sub->subscriptionType == MouseSubscriptionType::MouseClick) {
                    InputConsumptionType shouldHandle = sub->shouldHandleMouse(MousePosition);
                    switch (shouldHandle) {
                        case InputConsumptionType::Consume:
                            sub->handleMouse(MousePosition, MousePosition, Vector2::Zero, 0.01f);
                            clickHandled = true;
                            break;
                    }
                }
                if (clickHandled) {
                    break;
                }
            }

            if (!clickHandled) {
                IsReadyToStartDrag = true;
            }
        }
    }

    void NotifyMouseUp(MouseButton button) {
        MouseButtonState &= ~(int32)button;
        
        if (button == MouseButton::Left) {
            if (IsDragging) {
                CurrentSubscription->endHandleMouse();
                CurrentSubscription = nullptr;
                IsDragging = false;
            }
        }
    }

    void NotifyKeyDown(KeyboardKey key) {
        bool wasDown = KeysDown[(uint32)key];
        KeysDown[(uint32)key] = true;
        if (!wasDown) {
            for (auto sub : KeySubscriptions) {
                bool handled = false;
                InputConsumptionType shouldhandle = sub->shouldHandleKey(key);
                switch (shouldhandle) {
                    case InputConsumptionType::Consume:
                        sub->handleKey(key);
                        sub->endHandleKey();
                        handled = true;
                        break;
                }
                if (handled) {
                    break;
                }
            }
        }
    }

    void NotifyKeyUp(KeyboardKey key) {
        KeysDown[(uint32)key] = false;
    }

    uint32 SubscribeToMouseEvent(InputPriorities priority,
                                 MouseSubscriptionType subscriptionType,
                                 Input::ShouldHandleMouseLambda shouldHandleMouse,
                                 Input::HandleMouseLambda handleMouse,
                                 Input::EndHandleMouseLambda endHandleMouse) {

        MouseSubscriptionDetails* details = new MouseSubscriptionDetails();
        details->id = IncrementingId++;
        details->priority = priority;
        details->subscriptionType = subscriptionType;
        details->shouldHandleMouse = shouldHandleMouse;
        details->handleMouse = handleMouse;
        details->endHandleMouse = endHandleMouse;
        // TODO: insert at priority, not at end with a sort.
        MouseSubscriptions.push_back(details);
        std::sort(MouseSubscriptions.begin(), MouseSubscriptions.end(), [](MouseSubscriptionDetails* a, MouseSubscriptionDetails* b) {
            return a->priority < b->priority;
        });
        return details->id;
    }

    void UnsubscribeFromMouseEvent(uint32 id) {
        xoFatal("Unsupported.");
    }

    uint32 SubscribeToKeyEvent(InputPriorities priority,
                               KeySubscriptionType subscriptionType,
                               Input::ShouldHandleKeyLambda shouldHandleKey,
                               Input::HandleKeyLambda handleKey,
                               Input::EndHandleKeyLambda endHandleKey) {
        KeySubscriptionDetails* details = new KeySubscriptionDetails();
        details->id = IncrementingId++;
        details->priority = priority;
        details->subscriptionType = subscriptionType;
        details->shouldHandleKey = shouldHandleKey;
        details->handleKey = handleKey;
        details->endHandleKey = endHandleKey;
        
        KeySubscriptions.push_back(details);
        std::sort(KeySubscriptions.begin(), KeySubscriptions.end(), [](KeySubscriptionDetails* a, KeySubscriptionDetails* b) {
            return a->priority < b->priority;
        });
        return details->id;
    }

    void UnsubscribeFromKeyEvent(uint32 id) {
        xoFatal("Unsupported.");
    }
};

uint32 InputImpl::IncrementingId = 0;
Input* Input::InputInstance = nullptr;

/*static*/Input& Input::Get() {
    return *InputInstance;
}

uint32 Input::SubscribeToMouseEvent(InputPriorities priority,
                                    MouseSubscriptionType subscriptionType,
                                    ShouldHandleMouseLambda shouldHandleMouse,
                                    HandleMouseLambda handleMouse,
                                    EndHandleMouseLambda endHandleMouse) {
    return Impl->SubscribeToMouseEvent(priority, subscriptionType, shouldHandleMouse, handleMouse, endHandleMouse);
}

void Input::UnsubscribeFromMouseEvent(uint32 id) {

}

uint32 Input::SubscribeToKeyEvent(InputPriorities priority,
                                  KeySubscriptionType subscriptionType,
                                  ShouldHandleKeyLambda shouldHandleKey,
                                  HandleKeyLambda handleKey,
                                  EndHandleKeyLambda endHandleKey) {
    return Impl->SubscribeToKeyEvent(priority, subscriptionType, shouldHandleKey, handleKey, endHandleKey);
}

void Input::UnsubscribeFromKeyEvent(uint32 id) {

}
////////////////////////////////////////////////////////////////////////// WindowImpl API
Input::Input() {
    InputInstance = this;
    Impl = new InputImpl();
}

Input::~Input() {
    delete Impl;
}

void Input::NotifyMouseMove(float32 x, float32 y) {
    Impl->NotifyMouseMove(x, y);
}

void Input::NotifyMouseDown(MouseButton button) {
    Impl->NotifyMouseDown(button);
}

void Input::NotifyMouseUp(MouseButton button) {
    Impl->NotifyMouseUp(button);
}

void Input::NotifyKeyDown(KeyboardKey key) {
    Impl->NotifyKeyDown(key);
}

void Input::NotifyKeyUp(KeyboardKey key) {
    Impl->NotifyKeyUp(key);
}
////////////////////////////////////////////////////////////////////////// End WindowImpl API
}