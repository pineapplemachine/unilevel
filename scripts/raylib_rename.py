help_text = """
This is a helper script which may be used to rename
declarations in the raylib library.

Unfortunately, raylib uses some function and struct names
which are likely to conflict with other libraries,
such as the struct type "Rectangle".

This script solves the problem by adding a "Raylib" prefix
to all functions and structs not already prefixed with "rl"
and a "RAYLIB_" prefix to all defines, macros, and enum
values not already prefixed with "RL_".

You can perform this prefix renaming in the raylib source
code by using this script like so:

```
python scripts/raylib_rename.py raylib \
    [path to raylib directory]
```

The script can also be used to update references to raylib
declarations in some existing source code, like so:

```
python scripts/raylib_rename.py dep \
    [path to raylib directory] \
    [one or more source file glob patterns...]
```
"""

import argparse
import glob
import json
import os
import re
import shutil
import sys

# Prefix functions and types with this string
ID_PREFIX_LOWER = "Raylib"
# Prefix defines and enum members with this string
ID_PREFIX_UPPER = "RAYLIB_"

ID_EXCEPTIONS = {
    "__declspec": None,
    "fopen": None,
    "MAX_PATH": None,
    "PLATFORM_DESKTOP": "RAYLIB_PLATFORM_DESKTOP",
    "PLATFORM_ANDROID": "RAYLIB_PLATFORM_ANDROID",
    "PLATFORM_RPI": "RAYLIB_PLATFORM_RPI",
    "PLATFORM_DRM": "RAYLIB_PLATFORM_DRM",
    "PLATFORM_WEB": "RAYLIB_PLATFORM_WEB",
    "SUPPORT_DEFAULT_FONT": "RAYLIB_SUPPORT_DEFAULT_FONT",
    "SUPPORT_CAMERA_SYSTEM": "RAYLIB_SUPPORT_CAMERA_SYSTEM",
    "SUPPORT_GESTURES_SYSTEM": "RAYLIB_SUPPORT_GESTURES_SYSTEM",
    "SUPPORT_MOUSE_GESTURES": "RAYLIB_SUPPORT_MOUSE_GESTURES",
    "SUPPORT_TOUCH_AS_MOUSE": "RAYLIB_SUPPORT_TOUCH_AS_MOUSE",
    "SUPPORT_SSH_KEYBOARD_RPI": "RAYLIB_SUPPORT_SSH_KEYBOARD_RPI",
    "SUPPORT_BUSY_WAIT_LOOP": "RAYLIB_SUPPORT_BUSY_WAIT_LOOP",
    "SUPPORT_PARTIALBUSY_WAIT_LOOP": "RAYLIB_SUPPORT_PARTIALBUSY_WAIT_LOOP",
    "SUPPORT_EVENTS_WAITING": "RAYLIB_SUPPORT_EVENTS_WAITING",
    "SUPPORT_SCREEN_CAPTURE": "RAYLIB_SUPPORT_SCREEN_CAPTURE",
    "SUPPORT_GIF_RECORDING": "RAYLIB_SUPPORT_GIF_RECORDING",
    "SUPPORT_COMPRESSION_API": "RAYLIB_SUPPORT_COMPRESSION_API",
    "SUPPORT_EVENTS_AUTOMATION": "RAYLIB_SUPPORT_EVENTS_AUTOMATION",
    # "RL_MALLOC": "RAYLIB_MALLOC",
    # "RL_CALLOC": "RAYLIB_CALLOC",
    # "RL_REALLOC": "RAYLIB_REALLOC",
    # "RL_FREE": "RAYLIB_FREE",
    # "RL_COLOR_TYPE": "RAYLIB_COLOR_TYPE",
    # "RL_RECTANGLE_TYPE": "RAYLIB_RECTANGLE_TYPE",
    # "RL_VECTOR2_TYPE": "RAYLIB_VECTOR2_TYPE",
    # "RL_VECTOR3_TYPE": "RAYLIB_VECTOR3_TYPE",
    # "RL_VECTOR4_TYPE": "RAYLIB_VECTOR4_TYPE",
    # "RL_QUATERNION_TYPE": "RAYLIB_QUATERNION_TYPE",
    # "RL_MATRIX_TYPE": "RAYLIB_MATRIX_TYPE",
}
ID_PREFIX_EXCLUSIONS = [
    "_GLFW",
    "_glfw",
    "_WIN",
    "gl",
    "GL_",
    "glad_",
    "GLAD_",
    "glfw",
    "GLFW_",
    "rl",
    "RL_",
    "STB_",
    "STBI_",
    "WGL_",
]

RE_META_CHARS = (
    "\\", "|", "*", ".", "?", "^", "$",
    "(", ")", "[", "]", "{", "}",
)

RE_DEFINE = (
    r'(?m)^(\s*)#define ([^\s\(]+)'
)
RE_DEFINE_REPL = (
    r'\1#define \2'
)
RE_TYPEDEF_STRUCT = (
    r'(?s)typedef struct ([^\s]+) (\{.+?\} )?(\1);'
)
RE_TYPEDEF_STRUCT_REPL = (
    r'typedef struct \1 \2\1;'
)
RE_TYPEDEF_ENUM = (
    r'(?s)typedef enum (\{.+?\}) ([^\s]+);'
)
RE_TYPEDEF_ENUM_REPL = (
    r'typedef enum \1 \2;'
)
RE_TYPEDEF_ENUM_MEMBER = (
    r'(?m)^(\s*)([A-Z0-9_][A-Za-z0-9_]+)(\s*(=.+?)?,?)'
)
RE_TYPEDEF_ENUM_MEMBER_REPL = (
    r'\1\2\3'
)
RE_TYPEDEF_CALLBACK = (
    r'typedef (.+?) (\**)\(\*([^\s\(]+)\)\('
)
RE_TYPEDEF_CALLBACK_REPL = (
    r'typedef \1 \2(*\3)('
)
RE_API_FUNCTION = (
    r'(R[LM]API) (.+?) (\**)([^\s\(]+)\('
)
RE_API_FUNCTION_REPL = (
    r'\1 \2 \3\4('
)

def __main__():
    command = sys.argv[1] if len(sys.argv) > 1 else None
    if not command or command in ("--help", "help", "usage"):
        print(help_text)
    elif command == "raylib":
        main_raylib(sys.argv[2])
    elif command == "dep":
        main_dep(sys.argv[2], sys.argv[3:])

def main_raylib(raylib_dir_path):
    print("Adding prefixes to raylib declaration names in directory ", raylib_dir_path)
    prefixer = RaylibPrefixer()
    raylib_id_json_path = os.path.join(raylib_dir_path, "renamed_identifiers.json")
    raylib_src = os.path.join(raylib_dir_path, "src")
    prefixer.read_raylib_src_files(raylib_src)
    prefixer.rename_declarations()
    prefixer.rename_identifiers()
    prefixer.write_identifiers(raylib_id_json_path)
    prefixer.write_raylib_src_files(raylib_src)

def main_dep(raylib_dir_path, dep_glob_patterns):
    print("Adding prefixes to raylib references in paths", dep_glob_patterns)
    prefixer = RaylibPrefixer()
    raylib_id_json_path = os.path.join(raylib_dir_path, "renamed_identifiers.json")
    prefixer.read_identifiers(raylib_id_json_path)
    prefixer.rename_identifiers_in_src_files(dep_glob_patterns)

class RaylibPrefixer:
    def __init__(self):
        self.rl_src_files = {}
        self.identifier_map = {}
        for id_old, id_new in ID_EXCEPTIONS.items():
            if id_new:
                self.identifier_map[id_old] = id_new
    
    def add_identifier(self, identifier, prefix=ID_PREFIX_LOWER):
        if identifier in ID_EXCEPTIONS:
            return ID_EXCEPTIONS[identifier] or identifier
        if identifier.startswith(prefix) or any(
            identifier.startswith(pre) for pre in ID_PREFIX_EXCLUSIONS
        ):
            return identifier
        for meta_char in RE_META_CHARS:
            if meta_char in identifier:
                raise ValueError("Invalid identifier %s" % identifier)
        prefixed_identifier = prefix + identifier
        if identifier in self.identifier_map and (
            self.identifier_map[identifier] != prefixed_identifier
        ):
            raise ValueError("Identifier conflict: %s and %s" % (
                self.identifier_map[identifier],
                prefixed_identifier,
            ))
        self.identifier_map[identifier] = prefixed_identifier
        return prefixed_identifier
    
    def read_identifiers(self, json_path):
        with open(json_path, "rt", encoding="utf-8") as json_file:
            self.identifier_map = json.load(json_file)
        if not isinstance(self.identifier_map, dict):
            raise ValueError("Expected %s to contain a JSON object." % json_path)
    
    def write_identifiers(self, json_path):
        with open(json_path, "wt", encoding="utf-8") as json_file:
            json.dump(self.identifier_map, json_file)
    
    def read_raylib_src_files(self, src_dir_path):
        src_dir_original_path = src_dir_path + "_original"
        if not os.path.exists(src_dir_original_path):
            print("Copying raylib src files from %s to %s" % (
                src_dir_path, src_dir_original_path
            ))
            shutil.copytree(src_dir_path, src_dir_original_path)
        file_names = list(os.listdir(src_dir_original_path))
        file_names.append("external/rl_gputex.h")
        for file_name in file_names:
            if not (
                file_name == "build.zig" or
                file_name == "CMakeLists.txt" or
                file_name == "Makefile" or
                file_name.endswith(".h") or
                file_name.endswith(".c")
            ):
                continue
            file_path = os.path.join(src_dir_original_path, file_name)
            print("Reading original raylib src file:", file_path)
            with open(file_path, "rt", encoding="utf-8") as src_file:
                self.rl_src_files[file_name] = src_file.read()
    
    def write_raylib_src_files(self, src_dir_path):
        for file_name in self.rl_src_files.keys():
            file_path = os.path.join(src_dir_path, file_name)
            print("Writing modified raylib src file:", file_path)
            with open(file_path, "wt", encoding="utf-8") as src_file:
                src_file.write(self.rl_src_files[file_name])
    
    def rename_identifiers_in_src_files(self, src_glob_patterns):
        visited_files = set()
        for pattern in src_glob_patterns:
            for file_path in glob.glob(pattern):
                if file_path in visited_files:
                    continue
                visited_files.add(file_path)
                original_file_path = file_path + ".rloriginal"
                if not os.path.exists(original_file_path):
                    print("Copying dependent file from %s to %s", (
                        file_path, original_file_path
                    ))
                    shutil.copyfile(file_path, original_file_path)
                print("Reading original raylib dependent src file:", original_file_path)
                with open(original_file_path, "rt", encoding="utf-8") as read_file:
                    src = read_file.read()
                src = self.rename_identifiers_src(src)
                print("Writing modified raylib dependent src file:", file_path)
                with open(file_path, "wt", encoding="utf-8") as write_file:
                    write_file.write(src)
    
    def rename_declarations(self):
        for file_name in self.rl_src_files.keys():
            if not file_name.endswith(".h"):
                continue
            self.rl_src_files[file_name] = (
                self.rename_decls_src(self.rl_src_files[file_name])
            )
    
    def rename_identifiers(self):
        for file_name in self.rl_src_files.keys():
            self.rl_src_files[file_name] = (
                self.rename_identifiers_src(self.rl_src_files[file_name])
            )
            
    def make_repl_fn(self, repl_str, id_groups, group_map_fns=None, id_prefix=ID_PREFIX_LOWER):
        def repl_fn(match):
            repl_str_result = repl_str
            for i in range(1, 1 + len(match.groups())):
                group_str = match.group(i) or ""
                if i in id_groups:
                    group_str = self.add_identifier(group_str, id_prefix)
                if group_map_fns and group_map_fns.get(i):
                    group_str = group_map_fns[i](group_str)
                repl_str_result = repl_str_result.replace(
                    r'\%d' % i, group_str
                )
            return repl_str_result
        return repl_fn
    
    def enum_decl_repl_map_fn(self, enum_src):
        return re.sub(
            RE_TYPEDEF_ENUM_MEMBER,
            self.make_repl_fn(RE_TYPEDEF_ENUM_MEMBER_REPL, [2], id_prefix=ID_PREFIX_UPPER),
            enum_src,
        )
    
    def rename_decls_src(self, src):
        src_renamed = src
        src_renamed = re.sub(
            RE_DEFINE,
            self.make_repl_fn(RE_DEFINE_REPL, [2], id_prefix=ID_PREFIX_UPPER),
            src_renamed,
        )
        src_renamed = re.sub(
            RE_TYPEDEF_STRUCT,
            self.make_repl_fn(RE_TYPEDEF_STRUCT_REPL, [1]),
            src_renamed,
        )
        src_renamed = re.sub(
            RE_TYPEDEF_ENUM,
            self.make_repl_fn(RE_TYPEDEF_ENUM_REPL, [2], {1: self.enum_decl_repl_map_fn}),
            src_renamed,
        )
        src_renamed = re.sub(
            RE_TYPEDEF_CALLBACK,
            self.make_repl_fn(RE_TYPEDEF_CALLBACK_REPL, [3]),
            src_renamed,
        )
        src_renamed = re.sub(
            RE_API_FUNCTION,
            self.make_repl_fn(RE_API_FUNCTION_REPL, [4]),
            src_renamed,
        )
        return src_renamed
    
    def rename_identifiers_src(self, src):
        re_pattern = (
            r'\b(%s)\b' % "|".join(self.identifier_map.keys())
        )
        def id_repl(match):
            pos = match.start(0)
            if src[pos - 1] == "." or src[pos - 2:pos] in ("->", "::"):
                return match.group(0)
            else:
                return self.identifier_map.get(match.group(0), "")
        return re.sub(
            re_pattern,
            id_repl,
            src,
        )

if __name__ == "__main__":
    __main__()
