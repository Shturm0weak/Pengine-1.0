workspace "PengineEngine"
	architecture "x64"

	configurations{
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

	includedirs{
		"$(SolutionDir)Includes/GLEW",
		"$(SolutionDir)Includes/GLFW",
		"$(SolutionDir)Vendor",
		"$(SolutionDir)%{prj.name}/Source/%{prj.name}"
	}

	libdirs {"$(SolutionDir)Libs"}

	links{
		"glfw3.lib",
		"opengl32.lib",
		"glew32s.lib",
		"ImGui.lib",
		"OpenAL32.lib",
		"Box2D.lib"
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
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/SandBox")
		}

		flags {
		 "MultiProcessorCompile",
		}

	filter "configurations:Debug"
		runtime "Debug"
		defines "_DEBUG"
		symbols "on"

	filter "configurations:Release"
		defines "_RELEASE"
		runtime "Release"
		symbols "on"
		optimize "Full"

project "SandBox"
	location "SandBox"
	kind "ConsoleApp"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	includedirs{
		"$(SolutionDir)Includes/GLEW",
		"$(SolutionDir)Includes/GLFW",
		"$(SolutionDir)Vendor",
		"$(SolutionDir)%{prj.name}/Source",
		"$(SolutionDir)Pengine/Source"
	}

	links{
		"glfw3.lib",
		"opengl32.lib",
		"glew32s.lib",
		"ImGui.lib",
		"OpenAL32.lib",
		"Pengine.lib",
		"Box2D.lib"
	}

	files {
		"%{prj.name}/**.h",
		"%{prj.name}/**.cpp"
	}

	libdirs {
		"$(SolutionDir)Libs",
		"bin/" .. outputdir .. "/Pengine"
	}

	filter "system:windows"
		systemversion "latest"
		cppdialect "C++17"

		flags {
		 "MultiProcessorCompile",
		}

		defines{
			"_CRT_SECURE_NO_WARNINGS",
			"_WIN64",
			"GLEW_STATIC",
			"_WINDLL"
		}

	filter "configurations:Debug"
		defines "_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "_RELEASE"
		runtime "Release"
		symbols "on"
		optimize "Full"