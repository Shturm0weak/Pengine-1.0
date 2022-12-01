#include "Shader.h"

#include <fstream>
#include <sstream>
#include <glew.h>
#include <glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <glfw3native.h>

#include "../Core/Logger.h"

using namespace Pengine;

Shader::Shader(const std::string& name, const std::string& filepath) : m_Name(name),m_FilePath(filepath), m_RendererID(0)
{
	ShaderProgramSource source = Parseshader(filepath);
	m_RendererID = CreateShader(source.m_VertexSource, source.m_GeometrySource, source.m_FragmentSource);
	Logger::Success("has been loaded!", "Shader", name.c_str());
}

Shader::~Shader()
{
	glDeleteProgram(m_RendererID);
	auto iter = s_Shaders.find(this->m_Name);
	if(iter != s_Shaders.end())
		s_Shaders.erase(iter);
}

Shader* Shader::Create(const std::string& name, const std::string& path)
{
	auto iter = s_Shaders.find(name);
	if (iter == s_Shaders.end()) 
	{
		s_Shaders.insert(std::make_pair(name, new Shader(name, path)));
		return Get(name);
	}
	else 
	{
#ifdef _DEBUG
		Logger::Warning("has been already existed!", "Shader", name.c_str());
#endif
		return iter->second;
	}
}

Shader * Shader::Get(const std::string& name, bool showErrors)
{
	auto iter = s_Shaders.find(name);
	if (iter != s_Shaders.end()) return iter->second;
	else if (showErrors)
		Logger::Warning("doesn't exist!", "Shader", name.c_str());
	return nullptr;
}

void Shader::Bind() const 
{
	glUseProgram(m_RendererID);
}

void Shader::UnBind() const 
{
	glUseProgram(0);
}

void Shader::SetUniform1i(const std::string& name, int value)
{
	glUniform1i(GetUniformLocation(name), value);
}

void Shader::SetUniform1iv(const std::string& name, int* value, size_t count)
{
	glUniform1iv(GetUniformLocation(name), count, value);
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3) 
{
	glUniform4f(GetUniformLocation(name), v0, v1, v2, v3);
}

void Shader::SetUniform3f(const std::string& name, float v0, float v1, float v2)
{
	glUniform3f(GetUniformLocation(name), v0, v1, v2);
}

void Shader::SetUniform2f(const std::string& name, float v0, float v1)
{
	glUniform2f(GetUniformLocation(name), v0, v1);
}

void Shader::SetUniformfv(const std::string& name, const std::vector<float> values)
{
	glUniform1fv(GetUniformLocation(name), values.size(), &values[0]);
}

void Shader::SetUniform4fv(const std::string& name, const glm::vec4& vec4) 
{
	float vec[4] = { vec4[0], vec4[1], vec4[2], vec4[3] };
	glUniform4fv(GetUniformLocation(name), 1, vec);
}

void Shader::SetUniform3fv(const std::string& name, const glm::vec3 & vec3)
{
	float vec[3] = { vec3[0], vec3[1], vec3[2]};
	glUniform3fv(GetUniformLocation(name), 1, vec);
}

void Shader::SetUniformMat4f(const std::string& name, const glm::mat4& matrix) 
{
	glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]);
}

void Shader::SetUniformMat3f(const std::string& name, const glm::mat3& matrix)
{
	glUniformMatrix3fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]);
}

void Shader::SetUniformMat4fv(const std::string& name, const std::vector<glm::mat4>& matrices)
{
	if (!matrices.empty())
	{
		glUniformMatrix4fv(GetUniformLocation(name), matrices.size(), GL_FALSE, &matrices[0][0][0]);
	}
}

uint32_t Shader::GetUniformBlockIndex(const char* name)
{
	return glGetUniformBlockIndex(m_RendererID, name);
}

std::vector<uint32_t> Shader::GetUniformIndices(uint32_t size, const char** names)
{
	std::vector<uint32_t> indices(size);
	glGetUniformIndices(m_RendererID, size, names, &indices[0]);
	return indices;
}

std::vector<int> Shader::GetUniformsOffset(uint32_t size, const char** names)
{
	std::vector<int> offsets(size);
	std::vector<uint32_t> indices = GetUniformIndices(size, names);
	glGetActiveUniformsiv(m_RendererID, size, &indices[0], GL_UNIFORM_OFFSET, &offsets[0]);
	return offsets;
}

int Shader::GetUniformBlockSize(const char* name)
{
	uint32_t index = GetUniformBlockIndex(name);
	int size = 0;
	if (index != GL_INVALID_INDEX)
	{
		glGetActiveUniformBlockiv(m_RendererID, index, GL_UNIFORM_BLOCK_DATA_SIZE, &size);
	}
	return size;
}

void Shader::Reload()
{
	glDeleteProgram(m_RendererID);
	ShaderProgramSource source = Parseshader(m_FilePath);
	m_RendererID = CreateShader(source.m_VertexSource, source.m_GeometrySource, source.m_FragmentSource);
	Logger::Success("has been updated!", "Shader", m_Name.c_str());
}

void Shader::SetUniform2fv(const std::string& name, const glm::vec2& vec2)
{
	float vec[2] = { vec2[0],vec2[1] };
	glUniform2fv(GetUniformLocation(name),1,vec);
}

void Shader::SetUniform1f(const std::string& name, float v0) 
{
	glUniform1fv(GetUniformLocation(name), 1, &v0);
}

int Shader::GetUniformLocation(const std::string& name) 
{
	if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
		return m_UniformLocationCache[name];
	int location = glGetUniformLocation(m_RendererID, name.c_str());
	if (location == -1)
		Logger::Warning("doesn't exist!", "Uniform", (m_Name + "::" + name).c_str());
	m_UniformLocationCache[name] = location;
	return location;
}

ShaderProgramSource Shader::Parseshader(const std::string& filepath)
{
	std::ifstream stream(filepath);

	enum class shadertype {

		NONE = -1, VERTEX = 0, GEOMETRY = 1, FRAGMENT = 2

	};

	shadertype type = shadertype::NONE;
	std::string line;
	std::stringstream ss[3];
	while (getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
			{
				type = shadertype::VERTEX;
			}
			else if (line.find("geometry") != std::string::npos)
			{
				type = shadertype::GEOMETRY;
			}
			else if (line.find("fragment") != std::string::npos)
			{
				type = shadertype::FRAGMENT;
			}
		}
		else
		{
			ss[(int)type] << line << '\n';
		}
	}
	return { ss[0].str(), ss[1].str(), ss[2].str() };
}

uint32_t Shader::CompileShader(uint32_t type, const std::string& source) 
{
	uint32_t id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = new char[length * sizeof(char)];
		glGetShaderInfoLog(id, length, &length, message);
		const char* typeOfShader = type == GL_VERTEX_SHADER ? "Vertex" : "Fragment";
		Logger::Error("failed to compile!", "Shader", std::string(typeOfShader + m_Name).c_str());
		printf_s(message);
		delete[] message;
		glDeleteShader(id);
		return 0;
	}

	return id;
}

uint32_t Shader::CreateShader(
	const std::string& vertexShader, 
	const std::string& geometryShader,
	const std::string& FragmentShader)
{
	uint32_t programm = glCreateProgram();

	uint32_t vs;
	uint32_t gs;
	uint32_t fs;

	if (!vertexShader.empty())
	{
		vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
		glAttachShader(programm, vs);
		glDeleteShader(vs);
	}

	if (!geometryShader.empty())
	{
		gs = CompileShader(GL_GEOMETRY_SHADER, geometryShader);
		glAttachShader(programm, gs);
		glDeleteShader(gs);
	}

	if (!FragmentShader.empty())
	{
		fs = CompileShader(GL_FRAGMENT_SHADER, FragmentShader);
		glAttachShader(programm, fs);
		glDeleteShader(fs);
	}

	glLinkProgram(programm);
	glValidateProgram(programm);

	return programm;
}