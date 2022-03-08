#include "OpenGL/Shader.h"
#include "Core/Input.h"
#include "Core/Logger.h"
#include "Core/EntryPoint.h"
#include "Core/Visualizer.h"
#include "Core/UUID.h"
#include "Components/Renderer2D.h"

using namespace Pengine;

// Shader.
std::unordered_map<std::string, Shader*> Shader::s_Shaders;
const char** Shader::s_NamesOfShaders;

// Input.
std::unordered_map<int, int> Input::s_Keys;

// Logger.
std::tm* Logger::s_CurrentTime;
std::string Logger::s_TimeString;

// EntryPoint.
Application* EntryPoint::m_Application = nullptr;

// Visualizer.
std::vector<Visualizer::LineParams> Visualizer::m_Lines;
std::vector<Visualizer::QuadParams> Visualizer::m_Quads;

// UUID.
std::vector<size_t> Pengine::UUID::s_UUIDs;