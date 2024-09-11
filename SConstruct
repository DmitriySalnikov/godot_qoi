#!/usr/bin/env python

from SCons.Script import SConscript
from SCons.Script.SConscript import SConsEnvironment

import SCons, SCons.Script
import sys, os, platform
import lib_utils, lib_utils_external

# Fixing the encoding of the console
if platform.system() == "Windows":
    os.system("chcp 65001")

# Project config
project_name = "Godot QOI"
lib_name = "godot_qoi"
default_output_dir = os.path.join("addons", "godot_qoi", "libs")
src_folder = "src"

# If necessary, add patches from the code
patches_to_apply = [
    "patches/godot_cpp_exclude_unused_classes.patch",  # Removes unused godot-cpp classes from the build process
    "patches/unity_build.patch",  # Speeds up the build by merging the source files. It can increase the size of assemblies.
    "patches/web_threads.patch",  # Adds the build flag that appeared in Godot 4.3. Required for a web build compatible with Godot 4.3.
    "patches/big_int_fix.patch",  # Fixes runtime link errors
]

print(
    f"If you add new source files (e.g. .cpp, .c), do not forget to specify them in '{src_folder}/default_sources.json'.\n\tOr add them to 'setup_defines_and_flags' inside 'SConstruct'."
)
print("To apply git patches, use 'scons apply_patches'.")
# print("To build cmake libraries, use 'scons build_cmake'.")


# Additional console arguments
def setup_options(env: SConsEnvironment, arguments):
    from SCons.Variables import Variables, BoolVariable, EnumVariable, PathVariable

    opts = Variables([], arguments)

    # It must be here for lib_utils.py
    opts.Add(
        PathVariable(
            "addon_output_dir", "Path to the output directory", default_output_dir, PathVariable.PathIsDirCreate
        )
    )

    opts.Add(BoolVariable("telemetry_enabled", "Enable the telemetry module", False))
    opts.Add(BoolVariable("lto", "Link-time optimization", False))

    opts.Update(env)
    env.Help(opts.GenerateHelpText(env))


# Additional compilation flags
def setup_defines_and_flags(env: SConsEnvironment, src_out):
    # Add more sources to `src_out` if needed

    if env["telemetry_enabled"]:
        tele_src = "my_telemetry_modules/GDExtension/usage_time_reporter.cpp"
        if os.path.exists(os.path.join(src_folder, tele_src)):
            env.Append(CPPDEFINES=["TELEMETRY_ENABLED", "TELEMETRY_PROJECT_GQOI"])
            src_out.append(tele_src)
            print("Compiling with telemetry support!")
        else:
            print("No telemetry source file found.")
            env.Exit(1)

    if env["lto"]:
        if env.get("is_msvc", False):
            env.AppendUnique(CCFLAGS=["/GL"])
            env.AppendUnique(ARFLAGS=["/LTCG"])
            env.AppendUnique(LINKFLAGS=["/LTCG"])
        else:
            env.AppendUnique(CCFLAGS=["-flto"])
            env.AppendUnique(LINKFLAGS=["-flto"])

    if env.get("is_msvc", False):
        env.Append(LINKFLAGS=["/WX:NO"])

    if env["platform"] in ["linux"]:  # , "android"?
        env.Append(
            LINKFLAGS=[
                "-static-libgcc",
                "-static-libstdc++",
            ]
        )
    if env["platform"] == "android":
        env.Append(
            LIBS=[
                "log",
            ]
        )
    print()


def generate_sources_for_resources(env, src_out):
    # Array of (path, is_text)
    shared_files = [
        ("src/resources/extendable_meshes.gdshader", True),
    ]
    lib_utils.generate_resources_cpp_h_files(shared_files, "GQOI", src_folder, "shared_resources.gen", src_out)

    print("The generation of C++ sources with the contents of resources has been completed")
    print()


def apply_patches(target, source, env: SConsEnvironment):
    return lib_utils_external.apply_git_patches(env, patches_to_apply, "godot-cpp")


def get_android_toolchain() -> str:
    sys.path.insert(0, "godot-cpp/tools")
    import android

    sys.path.pop(0)
    return os.path.join(android.get_android_ndk_root(env), "build/cmake/android.toolchain.cmake")


# Additional build of the projects via CMake
# def build_cmake(target, source, env: SConsEnvironment):
#    extra_flags = []
#    if env["platform"] in ["macos", "ios"]:
#        extra_flags += ["-DCMAKE_OSX_ARCHITECTURES=arm64;x86_64", "-DCMAKE_OSX_DEPLOYMENT_TARGET=10.14"]
#    if env["platform"] in ["android"]:
#        extra_flags += [f"-DCMAKE_TOOLCHAIN_FILE={get_android_toolchain()}"]
#    return lib_utils_external.cmake_build_project(env, "project", extra_flags)

env: SConsEnvironment = SConscript("godot-cpp/SConstruct")
env = env.Clone()

args = ARGUMENTS
additional_src = []
setup_options(env, args)
setup_defines_and_flags(env, additional_src)
#generate_sources_for_resources(env, additional_src)

extra_tags = ""

lib_utils.get_library_object(
    env, project_name, lib_name, extra_tags, env["addon_output_dir"], src_folder, additional_src
)

# Register console commands
env.Command("apply_patches", [], apply_patches)
# env.Command("build_cmake", [], build_cmake)
