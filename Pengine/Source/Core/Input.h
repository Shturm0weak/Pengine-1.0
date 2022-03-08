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

        static bool IsMouseDown(int button);
        static bool IsMousePressed(int button);
        static bool IsMouseReleased(int button);
        static bool IsKeyDown(int keycode);
        static bool IsKeyPressed(int keycode);
        static bool IsKeyReleased(int keycode);

        static void SetupCallBack();
        static void ResetInput();
    private:

        static void KeyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void MouseButtonCallBack(GLFWwindow* window, int button, int action, int mods);

        static std::unordered_map<int, int> s_Keys; //first - keycode, second - action
    };

}