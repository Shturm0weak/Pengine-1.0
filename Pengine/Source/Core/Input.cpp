#include "Input.h"
#include "Window.h"

#include <algorithm>

using namespace Pengine;

void Input::SetupCallBack()
{
    GLFWwindow* window = Window::GetInstance().GetWindow();
    glfwSetKeyCallback(window, Input::KeyCallBack);
    glfwSetMouseButtonCallback(window, Input::MouseButtonCallBack);
}

bool Input::IsMouseDown(int button)
{
    return glfwGetMouseButton(Window::GetInstance().GetWindow(), button);
}

bool Input::IsMousePressed(int button)
{
    auto buttonIter = s_Keys.find(button);
    if (buttonIter != s_Keys.end())
    {
        if (buttonIter->second == 1) return true;
    }

    return false;
}

bool Input::IsMouseReleased(int button)
{
    auto buttonIter = s_Keys.find(button);
    if (buttonIter != s_Keys.end())
    {
        if (buttonIter->second == 0) return true;
    }

    return false;
}

bool Input::IsKeyDown(int keycode)
{
    return glfwGetKey(Window::GetInstance().GetWindow(), keycode);
}

bool Input::IsKeyPressed(int keycode)
{
    auto keyIter = s_Keys.find(keycode);
    if (keyIter != s_Keys.end())
    {
        if (keyIter->second == 1)
        {
            return true;
        }
    }

    return false;
}

bool Input::IsKeyReleased(int keycode)
{
    auto keyIter = s_Keys.find(keycode);
    if (keyIter != s_Keys.end())
    {
        if (keyIter->second == 0) return true;
    }

    return false;
}

void Input::ResetInput()
{
    for (auto key = s_Keys.begin(); key != s_Keys.end(); key++)
    {
        key->second = -1;
    }
}

void Input::KeyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    auto keyIter = s_Keys.find(key);
    if (keyIter != s_Keys.end())
    {
        keyIter->second = action;
    }
    else
    {
        s_Keys.insert(std::make_pair(key, action));
    }

    ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
}

void Input::MouseButtonCallBack(GLFWwindow* window, int button, int action, int mods)
{
    auto keyIter = s_Keys.find(button);
    if (keyIter != s_Keys.end())
    {
        keyIter->second = action;
    }
    else
    {
        s_Keys.insert(std::make_pair(button, action));
    }

    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
}