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
        # Enables ImGui Freetype font rendering support.
        # https://github.com/ocornut/imgui/blob/master/misc/freetype/README.md#usage
        "IMGUI_ENABLE_FREETYPE",
        # Allows the use of math operators with ImGui math types.
        # https://github.com/ocornut/imgui/issues/2832#issuecomment-538777840
        "IMGUI_DEFINE_MATH_OPERATORS",
        # Improves compilation time for spdlog dependency.
        # Note that spdlog must be compiled first to produce `libspdlog.a`,
        # a step which is automated by `scripts/get_dependencies.py`.
        # https://github.com/gabime/spdlog/issues/1708#issuecomment-709589158
        "SPDLOG_COMPILED_LIB",
    ],
    LIBS = Split(cpp_libs),
)

if build_mode == "debug":
    binary_name += "_debug"
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
elif build_mode == "fast":
    binary_name += "_fast"
    env.Append(
        CPPDEFINES="NDEBUG",
        CPPFLAGS=["-O0"],
    )
elif build_mode == "release":
    env.Append(
        CPPDEFINES="NDEBUG",
        CPPFLAGS=["-O2"],
    )
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

