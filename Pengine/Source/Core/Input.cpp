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

bool Input::Mouse::IsMouseDown(int button)
{
    return glfwGetMouseButton(Window::GetInstance().GetWindow(), button);
}

bool Input::Mouse::IsMousePressed(int button)
{
    auto buttonIter = s_Keys.find(button);
    if (buttonIter != s_Keys.end())
    {
        if (buttonIter->second == 1) return true;
    }

    return false;
}

bool Input::Mouse::IsMouseReleased(int button)
{
    auto buttonIter = s_Keys.find(button);
    if (buttonIter != s_Keys.end())
    {
        if (buttonIter->second == 0) return true;
    }

    return false;
}

bool Input::KeyBoard::IsKeyDown(int keycode)
{
    return glfwGetKey(Window::GetInstance().GetWindow(), keycode);
}

bool Input::KeyBoard::IsKeyPressed(int keycode)
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

bool Input::KeyBoard::IsKeyReleased(int keycode)
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
    m_JoyStick.Update();

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
    auto buttonIter = s_Keys.find(button);
    if (buttonIter != s_Keys.end())
    {
        buttonIter->second = action;
    }
    else
    {
        s_Keys.insert(std::make_pair(button, action));
    }

    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
}

void Input::JoyStickInfo::Update()
{
    m_PreviuosButtons = m_Buttons;

    for (auto axis = m_Axes.begin(); axis != m_Axes.end(); axis++)
    {
        axis->second = -1;
    }

    for (auto button = m_Buttons.begin(); button != m_Buttons.end(); button++)
    {
        button->second = -1;
    }

    m_IsPresent = glfwJoystickPresent(m_ID);

    if (m_IsPresent)
    {
        int axesCount = 0;
        const float* axes = glfwGetJoystickAxes(m_ID, &axesCount);

        if (axes)
        {
            for (size_t i = 0; i < axesCount; i++)
            {
                auto axisIter = m_Axes.find(i);
                if (axisIter != m_Axes.end())
                {
                    axisIter->second = axes[i];
                }
                else
                {
                    m_Axes.insert(std::make_pair(i, axes[i]));
                }
            }
        }

        int buttonCount = 0;
        const unsigned char* buttons = glfwGetJoystickButtons(m_ID, &buttonCount);

        if (buttons)
        {
            for (size_t i = 0; i < buttonCount; i++)
            {
                auto buttonIter = m_Buttons.find(i);
                if (buttonIter != m_Buttons.end())
                {
                    buttonIter->second = buttons[i];
                }
                else
                {
                    m_Buttons.insert(std::make_pair(i, buttons[i]));
                }
            }
        }
    }
}

bool Input::JoyStick::IsButtonDown(int buttonCode)
{
    auto buttonIter = m_JoyStick.m_Buttons.find(buttonCode);
    if (buttonIter != m_JoyStick.m_Buttons.end())
    {
        if (buttonIter->second == 1)
        {
            return true;
        }
    }

    return false;
}

bool Input::JoyStick::IsButtonPressed(int buttonCode)
{
    auto buttonIter = m_JoyStick.m_Buttons.find(buttonCode);
    auto previousButtonIter = m_JoyStick.m_PreviuosButtons.find(buttonCode);
    if (buttonIter != m_JoyStick.m_Buttons.end())
    {
        if (buttonIter->second == 1 && previousButtonIter->second != 1)
        {
            return true;
        }
    }

    return false;
}

bool Input::JoyStick::IsButtonReleased(int buttonCode)
{
    auto buttonIter = m_JoyStick.m_Buttons.find(buttonCode);
    if (buttonIter != m_JoyStick.m_Buttons.end())
    {
        if (buttonIter->second == 0)
        {
            return true;
        }
    }

    return false;
}

float Input::JoyStick::GetAxis(int axisCode)
{
    auto axisIter = m_JoyStick.m_Axes.find(axisCode);
    if (axisIter != m_JoyStick.m_Axes.end())
    {
        return axisIter->second;
    }

    return 0.0f;
}
