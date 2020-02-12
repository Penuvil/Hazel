#pragma once

#ifdef HZ_PLATFORM_ANDROID
#include "Hazel/Core/Input.h"

namespace Hazel {

    class AndroidInput : public Input
    {
    protected:
        virtual bool IsKeyPressedImpl(KeyCode key) override;

        virtual bool IsMouseButtonPressedImpl(MouseCode button) override;
        virtual std::pair<float, float> GetMousePositionImpl() override;
        virtual float GetMouseXImpl() override;
        virtual float GetMouseYImpl() override;
    };

}
#endif