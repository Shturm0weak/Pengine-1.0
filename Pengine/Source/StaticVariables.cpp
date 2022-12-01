#include "OpenGL/Shader.h"
#include "Core/Input.h"
#include "Core/Logger.h"
#include "Core/EntryPoint.h"
#include "Core/Visualizer.h"
#include "Core/UUID.h"
#include "Core/Timer.h"
#include "Core/Editor.h"

#include <functional>

using namespace Pengine;

// Shader.
std::unordered_map<std::string, Shader*> Shader::s_Shaders;
const char** Shader::s_NamesOfShaders;

// Input.
std::unordered_map<int, int> Input::s_Keys;
Input::JoyStickInfo Input::m_JoyStick;

// Logger.
std::tm* Logger::s_CurrentTime;
std::string Logger::s_TimeString;

// EntryPoint.
Application* EntryPoint::m_Application = nullptr;

// Visualizer.
std::vector<Visualizer::LineParams> Visualizer::m_Lines;
std::vector<Visualizer::QuadParams> Visualizer::m_Quads;
std::vector<Visualizer::CircleParams> Visualizer::m_Circles;

// UUID.
std::vector<std::string> Pengine::UUID::s_UUIDs;

// Timer.
std::vector<std::pair<std::function<void()>, float>> Timer::s_Callbacks;

// Editor.
size_t Editor::Stats::s_AllocationsCount = 0;

// Not a good place to put this code, but it works :)
#include <new>

void* operator new(size_t size)
{
    Editor::Stats::s_AllocationsCount++;

    if (size == 0)
    {
        size++;
    }

    if (void* ptr = std::malloc(size)) return ptr;

    throw std::bad_alloc{};
}