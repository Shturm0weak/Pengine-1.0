#pragma once

#include "Core.h"
#include "TextureManager.h"
#include "../OpenGL/Shader.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/matrix_decompose.hpp"

#include "UUID.h"
#include "Component.h"

#include <filesystem>
#include <map>
#include <vector>
#include <unordered_map>
#include <algorithm>

namespace fs = std::filesystem;

namespace Pengine
{

	namespace Utils
	{
		/*template<class T>
		PENGINE_API inline T Find(std::unordered_map<UUID, T*> map, T* value)
		{
			auto mapIter = std::find_if(map.begin(), map.end(), [value](const auto& second) {return second.m_FilePath == value; });
			if (mapIter != map.end())
			{
				return mapIter.second;
			}
			else
			{
				return nullptr;
			}
		}*/

		PENGINE_API inline glm::vec2 Direction(const glm::vec2& a, const glm::vec2& b)
		{
			return glm::normalize(glm::vec2(b.x - a.x, b.y - a.y));
		}

		PENGINE_API inline float GetLength(const glm::vec2& a)
		{
			return glm::sqrt(a.x * a.x + a.y * a.y);
		}

		PENGINE_API inline float GetSquaredLength(const glm::vec2& a)
		{
			return (a.x * a.x + a.y * a.y);
		}

		PENGINE_API inline float Epsilonf()
		{
			return 0.000001f;
		}

		PENGINE_API inline bool IntersectTriangle(glm::vec3 start, glm::vec3 dir, float length, glm::vec3& a, glm::vec3& b, glm::vec3& c, glm::vec3& planeNorm, glm::vec3& position)
		{
			glm::vec3 end = dir * length;
			glm::vec3 rayDelta = end - start;
			glm::vec3 rayToPlaneDelta = a - start;
			float ratio = glm::dot(rayToPlaneDelta, planeNorm);
			glm::vec3 proj = planeNorm * ratio;
			float vp = glm::dot(rayDelta, planeNorm);
			if (vp >= -0.0001 && vp <= 0.0001)
			{
				return false;
			}
			float wp = glm::dot(rayToPlaneDelta, planeNorm);
			float t = wp / vp;
			glm::vec3 iPos = rayDelta * t + start;
			position = iPos;
			glm::vec3 edge0 = b - a;
			glm::vec3 edge1 = c - b;
			glm::vec3 edge2 = a - c;
			glm::vec3 c0 = iPos - a;
			glm::vec3 c1 = iPos - b;
			glm::vec3 c2 = iPos - c;

			if (glm::dot(planeNorm, glm::cross(edge0, c0)) > 0 &&
				glm::dot(planeNorm, glm::cross(edge1, c1)) > 0 &&
				glm::dot(planeNorm, glm::cross(edge2, c2)) > 0) return true;
			else return false;
		}

		PENGINE_API inline std::vector<float> StandartUV()
		{
			return { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f };
		}

		PENGINE_API inline bool IsEqual(float a, float b)
		{
			return glm::abs(a - b) < glm::epsilon<float>();
		}

		PENGINE_API inline bool IsEqual(const glm::vec2& a, const glm::vec2& b)
		{
			return Utils::IsEqual(a.x, b.x) && Utils::IsEqual(a.y, b.y);
		}

		PENGINE_API inline bool IsEqual(const glm::vec3& a, const glm::vec3& b)
		{
			return Utils::IsEqual(a.x, b.x) && Utils::IsEqual(a.y, b.y) && Utils::IsEqual(a.z, b.z);
		}

		PENGINE_API inline bool IsPointInsideRect(const glm::vec2& point, const float rect[8])
		{
			return (point.x > rect[0] && point.x < rect[2] && point.y > rect[1] && point.y < rect[7]);
		}

		PENGINE_API inline bool Contains(const std::string& string, const std::string& content)
		{
			return string.find(content) != std::string::npos;
		}

		PENGINE_API inline bool IsUserDefinedComponent(const std::string& type)
		{
			return !Utils::Contains(type, "Pengine::");
		}

		template<class T>
		inline bool IsThere(const std::vector<T>& vector, T object)
		{
			return std::find(vector.begin(), vector.end(), object) != vector.end();
		}

		template<class T>
		PENGINE_API inline bool Erase(std::vector<T*>& vector, T* object)
		{
			auto objectIter = std::find(vector.begin(), vector.end(), object);
			if (objectIter != vector.end())
			{
				vector.erase(objectIter);
				return true;
			}
			return false;
		}

		template<class T>
		PENGINE_API inline bool Erase(std::vector<T>& vector, T object)
		{
			auto objectIter = std::find(vector.begin(), vector.end(), object);
			if (objectIter != vector.end())
			{
				vector.erase(objectIter);
				return true;
			}
			return false;
		}

		template<typename T>
		PENGINE_API inline bool IsThere(const std::unordered_map<UUID, T*>& map, UUID uuid)
		{
			return map.find(uuid) != map.end();
		}

		template<typename T>
		PENGINE_API inline bool Erase(std::unordered_map<UUID, T*>& map, UUID uuid)
		{
			auto objectIter = map.find(uuid);
			if (objectIter != map.end())
			{
				map.erase(objectIter);
				return true;
			}
			return false;
		}

		template<typename T>
		PENGINE_API inline T* Find(std::unordered_map <std::string, T*>& map, T* value)
		{
			auto objectIter = std::find_if(map.begin(), map.end(), [value](const auto& iter) { return iter.second == value; });
			if (objectIter != map.end())
			{
				return objectIter.second;
			}
			return nullptr;
		}

		template<typename T>
		PENGINE_API inline bool Erase(std::unordered_map <std::string, T*>& map, T* value)
		{
			auto objectIter = std::find_if(map.begin(), map.end(), [value](const auto& iter) { return iter.second == value; });
			if (objectIter != map.end())
			{
				map.erase(objectIter);
				return true;
			}
			return false;
		}

		template<class T>
		inline std::string GetTypeName()
		{
			return (std::string(typeid(T).name()).substr(6));
		}

		PENGINE_API inline void Print(const glm::mat3& mat)
		{
			for (size_t i = 0; i < 3; i++)
			{
				for (size_t j = 0; j < 3; j++)
				{
					printf_s("%f ", mat[j][i]);
				}
				printf_s("\n");
			}
		}

		PENGINE_API inline void Print(const glm::mat4& mat)
		{
			for (size_t i = 0; i < 4; i++)
			{
				for (size_t j = 0; j < 4; j++)
				{
					printf_s("%f ", mat[j][i]);
				}
				printf_s("\n");
			}
		}

		PENGINE_API inline glm::vec3 GetPosition(const glm::mat4& position)
		{
			const float* matPtr = glm::value_ptr(position);
			return { matPtr[12], matPtr[13], matPtr[14] };
		}

		PENGINE_API inline glm::vec3 GetScale(const glm::mat4& scale)
		{
			const float* matPtr = glm::value_ptr(scale);
			return { matPtr[0], matPtr[5], matPtr[10] };
		}

		PENGINE_API inline std::string RemoveResolution(const std::string& string)
		{
			size_t index = string.find_last_of(".");
			if (index == std::string::npos)
			{
				return string;
			}
			return string.substr(0, index);
		}

		PENGINE_API inline std::string GetNameFromFilePath(const std::string& path, int resolutionLength = 3)
		{
			size_t index = path.find_last_of("/");
			if (index == std::string::npos)
			{
				index = path.find_last_of("\\");
			}
			return path.substr(index + 1, path.length() - index - 2 - resolutionLength);
		}

		PENGINE_API inline std::string ReplaceSlashWithDoubleSlash(const std::string& string)
		{
			std::string replacedString = string;
			std::replace(replacedString.begin(), replacedString.end(), '/', '\\');
			return replacedString;
		}

		PENGINE_API inline std::string GetDirectoryFromFilePath(const std::string& path)
		{
			size_t index = path.find_last_of("/");
			if (index == std::string::npos)
			{
				index = path.find_last_of("\\");
			}
			return path.substr(0, index);
		}

		PENGINE_API inline std::string GetResolutionFromFilePath(const std::string& path)
		{
			size_t index = path.find_last_of(".");
			if (index == std::string::npos)
			{
				return "None";
			}
			return path.substr(index + 1, path.length() - 1);
		}

		PENGINE_API inline bool MatchType(const std::string& path, std::vector<std::string> types)
		{
			for (size_t i = 0; i < types.size(); i++)
			{
				if (Utils::GetResolutionFromFilePath(path) == types[i])
				{
					return true;
				}
			}

			return false;
		}

		PENGINE_API inline void LoadShadersFromFolder(const std::string& path)
		{
			std::vector<std::string> shaders;
			for (const auto& entry : fs::directory_iterator(path))
			{
				shaders.push_back(entry.path().string());
				size_t index = 0;
				index = shaders.back().find("\\", index);
				shaders.back().replace(index, 1, "/");
				Shader::Create(GetNameFromFilePath(shaders.back(), 6), shaders.back());
			}
		}

		PENGINE_API inline std::vector<Texture*> LoadTexturesFromFolder(const std::string& path)
		{
			std::vector<std::string> textures;
			std::vector<Texture*> texturesPtr;
			for (const auto& entry : fs::directory_iterator(path))
			{
				std::string pathToTexture = entry.path().string();
				textures.push_back(entry.path().string());
				size_t index = 0;
				index = textures.back().find("\\", index);
				textures.back().replace(index, 1, "/");
				if (auto texture = TextureManager::GetInstance().Create(textures.back()))
				{
					texturesPtr.push_back(texture);
				}
			}

			return texturesPtr;
		}

		template<class T>
		PENGINE_API inline char* PreAllocateMemory(std::map<char*, uint64_t>& memoryPool, std::vector<char*>& freeMemory)
		{
			if (freeMemory.capacity() == 0)
			{
				freeMemory.reserve(MAX_PREALLOCATED_INSTANCES);
			}

			if (memoryPool.size() == 0 || memoryPool.rbegin()->second == MAX_PREALLOCATED_INSTANCES)
			{
				char* newPreAllocMemory = new char[sizeof(T) * MAX_PREALLOCATED_INSTANCES];
				memoryPool.insert(std::make_pair(newPreAllocMemory, 0));
				for (size_t i = 0; i < MAX_PREALLOCATED_INSTANCES * sizeof(T); i += sizeof(T))
				{
					freeMemory.push_back(newPreAllocMemory + i);
				}
			}

			if (freeMemory.size() > 0)
			{
				for (auto memoryPoolIter = memoryPool.begin(); memoryPoolIter != memoryPool.end(); memoryPoolIter++)
				{
					uint64_t ptr = (uint64_t)freeMemory.back();
					if ((uint64_t)(memoryPoolIter->first) <= ptr && ptr <= (uint64_t)(memoryPoolIter->first) + MAX_PREALLOCATED_INSTANCES * sizeof(T))
					{
						memoryPoolIter->second++;
						char* ptr = freeMemory.back();
						freeMemory.pop_back();

						return ptr;
					}
				}
			}

			assert(false);
		}

		inline bool DecomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale)
		{
			// From glm::decompose in matrix_decompose.inl

			using namespace glm;
			using T = float;

			mat4 LocalMatrix(transform);

			// Normalize the matrix.
			if (epsilonEqual(LocalMatrix[3][3], static_cast<float>(0), epsilon<T>())) return false;

			// First, isolate perspective.  This is the messiest.
			if (
				epsilonNotEqual(LocalMatrix[0][3], static_cast<T>(0), epsilon<T>()) ||
				epsilonNotEqual(LocalMatrix[1][3], static_cast<T>(0), epsilon<T>()) ||
				epsilonNotEqual(LocalMatrix[2][3], static_cast<T>(0), epsilon<T>()))
			{
				// Clear the perspective partition
				LocalMatrix[0][3] = LocalMatrix[1][3] = LocalMatrix[2][3] = static_cast<T>(0);
				LocalMatrix[3][3] = static_cast<T>(1);
			}

			// Next take care of translation (easy).
			translation = vec3(LocalMatrix[3]);
			LocalMatrix[3] = vec4(0, 0, 0, LocalMatrix[3].w);

			vec3 Row[3], Pdum3;

			// Now get scale and shear.
			for (length_t i = 0; i < 3; ++i)
				for (length_t j = 0; j < 3; ++j)
					Row[i][j] = LocalMatrix[i][j];

			// Compute X scale factor and normalize first row.
			scale.x = length(Row[0]);
			Row[0] = detail::scale(Row[0], static_cast<T>(1));
			scale.y = length(Row[1]);
			Row[1] = detail::scale(Row[1], static_cast<T>(1));
			scale.z = length(Row[2]);
			Row[2] = detail::scale(Row[2], static_cast<T>(1));

			// At this point, the matrix (in rows[]) is orthonormal.
			// Check for a coordinate system flip.  If the determinant
			// is -1, then negate the matrix and the scaling factors.
#if 0
			Pdum3 = cross(Row[1], Row[2]); // v3Cross(row[1], row[2], Pdum3);
			if (dot(Row[0], Pdum3) < 0)
			{
				for (length_t i = 0; i < 3; i++)
				{
					scale[i] *= static_cast<T>(-1);
					Row[i] *= static_cast<T>(-1);
				}
			}
#endif

			rotation.y = asin(-Row[0][2]);
			if (cos(rotation.y) != 0)
			{
				rotation.x = atan2(Row[1][2], Row[2][2]);
				rotation.z = atan2(Row[0][1], Row[0][0]);
			}
			else
			{
				rotation.x = atan2(-Row[2][0], Row[1][1]);
				rotation.z = 0;
			}
			return true;
		}

		PENGINE_API inline int DeleteDirectory(const std::wstring& refcstrRootDirectory,
			bool              bDeleteSubdirectories = false)
		{
			bool            bSubdirectory = false;       // Flag, indicating whether
														 // subdirectories have been found
			HANDLE          hFile;                       // Handle to directory
			std::wstring     strFilePath;                 // Filepath
			std::wstring     strPattern;                  // Pattern
			WIN32_FIND_DATA FileInformation;             // File information


			strPattern = refcstrRootDirectory + L"\\*.*";
			hFile = ::FindFirstFile(strPattern.c_str(), &FileInformation);
			if (hFile != INVALID_HANDLE_VALUE)
			{
				do
				{
					if (FileInformation.cFileName[0] != '.')
					{
						strFilePath.erase();
						strFilePath = refcstrRootDirectory + L"\\" + FileInformation.cFileName;

						if (FileInformation.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
						{
							if (bDeleteSubdirectories)
							{
								// Delete subdirectory
								int iRC = DeleteDirectory(strFilePath, bDeleteSubdirectories);
								if (iRC)
									return iRC;
							}
							else
								bSubdirectory = true;
						}
						else
						{
							// Set file attributes
							if (::SetFileAttributes(strFilePath.c_str(),
								FILE_ATTRIBUTE_NORMAL) == FALSE)
								return ::GetLastError();

							// Delete file
							if (::DeleteFile(strFilePath.c_str()) == FALSE)
								return ::GetLastError();
						}
					}
				} while (::FindNextFile(hFile, &FileInformation) == TRUE);

				// Close handle
				::FindClose(hFile);

				DWORD dwError = ::GetLastError();
				if (dwError != ERROR_NO_MORE_FILES)
					return dwError;
				else
				{
					if (!bSubdirectory)
					{
						// Set directory attributes
						if (::SetFileAttributes(refcstrRootDirectory.c_str(),
							FILE_ATTRIBUTE_NORMAL) == FALSE)
							return ::GetLastError();

						// Delete directory
						if (::RemoveDirectory(refcstrRootDirectory.c_str()) == FALSE)
							return ::GetLastError();
					}
				}
			}

			return 0;
		}

	}

}