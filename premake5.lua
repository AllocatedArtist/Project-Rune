workspace "Project Rune"
  configurations { "Debug", "Release" }
  location "bin"
  includedirs { 
    "vendor/glad/include",
    "vendor/glfw/include",
    "vendor/plog/include",
    "vendor/glm",
    "vendor",
    "vendor/entt/single_include",
    "vendor/imgui",
    "vendor/imgui/misc",
    "vendor/tinygltf",
  }

project "Project-Rune"
  kind "ConsoleApp"
  language "C++"
  cppdialect "C++17"
  targetdir "bin/%{cfg.buildcfg}"
  toolset "gcc"

  libdirs "libs"
  links { 
    "glfw3",
    "gdi32",
  }

  files { 
    "vendor/tinygltf/tiny_gltf.cc",
    "vendor/imgui/*.cpp",
    "vendor/imgui/backends/imgui_impl_opengl3.cpp",
    "vendor/imgui/backends/imgui_impl_glfw.cpp",
    "vendor/imgui/misc/cpp/*.cpp",
    "vendor/glad/src/*.cc",
    "vendor/stb/*.cc",
    "src/**.cc" 
  }

  defines { 
  }

  filter "configurations:Debug"
  defines { "DEBUG" }
  optimize "Debug"
  symbols "On"

  filter "configurations:Release"
  defines { "RELEASE" }
  optimize "Speed"
