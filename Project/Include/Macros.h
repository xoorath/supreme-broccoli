#pragma once

// Breaks the first time this is hit.
#define TODO_IMPLEMENT() \
    { \
        static bool onceOnly = true; \
        if(onceOnly) { \
            __debugbreak(); \
            onceOnly = false; \
        } \
    }