#pragma once

#include "Core.h"
#include "glew.h"
#include "glfw3.h"

#include "../Enums/KeyCode.h"

#include <unordered_map>

namespace Pengine
{

    class PENGINE_API Input 
    {
    public:

        struct PENGINE_API Mouse
        {
            static bool IsMouseDown(int button);
            
            static bool IsMousePressed(int button);
            
            static bool IsMouseReleased(int button);
        };

        struct PENGINE_API KeyBoard
        {
            static bool IsKeyDown(int keycode);
            
            static bool IsKeyPressed(int keycode);
            
            static bool IsKeyReleased(int keycode);
        };

        struct PENGINE_API JoyStick
        {
            static bool IsButtonDown(int buttonCode);
            
            static bool IsButtonPressed(int buttonCode);
            
            static bool IsButtonReleased(int buttonCode);
            
            static float GetAxis(int axisCode);
        };

        static void SetupCallBack();
        
        static void ResetInput();
    private:

        static void KeyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods);
        
        static void MouseButtonCallBack(GLFWwindow* window, int button, int action, int mods);

        static std::unordered_map<int, int> s_Keys; // First - keycode, second - action.

        struct JoyStickInfo
        {
            int m_ID = GLFW_JOYSTICK_1;
            int m_IsPresent = 0;

            std::unordered_map<int, float> m_Axes; // First - axis, second - value.
            std::unordered_map<int, int> m_Buttons; // First - keycode, second - action.
            std::unordered_map<int, int> m_PreviuosButtons; // First - keycode, second - action.

            void Update();
        } static m_JoyStick;
    };

}