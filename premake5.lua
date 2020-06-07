workspace "Chat"
	architecture "x86"

	configurations
	{
		"Debug",
		"Release"
	}


outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "Chatclient"
	location "Chatclient"
	kind "ConsoleApp"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin/intermediates/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/*.h",
		"%{prj.name}/src/*.cpp",
		"%{prj.name}/src/vendor/imgui/*.h",
		"%{prj.name}/src/vendor/imgui/*.cpp",
		"%{prj.name}/src/vendor/imgui/examples/imgui_impl_glfw.h",
		"%{prj.name}/src/vendor/imgui/examples/imgui_impl_glfw.cpp",
		"%{prj.name}/src/vendor/imgui/examples/imgui_impl_opengl3.h",
		"%{prj.name}/src/vendor/imgui/examples/imgui_impl_opengl3.cpp"
	}
	

	includedirs
	{
		"dependencies/GLFW/include",
		"dependencies/GLEW/include",
		"%{prj.location}/src/vendor/imgui"
	}

	libdirs
	{
		"dependencies/GLFW/lib-vc2019",
		"dependencies/GLEW/lib/Release/Win32"
	}

	links
	{
		"glfw3.lib",
		"opengl32.lib",
		"User32.lib",
		"Gdi32.lib",
		"Shell32.lib",
		"glew32s.lib"
	}

	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"

		defines
		{
			"GLEW_STATIC"
		}

	filter "configurations:Debug"
		defines "CC_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "CC_RELEASE"
		optimize "On"


project "Chatserver"
	location "Chatserver"
	kind "ConsoleApp"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin/intermediates/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/*.h",
		"%{prj.name}/src/*.cpp"
	}

	includedirs
	{
	}

	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"

		defines
		{
		}

	filter "configurations:Debug"
		defines "CS_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "CS_RELEASE"
		optimize "On"
 
