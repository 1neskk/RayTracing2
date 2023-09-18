-- premake5.lua

VULKAN_SDK = os.getenv("VULKAN_SDK")
IncludeDir = {}
IncludeDir["VulkanSDK"] = "%{VULKAN_SDK}/Include"

LibraryDir = {}
LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/Lib"

Library = {}
Library["Vulkan"] = "vulkan-1.lib"

workspace "PathTracer"
    architecture "x64"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

    project "RayTracing"
        location "RayTracing"
        kind "ConsoleApp"
        language "C++"
        cppdialect "C++17"
        --staticruntime "on"

        targetdir ("bin/" .. outputdir .. "/%{prj.name}")
        objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

        files
        {
            "%{prj.name}/src/**.h",
            "%{prj.name}/src/**.cpp"
        }

        includedirs
        {
            "%{prj.name}/thirdparty/glm",
            "%{prj.name}/thirdparty/glfw/include/",
            "%{prj.name}/thirdparty/imgui",
            "%{prj.name}/thirdparty/stb",
            "%{IncludeDir.VulkanSDK}"
        }

        links
        {
            "ImGui.lib",
            "glfw3.lib",
            "opengl32.lib",
            "%{Library.Vulkan}"
        }

        libdirs
        {
            "%{prj.name}/libs/",
            "%{LibraryDir.VulkanSDK}"
        }

        filter "system:windows"
            systemversion "latest"

            defines
            {
                "PT_PLATFORM_WINDOWS"
            }

        filter "configurations:Debug"
            defines { "PT_DEBUG" }
            runtime "Debug"
            symbols "On"

        filter "configurations:Release"
            defines { "PT_RELEASE" }
            runtime "Release"
            optimize "On"
            symbols "On"

        filter "configurations:Dist"
            defines { "PT_DIST" }
            runtime "Release"
            optimize "On"
            symbols "Off"