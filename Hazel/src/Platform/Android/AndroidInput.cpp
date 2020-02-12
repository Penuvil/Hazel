#include "hzpch.h"

#ifdef HZ_PLATFORM_ANDROID
#include "Platform/Android/AndroidInput.h"

namespace Hazel {
    bool AndroidInput::IsKeyPressedImpl(KeyCode key) 
    {
        return false;
    }

    bool AndroidInput::IsMouseButtonPressedImpl(MouseCode button)
    {
        return false;
    } 

    std::pair<float, float> AndroidInput::GetMousePositionImpl() 
    {
        return {0.0f, 0.0f};
    }

    float AndroidInput::GetMouseXImpl()
    {
        return 0.0f;
    } 

    float AndroidInput::GetMouseYImpl()
    {
        return 0.0f;
    }
}
#endif