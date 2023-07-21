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
  }

project "Project-Rune"
  kind "ConsoleApp"
  language "C++"
  cppdialect "C++17"
  targetdir "bin/%{cfg.buildcfg}"

  libdirs "libs"
  links { 
    "glfw3",
    "gdi32",
  }


  files { 
    "vendor/glad/src/*.cc",
    "vendor/stb/*.cc",
    "src/**.cc" 
  }

  filter "configurations:Debug"
  defines { "DEBUG" }
  optimize "Debug"
  symbols "On"

  filter "configurations:Release"
  defines { "RELEASE" }
  optimize "Speed"
