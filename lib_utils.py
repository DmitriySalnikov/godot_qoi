#!/usr/bin/env python3

import json
from patches import unity_tools
from pathlib import Path

lib_name = "godot_qoi"
output_dir = Path("addons") / lib_name / "libs"
src_folder = "src"


def setup_options(env, arguments, gen_help):
    from SCons.Variables import Variables, BoolVariable, EnumVariable, PathVariable
    opts = Variables([], arguments)

    opts.Add(BoolVariable("lto", "Link-time optimization", False))
    opts.Add(PathVariable("addon_output_dir", "Path to the output directory",
             output_dir, PathVariable.PathIsDirCreate))
    opts.Update(env)

    gen_help(env, opts)


def gdnative_setup_defines_and_flags(env):
    env.Append(CPPDEFINES=["GDEXTENSION_LIBRARY"])

    if env["lto"]:
        if env.get("is_msvc", False):
            env.AppendUnique(CCFLAGS=["/GL"])
            env.AppendUnique(ARFLAGS=["/LTCG"])
            env.AppendUnique(LINKFLAGS=["/LTCG"])
        else:
            env.AppendUnique(CCFLAGS=["-flto"])
            env.AppendUnique(LINKFLAGS=["-flto"])


def gdnative_get_sources(src):
    res = [src_folder + "/" + file for file in src]
    res = unity_tools.generate_unity_build(res, "qoi_")

    return res


def gdnative_get_library_object(env, arguments=None, gen_help=None):
    if arguments != None and gen_help:
        setup_options(env, arguments, gen_help)
    gdnative_setup_defines_and_flags(env)

    env.Append(CPPPATH=[src_folder])

    src = []
    with open(src_folder + "/default_sources.json") as f:
        src = json.load(f)

    # store all obj's in a dedicated folder
    env["SHOBJPREFIX"] = "#obj/"

    # some additional tags
    additional_tags = ""

    library_full_name = "lib" + lib_name + ".{}.{}.{}{}{}".format(
        env["platform"], env["target"], env["arch"], additional_tags,env["SHLIBSUFFIX"])

    env.Default(env.SharedLibrary(
        target=env.File(Path(env["addon_output_dir"]) / library_full_name),
        source=gdnative_get_sources(src),
        SHLIBSUFFIX=env["SHLIBSUFFIX"]
    ))

    # Needed for easy reuse of this library in other build scripts
    # TODO: not tested at the moment. Probably need some changes in the C++ code
    env = env.Clone()
    env.Append(LIBPATH=[env.Dir(env["addon_output_dir"])])
    if env.get("is_msvc", False):
        env.Append(LIBS=[library_full_name.replace(".dll", ".lib")])
    else:
        env.Append(LIBS=[library_full_name])

    return env
