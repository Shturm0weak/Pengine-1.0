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
		"$(SolutionDir)Vendor/ImGui"
	}

	links {
		"Box2D",
		"Yaml",
		"ImGui",
		"glfw3.lib",
		"opengl32.lib",
		"glew32s.lib",
		"ImGui.lib",
		"OpenAL32.lib",
		"Box2D.lib",
		"Yaml.lib",
		"BulletDynamics.lib",
		"BulletCollision.lib",
		"LinearMath.lib"
	}

	postbuildcommands {
		("{COPY} %{cfg.buildtarget.relpath} $(SolutionDir)/bin/" .. outputdir .. "/SandBox"),
		("{COPY} $(SolutionDir)Dlls/lua54.dll $(SolutionDir)/bin/" .. outputdir .. "/SandBox")
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

		flags {
			"MultiProcessorCompile"
		}

	filter "configurations:Debug"
		runtime "Debug"
		defines "_DEBUG"
		symbols "on"
		
		postbuildcommands {
			("{COPY} $(SolutionDir)bin/" .. outputdir .. "/%{prj.name}/%{prj.name}.lib $(SolutionDir)Libs/Debug"),
		}

		libdirs {"$(SolutionDir)Libs/Debug"}
		
	filter "configurations:Release"
		defines "_RELEASE"
		runtime "Release"
		symbols "on"
		optimize "Full"

		postbuildcommands {
			("{COPY} $(SolutionDir)bin/" .. outputdir .. "/%{prj.name}/%{prj.name}.lib $(SolutionDir)Libs/Release"),
		}

		libdirs {"$(SolutionDir)Libs/Release"}

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
		"Yaml.lib",
		"BulletDynamics.lib",
		"BulletCollision.lib",
		"LinearMath.lib"
	}

	files {
		"%{prj.name}/**.h",
		"%{prj.name}/**.cpp"
	}

	filter "system:windows"
		systemversion "latest"
		cppdialect "C++17"

		flags {
			"MultiProcessorCompile"
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
		"$(SolutionDir)" .. ImGuiLocation,
		"$(SolutionDir)Includes/GLEW",
		"$(SolutionDir)" .. ImGuiLocation .. "/libs/glfw/include",
		"$(SolutionDir)" .. ImGuiLocation .. "/misc/freetype"
	}

	filter "system:windows"
		systemversion "latest"
		cppdialect "C++17"
		staticruntime "On"

		flags {
			"MultiProcessorCompile"
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
			("{COPY} $(SolutionDir)bin/" .. outputdir .. "/%{prj.name}/%{prj.name}.lib $(SolutionDir)Libs/Debug")
		}

	filter "configurations:Release"
		runtime "Release"
		optimize "Full"

		postbuildcommands {
			("{COPY} $(SolutionDir)bin/" .. outputdir .. "/%{prj.name}/%{prj.name}.lib $(SolutionDir)Libs/Release")
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
		"$(SolutionDir)" .. Box2DLocation .. "/include",
		"$(SolutionDir)" .. Box2DLocation .. "/src",
		"$(SolutionDir)" .. Box2DLocation .. "/testbed",
		"$(SolutionDir)" .. Box2DLocation .. "/extern",
		"$(SolutionDir)" .. Box2DLocation .. "/extern/glad/include",
		"$(SolutionDir)" .. Box2DLocation .. "/extern/glfw/include"
	}

	filter "system:windows"
		systemversion "latest"
		cppdialect "C++17"
		staticruntime "Off"

		flags {
			"MultiProcessorCompile"
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
			("{COPY} $(SolutionDir)bin/" .. outputdir .. "/%{prj.name}/%{prj.name}.lib $(SolutionDir)Libs/Debug")
		}

	filter "configurations:Release"
		runtime "Release"
		optimize "Full"

		postbuildcommands {
			("{COPY} $(SolutionDir)bin/" .. outputdir .. "/%{prj.name}/%{prj.name}.lib $(SolutionDir)Libs/Release")
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
		"$(SolutionDir)" .. YamlLocation
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
			("{COPY} $(SolutionDir)bin/" .. outputdir .. "/%{prj.name}/%{prj.name}.lib $(SolutionDir)Libs/Debug")
		}

	filter "configurations:Release"
		runtime "Release"
		optimize "Full"

		postbuildcommands {
			("{COPY} $(SolutionDir)bin/" .. outputdir .. "/%{prj.name}/%{prj.name}.lib $(SolutionDir)Libs/Release")
		}

BulletDynamicsLocation = "Vendor/BulletDynamics"

project "BulletDynamics"
	location (BulletDynamicsLocation)
	kind "StaticLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files {
		BulletDynamicsLocation .. "/**.h",
		BulletDynamicsLocation .. "/**.cpp"
	}

	includedirs {
		"$(SolutionDir)Vendor",
		"$(SolutionDir)Includes/BulletCommon"
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
			("{COPY} $(SolutionDir)bin/" .. outputdir .. "/%{prj.name}/%{prj.name}.lib $(SolutionDir)Libs/Debug")
		}

	filter "configurations:Release"
		runtime "Release"
		optimize "Full"

		postbuildcommands {
			("{COPY} $(SolutionDir)bin/" .. outputdir .. "/%{prj.name}/%{prj.name}.lib $(SolutionDir)Libs/Release")
		}

BulletCollisionLocation = "Vendor/BulletCollision"

project "BulletCollision"
	location (BulletCollisionLocation)
	kind "StaticLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files {
		BulletCollisionLocation .. "/**.h",
		BulletCollisionLocation .. "/**.cpp"
	}

	includedirs {
		"$(SolutionDir)Vendor",
		"$(SolutionDir)Includes/BulletCommon"
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
			("{COPY} $(SolutionDir)bin/" .. outputdir .. "/%{prj.name}/%{prj.name}.lib $(SolutionDir)Libs/Debug")
		}

	filter "configurations:Release"
		runtime "Release"
		optimize "Full"

		postbuildcommands {
			("{COPY} $(SolutionDir)bin/" .. outputdir .. "/%{prj.name}/%{prj.name}.lib $(SolutionDir)Libs/Release")
		}


LinearMathLocation = "Vendor/LinearMath"

project "LinearMath"
	location (LinearMathLocation)
	kind "StaticLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files {
		LinearMathLocation .. "/**.h",
		LinearMathLocation .. "/**.cpp"
	}

	includedirs {
		"$(SolutionDir)Vendor",
		"$(SolutionDir)Includes/BulletCommon"
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
			("{COPY} $(SolutionDir)bin/" .. outputdir .. "/%{prj.name}/%{prj.name}.lib $(SolutionDir)Libs/Debug")
		}

	filter "configurations:Release"
		runtime "Release"
		optimize "Full"

		postbuildcommands {
			("{COPY} $(SolutionDir)bin/" .. outputdir .. "/%{prj.name}/%{prj.name}.lib $(SolutionDir)Libs/Release")
		}