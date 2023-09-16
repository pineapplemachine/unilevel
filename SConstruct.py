# SConscript("SConsript.unilevel.scons", variant_dir="build", duplicate=0)

import sys, os

cpp_platform = ""
cpp_libs = ""
cpp_tools = []
binary_ext = ""

build_mode = ARGUMENTS.get("mode", "release")
binary_name = "unilevel"
binary_dir = build_mode

if sys.platform.startswith("linux"):
    cpp_platform = "PLATFORM_LINUX"
    cpp_libs = "raylib freetype spdlog GL m pthread dl rt X11"
    cpp_tools = ["default", "g++"]
elif sys.platform.startswith("win"):
    cpp_platform = "PLATFORM_WINDOWS"
    cpp_libs = "raylib freetype spdlog opengl32 gdi32 winmm"
    cpp_tools = ["mingw", "g++"]
    binary_ext = ".exe"
elif sys.platform.startswith("darwin"):
    cpp_platform = "PLATFORM_DARWIN"
    cpp_libs = "raylib freetype spdlog GL m pthread dl rt X11"
    cpp_tools = ["default", "g++"]
else:
    raise ValueError(
        "Build script does not yet support this platform: %s" %
        sys.platform
    )

env = Environment(
    tools = cpp_tools,
    CPPFLAGS = [
        "-std=c++20",
    ],
    LIBPATH = [
        "./lib"
    ],
    CPPPATH = [
        "./src",
        "./include",
        "./include/imgui",
        "./include/imgui/misc/freetype",
        "./include/raylib",
        "./include/rlImGui",
    ],
    CPPDEFINES = [
        cpp_platform,
        "IMGUI_DEFINE_MATH_OPERATORS",
        "IMGUI_ENABLE_FREETYPE",
    ],
    LIBS = Split(cpp_libs),
)

if build_mode == "debug":
    env.Append(
        CPPDEFINES="DEBUG",
        CPPFLAGS=[
            "-g",
            "-fsanitize=undefined",
            "-fsanitize-trap"
            # https://stackoverflow.com/questions/6404636/libstdc-6-dll-not-found
            # "-static-libgcc",
            # "-static-libstdc++",
        ],
    )
    binary_name += "_debug"
elif build_mode == "fast":
    env.Append(CPPFLAGS=["-O0"])
    binary_name += "_fast"
elif build_mode == "release":
    env.Append(CPPFLAGS=["-O2"])
else:
    raise ValueError("Unknown build mode %s" % build_mode)

variant_dir = "bin/obj/"
variant_dir_src = "%s/src" % variant_dir
variant_dir_include = "%s/include" % variant_dir
env.VariantDir(variant_dir_src, "src", duplicate=0)
env.VariantDir(variant_dir_include, "include", duplicate=0)

sources = []
for root, dirs, files in os.walk("src"):
    sources.append(Glob("%s/%s/*.cpp" % (variant_dir, root)))
for root, dirs, files in os.walk("include"):
    sources.append(Glob("%s/%s/*.cpp" % (variant_dir, root)))

binary_path = "./%s" % (binary_name)
binary_file_path = binary_path + binary_ext
build = env.Program(
    target=binary_path,
    source=sources,
)

