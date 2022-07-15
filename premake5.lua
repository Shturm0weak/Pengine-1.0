workspace "PengineEngine"
	architecture "x64"

	configurations {
		"Debug",
		"Release"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "Pengine"
	location "Pengine"
	kind "SharedLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files {
		"%{prj.name}/**.h",
		"%{prj.name}/**.cpp"
	}

	includedirs {
		"$(SolutionDir)Includes/GLEW",
		"$(SolutionDir)Includes/GLFW",
		"$(SolutionDir)Includes/LUA",
		"$(SolutionDir)Includes",
		"$(SolutionDir)Vendor",
		"$(SolutionDir)Vendor/ImGui",
		"$(SolutionDir)%{prj.name}/Source/%{prj.name}"
	}

	links {
		"Box2D",
		"Yaml",
		"ImGui",
		"RTTR",
		"glfw3.lib",
		"opengl32.lib",
		"glew32s.lib",
		"ImGui.lib",
		"OpenAL32.lib",
		"Box2D.lib",
		"RTTR.lib",
		"Yaml.lib"
	}

	filter "system:windows"
		systemversion "latest"
		cppdialect "C++17"

		defines {
			"_CRT_SECURE_NO_WARNINGS",
			"GLEW_STATIC",
			"PENGINE_ENGINE",
			"_WIN64",
			"VORBIS_FPU_CONTROL"
		}

		postbuildcommands {
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/SandBox"),
			("{COPY} $(SolutionDir)Dlls/lua54.dll ../bin/" .. outputdir .. "/SandBox"),
		}

		flags {
			"MultiProcessorCompile",
		}

	filter "configurations:Debug"
		runtime "Debug"
		defines "_DEBUG"
		symbols "on"
		
		libdirs {"$(SolutionDir)Libs/Debug"}

		postbuildcommands {
			("{COPY} $(SolutionDir)bin/" .. outputdir .. "/Pengine/Pengine.lib $(SolutionDir)Libs/Release")
		}

	filter "configurations:Release"
		defines "_RELEASE"
		runtime "Release"
		symbols "on"
		optimize "Full"

		libdirs {"$(SolutionDir)Libs/Release"}

		postbuildcommands {
			("{COPY} $(SolutionDir)bin/" .. outputdir .. "/Pengine/Pengine.lib $(SolutionDir)Libs/Debug")
		}

project "SandBox"
	location "SandBox"
	kind "ConsoleApp"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	includedirs {
		"$(SolutionDir)Includes/GLEW",
		"$(SolutionDir)Includes/GLFW",
		"$(SolutionDir)Includes/LUA",
		"$(SolutionDir)Includes",
		"$(SolutionDir)Vendor",
		"$(SolutionDir)Vendor/ImGui",
		"$(SolutionDir)%{prj.name}/Source",
		"$(SolutionDir)Pengine/Source"
	}

	links {
		"Pengine",
		"glfw3.lib",
		"opengl32.lib",
		"glew32s.lib",
		"ImGui.lib",
		"OpenAL32.lib",
		"Pengine.lib",
		"Box2D.lib",
		"RTTR.lib",
		"Yaml.lib"
	}

	files {
		"%{prj.name}/**.h",
		"%{prj.name}/**.cpp"
	}

	filter "system:windows"
		systemversion "latest"
		cppdialect "C++17"

		flags {
			"MultiProcessorCompile",
		}

		defines {
			"_CRT_SECURE_NO_WARNINGS",
			"_WIN64",
			"GLEW_STATIC",
			"_WINDLL"
		}

	filter "configurations:Debug"
		defines "_DEBUG"
		runtime "Debug"
		symbols "on"

		libdirs {"$(SolutionDir)Libs/Debug"}

	filter "configurations:Release"
		defines "_RELEASE"
		runtime "Release"
		symbols "on"
		optimize "Full"

		libdirs {"$(SolutionDir)Libs/Release"}

ImGuiLocation = "Vendor/ImGui"

project "ImGui"
	location (ImGuiLocation)
	kind "StaticLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files {
		ImGuiLocation .. "/**.h",
		ImGuiLocation .. "/**.cpp"
	}

	includedirs {
		"$(SolutionDir)Vendor/ImGui",
		"$(SolutionDir)Includes/GLEW",
		"$(SolutionDir)Vendor/ImGui/libs/glfw/include",
		"$(SolutionDir)Vendor/ImGui/misc/freetype",
	}

	filter "system:windows"
		systemversion "latest"
		cppdialect "C++17"
		staticruntime "On"

		flags {
			"MultiProcessorCompile",
		}

	filter "system:linux"
		pic "On"
		systemversion "latest"
		cppdialect "C++17"
		staticruntime "On"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

		postbuildcommands {
			("{COPY} $(SolutionDir)bin/" .. outputdir .. "/ImGui/ImGui.lib $(SolutionDir)Libs/Debug")
		}

	filter "configurations:Release"
		runtime "Release"
		optimize "Full"

		postbuildcommands {
			("{COPY} $(SolutionDir)bin/" .. outputdir .. "/ImGui/ImGui.lib $(SolutionDir)Libs/Release")
		}

Box2DLocation = "Vendor/Box2D"

project "Box2D"
	location (Box2DLocation)
	kind "StaticLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files {
		Box2DLocation .. "/**.h",
		Box2DLocation .. "/**.cpp"
	}

	includedirs {
		"$(SolutionDir)Vendor/Box2D/include",
		"$(SolutionDir)Vendor/Box2D/src",
		"$(SolutionDir)Vendor/Box2D/testbed",
		"$(SolutionDir)Vendor/Box2D/extern",
		"$(SolutionDir)Vendor/Box2D/extern/glad/include",
		"$(SolutionDir)Vendor/Box2D/extern/glfw/include"
	}

	filter "system:windows"
		systemversion "latest"
		cppdialect "C++17"
		staticruntime "Off"

		flags {
			"MultiProcessorCompile",
		}

	filter "system:linux"
		pic "On"
		systemversion "latest"
		cppdialect "C++17"
		staticruntime "Off"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

		postbuildcommands {
			("{COPY} $(SolutionDir)bin/" .. outputdir .. "/Box2D/Box2D.lib $(SolutionDir)Libs/Debug")
		}

	filter "configurations:Release"
		runtime "Release"
		optimize "Full"

		postbuildcommands {
			("{COPY} $(SolutionDir)bin/" .. outputdir .. "/Box2D/Box2D.lib $(SolutionDir)Libs/Release")
		}

YamlLocation = "Vendor/yaml-cpp"

project "Yaml"
	location (YamlLocation)
	kind "StaticLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files {
		YamlLocation .. "/**.h",
		YamlLocation .. "/**.cpp"
	}

	includedirs {
		"$(SolutionDir)Vendor/yaml-cpp"
	}

	filter "system:windows"
		systemversion "latest"
		cppdialect "C++17"
		staticruntime "Off"

		flags {
			"MultiProcessorCompile",
		}

	filter "system:linux"
		pic "On"
		systemversion "latest"
		cppdialect "C++17"
		staticruntime "Off"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

		postbuildcommands {
			("{COPY} $(SolutionDir)bin/" .. outputdir .. "/Yaml/Yaml.lib $(SolutionDir)Libs/Debug")
		}

	filter "configurations:Release"
		runtime "Release"
		optimize "Full"

		postbuildcommands {
			("{COPY} $(SolutionDir)bin/" .. outputdir .. "/Yaml/Yaml.lib $(SolutionDir)Libs/Release")
		}

RTTRLocation = "Vendor/rttr"

project "RTTR"
	location (RTTRLocation)
	kind "StaticLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files {
		RTTRLocation .. "/**.h",
		RTTRLocation .. "/**.cpp"
	}

	includedirs {
		"$(SolutionDir)Vendor"
	}

	filter "system:windows"
		systemversion "latest"
		cppdialect "C++17"
		staticruntime "Off"

		flags {
			"MultiProcessorCompile",
		}

	filter "system:linux"
		pic "On"
		systemversion "latest"
		cppdialect "C++17"
		staticruntime "Off"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

		postbuildcommands {
			("{COPY} $(SolutionDir)bin/" .. outputdir .. "/RTTR/RTTR.lib $(SolutionDir)Libs/Debug")
		}

	filter "configurations:Release"
		runtime "Release"
		optimize "Full"

		postbuildcommands {
			("{COPY} $(SolutionDir)bin/" .. outputdir .. "/RTTR/RTTR.lib $(SolutionDir)Libs/Release")
		}