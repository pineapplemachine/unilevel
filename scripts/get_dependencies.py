import os
import shutil
import subprocess
import sys
import urllib.request

import raylib_rename

# Dependencies are downloaded and built in this directory
WORKING_PATH = "cached"

# Version for raylib
RAYLIB_VERSION = "4.5.0"
# Version for Dear ImGui
IMGUI_VERSION = "1.89.9"
# Commit hash for rlImGui (no tagged releases)
RLIMGUI_VERSION = "af19e7de4736d3c2d237f1c225298d1d39c102ff"
# Version for Freetype
FREETYPE_VERSION = "2.13.2"
# Version for spdlog
SPDLOG_VERSION = "1.12.0"

PROJECT_SPDLOG_LIB_NAME = (
    "spdlog.lib" if sys.platform.startswith("win") else "libspdlog.a"
)

# Files are copied from dependencies to these locations
PROJECT_LIB_PATH = "lib"
PROJECT_RAYLIB_LIB_PATH = "%s/libraylib.a" % PROJECT_LIB_PATH
PROJECT_FREETYPE_LIB_PATH = "%s/libfreetype.a" % PROJECT_LIB_PATH
PROJECT_SPDLOG_LIB_PATH = "%s/%s" % (PROJECT_LIB_PATH, PROJECT_SPDLOG_LIB_NAME)
PROJECT_INCLUDE_PATH = "include"
PROJECT_RAYLIB_INCLUDE_PATH = "%s/raylib" % PROJECT_INCLUDE_PATH
PROJECT_IMGUI_INCLUDE_PATH = "%s/imgui" % PROJECT_INCLUDE_PATH
PROJECT_IMGUI_INCLUDE_FREETYPE_PATH = "%s/misc/freetype" % PROJECT_IMGUI_INCLUDE_PATH
PROJECT_RLIMGUI_INCLUDE_PATH = "%s/rlImGui" % PROJECT_INCLUDE_PATH
PROJECT_RLIMGUI_INCLUDE_EXTRAS_PATH = "%s/extras" % PROJECT_RLIMGUI_INCLUDE_PATH

# Information about raylib dependency
RAYLIB_DOWNLOAD_URL = "https://github.com/raysan5/raylib/archive/refs/tags/%s.tar.gz" % RAYLIB_VERSION
RAYLIB_ARCHIVE_PATH = "%s/raylib-%s.tar.gz" % (WORKING_PATH, RAYLIB_VERSION)
RAYLIB_ARCHIVE_EXTRACT_PATH = "%s/raylib-%s" % (WORKING_PATH, RAYLIB_VERSION)
RAYLIB_ROOT_PATH = "%s/raylib-%s" % (RAYLIB_ARCHIVE_EXTRACT_PATH, RAYLIB_VERSION)
RAYLIB_SRC_PATH = "%s/src" % RAYLIB_ROOT_PATH
RAYLIB_LIB_PATH = "%s/libraylib.a" % RAYLIB_SRC_PATH

# Information about Dear ImGui dependency
IMGUI_DOWNLOAD_URL = "https://github.com/ocornut/imgui/archive/refs/tags/v%s.tar.gz" % IMGUI_VERSION
IMGUI_ARCHIVE_PATH = "%s/imgui-%s.tar.gz" % (WORKING_PATH, IMGUI_VERSION)
IMGUI_ARCHIVE_EXTRACT_PATH = "%s/imgui-%s" % (WORKING_PATH, IMGUI_VERSION)
IMGUI_SRC_PATH = "%s/imgui-%s" % (IMGUI_ARCHIVE_EXTRACT_PATH, IMGUI_VERSION)
IMGUI_SRC_FREETYPE_PATH = "%s/misc/freetype" % (IMGUI_SRC_PATH)

# Information about rlImGui dependency
RLIMGUI_VERSION_SHORT = RLIMGUI_VERSION[:8]
RLIMGUI_DOWNLOAD_URL = "https://github.com/raylib-extras/rlImGui/archive/%s.tar.gz" % RLIMGUI_VERSION
RLIMGUI_ARCHIVE_PATH = "%s/rlImGui-%s.tar.gz" % (WORKING_PATH, RLIMGUI_VERSION_SHORT)
RLIMGUI_ARCHIVE_EXTRACT_PATH = "%s/rlImGui-%s" % (WORKING_PATH, RLIMGUI_VERSION_SHORT)
RLIMGUI_SRC_PATH = "%s/rlImGui-%s" % (RLIMGUI_ARCHIVE_EXTRACT_PATH, RLIMGUI_VERSION)
RLIMGUI_SRC_EXTRAS_PATH = "%s/extras" % RLIMGUI_SRC_PATH

# Information about Freetype dependency
FREETYPE_DOWNLOAD_URL = "https://download.savannah.gnu.org/releases/freetype/freetype-%s.tar.gz" % FREETYPE_VERSION
FREETYPE_ARCHIVE_PATH = "%s/freetype-%s.tar.gz" % (WORKING_PATH, FREETYPE_VERSION)
FREETYPE_ARCHIVE_EXTRACT_PATH = "%s/freetype-%s" % (WORKING_PATH, FREETYPE_VERSION)
FREETYPE_SRC_PATH = "%s/freetype-%s" % (FREETYPE_ARCHIVE_EXTRACT_PATH, FREETYPE_VERSION)
FREETYPE_SRC_INCLUDE_PATH = "%s/include" % FREETYPE_SRC_PATH
FREETYPE_LIB_PATH = "%s/freetype-%s/objs/libfreetype.a" % (FREETYPE_ARCHIVE_EXTRACT_PATH, FREETYPE_VERSION)

# Information about spdlog dependency
SPDLOG_DOWNLOAD_URL = "https://github.com/gabime/spdlog/archive/refs/tags/v%s.tar.gz" % SPDLOG_VERSION
SPDLOG_ARCHIVE_PATH = "%s/spdlog-%s.tar.gz" % (WORKING_PATH, SPDLOG_VERSION)
SPDLOG_ARCHIVE_EXTRACT_PATH = "%s/spdlog-%s" % (WORKING_PATH, SPDLOG_VERSION)
SPDLOG_SRC_PATH = "%s/spdlog-%s" % (SPDLOG_ARCHIVE_EXTRACT_PATH, SPDLOG_VERSION)
SPDLOG_SRC_INCLUDE_PATH = "%s/include" % SPDLOG_SRC_PATH
SPDLOG_SRC_BUILD_PATH = "%s/build" % SPDLOG_SRC_PATH
SPDLOG_LIB_PATH = (
    "%s/Release/%s" % (SPDLOG_SRC_BUILD_PATH, PROJECT_SPDLOG_LIB_NAME)
    if sys.platform.startswith("win") else
    "%s/%s" % (SPDLOG_SRC_BUILD_PATH, PROJECT_SPDLOG_LIB_NAME)
)

def main():
    print("Getting Unilevel dependencies.")
    get_raylib()
    get_imgui()
    get_rlImGui()
    get_freetype()
    get_spdlog()
    print("Done: All dependencies are ready.")

def download_file(output_path, url):
    if os.path.exists(output_path):
        print("Found existing file download: %s" % url)
    else:
        print("Downloading file from %s" % url)
        response = urllib.request.urlopen(url)
        os.makedirs(os.path.dirname(output_path), exist_ok=True)
        with open(output_path, "wb") as archive_file:
            archive_file.write(response.read())
        response.close()

def unpack_file(archive_path, output_path):
    if os.path.exists(output_path):
        print("Found existing directory from extracted archive: %s" %
            output_path
        )
    else:
        print("Extracting archive %s" % archive_path)
        os.makedirs(os.path.dirname(output_path), exist_ok=True)
        shutil.unpack_archive(archive_path, output_path)

def copy_overwrite_file(src_path, dst_path):
    print("Copying file from %s to %s" % (src_path, dst_path))
    if os.path.exists(dst_path):
        os.remove(dst_path)
    else:
        os.makedirs(os.path.dirname(dst_path), exist_ok=True)
    shutil.copyfile(src_path, dst_path)

def copy_overwrite_dir_ext_files(
    src_dir_path,
    dst_dir_path,
    ext_list,
    recursive=False,
):
    ext_str = ", ".join(map(lambda ext: "*" + ext, ext_list))
    print("Copying %s files from %s to %s" % (
        ext_str, src_dir_path, dst_dir_path
    ))
    if recursive:
        for root, dirs, files in os.walk(src_dir_path):
            src_dir_name = os.path.relpath(root, src_dir_path)
            for src_file_name in files:
                if any(src_file_name.endswith(ext) for ext in ext_list):
                    src_file_path = os.path.join(root, src_file_name)
                    dst_file_path = os.path.join(dst_dir_path, src_dir_name, src_file_name)
                    copy_overwrite_file(src_file_path, dst_file_path)
    else:
        for src_file_name in os.listdir(src_dir_path):
            if any(src_file_name.endswith(ext) for ext in ext_list):
                src_file_path = os.path.join(src_dir_path, src_file_name)
                dst_file_path = os.path.join(dst_dir_path, src_file_name)
                copy_overwrite_file(src_file_path, dst_file_path)

def make_lib(lib_path, make_path, make_commands=["make"]):
    if os.path.exists(lib_path):
        print("Found lib file already built: %s" % lib_path)
    else:
        print("Building lib %s from source" % lib_path)
        for command in make_commands:
            print("$", command)
            make_process = subprocess.Popen(
                command,
                cwd=make_path,
                shell=True,
            )
            make_process.communicate()

def get_raylib():
    print("Downloading and building raylib %s" % RAYLIB_VERSION)
    # Get raylib
    download_file(RAYLIB_ARCHIVE_PATH, RAYLIB_DOWNLOAD_URL)
    unpack_file(RAYLIB_ARCHIVE_PATH, RAYLIB_ARCHIVE_EXTRACT_PATH)
    # Add "Raylib" or "RAYLIB_" prefix to declarations
    raylib_rename.main_raylib(RAYLIB_ROOT_PATH)
    # Build raylib
    make_lib(RAYLIB_LIB_PATH, RAYLIB_SRC_PATH, [
        "make PLATFORM=RAYLIB_PLATFORM_DESKTOP",
    ])
    # Copy raylib files to appropriate places
    copy_overwrite_file(RAYLIB_LIB_PATH, PROJECT_RAYLIB_LIB_PATH)
    copy_overwrite_dir_ext_files(
        RAYLIB_SRC_PATH,
        PROJECT_RAYLIB_INCLUDE_PATH,
        [".h"],
    )
    print("Done: raylib %s is ready." % RAYLIB_VERSION)

def get_imgui():
    print("Downloading imgui %s" % IMGUI_VERSION)
    download_file(IMGUI_ARCHIVE_PATH, IMGUI_DOWNLOAD_URL)
    unpack_file(IMGUI_ARCHIVE_PATH, IMGUI_ARCHIVE_EXTRACT_PATH)
    copy_overwrite_dir_ext_files(
        IMGUI_SRC_PATH,
        PROJECT_IMGUI_INCLUDE_PATH,
        [".h", ".cpp"],
    )
    copy_overwrite_dir_ext_files(
        IMGUI_SRC_FREETYPE_PATH,
        PROJECT_IMGUI_INCLUDE_FREETYPE_PATH,
        [".h", ".cpp"],
    )
    print("Done: imgui %s is ready." % IMGUI_VERSION)
    
def get_rlImGui():
    print("Downloading rlImGui %s" % RLIMGUI_VERSION_SHORT)
    # Get rlImGui
    download_file(RLIMGUI_ARCHIVE_PATH, RLIMGUI_DOWNLOAD_URL)
    unpack_file(RLIMGUI_ARCHIVE_PATH, RLIMGUI_ARCHIVE_EXTRACT_PATH)
    # Update code to account for newly prefixed raylib declarations
    raylib_rename.main_dep(RAYLIB_ROOT_PATH, [
        os.path.join(RLIMGUI_SRC_PATH, "rlImGui.cpp"),
        os.path.join(RLIMGUI_SRC_PATH, "rlImGui.h"),
        os.path.join(RLIMGUI_SRC_PATH, "rlImGuiColors.h"),
    ])
    # Copy rlImGui files to appropriate places
    copy_overwrite_dir_ext_files(
        RLIMGUI_SRC_PATH,
        PROJECT_RLIMGUI_INCLUDE_PATH,
        [".h", ".cpp"],
    )
    copy_overwrite_dir_ext_files(
        RLIMGUI_SRC_EXTRAS_PATH,
        PROJECT_RLIMGUI_INCLUDE_EXTRAS_PATH,
        [".h", ".cpp"],
    )
    print("Done: rlImGui %s is ready." % RLIMGUI_VERSION_SHORT)

def get_freetype():
    print("Downloading and building freetype %s" % FREETYPE_VERSION)
    # Get freetype
    download_file(FREETYPE_ARCHIVE_PATH, FREETYPE_DOWNLOAD_URL)
    unpack_file(FREETYPE_ARCHIVE_PATH, FREETYPE_ARCHIVE_EXTRACT_PATH)
    # Build freetype
    make_lib(FREETYPE_LIB_PATH, FREETYPE_SRC_PATH, [
        "make setup ansi",
        "make",
    ])
    # Copy freetype files to appropriate places
    copy_overwrite_file(FREETYPE_LIB_PATH, PROJECT_FREETYPE_LIB_PATH)
    copy_overwrite_dir_ext_files(
        FREETYPE_SRC_INCLUDE_PATH,
        PROJECT_INCLUDE_PATH,
        [".h"],
        recursive=True,
    )
    print("Done: freetype %s is ready." % FREETYPE_VERSION)

def get_spdlog():
    print("Downloading and building spdlog %s" % SPDLOG_VERSION)
    download_file(SPDLOG_ARCHIVE_PATH, SPDLOG_DOWNLOAD_URL)
    unpack_file(SPDLOG_ARCHIVE_PATH, SPDLOG_ARCHIVE_EXTRACT_PATH)
    os.makedirs(SPDLOG_SRC_BUILD_PATH, exist_ok=True)
    make_lib(SPDLOG_LIB_PATH, SPDLOG_SRC_BUILD_PATH, [
        "cmake ..",
        "cmake --build . --config Release",
    ])
    copy_overwrite_file(SPDLOG_LIB_PATH, PROJECT_SPDLOG_LIB_PATH)
    copy_overwrite_dir_ext_files(
        SPDLOG_SRC_INCLUDE_PATH,
        PROJECT_INCLUDE_PATH,
        [".h"],
        recursive=True,
    )
    print("Done: spdlog %s is ready." % SPDLOG_VERSION)

if __name__ == "__main__":
    main()
