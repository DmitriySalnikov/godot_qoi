#!/usr/bin/env python3

import os
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
    opts.Add(PathVariable("addon_output_dir", "Path to the output directory", output_dir, PathVariable.PathIsDirCreate))
    opts.Update(env)

    gen_help(env, opts)


def setup_defines_and_flags(env):
    env.Append(CPPDEFINES=["GDEXTENSION_LIBRARY"])

    if env["lto"]:
        if env.get("is_msvc", False):
            env.AppendUnique(CCFLAGS=["/GL"])
            env.AppendUnique(ARFLAGS=["/LTCG"])
            env.AppendUnique(LINKFLAGS=["/LTCG"])
        else:
            env.AppendUnique(CCFLAGS=["-flto"])
            env.AppendUnique(LINKFLAGS=["-flto"])


def is_file_locked(file_path):
    if not os.path.exists(file_path):
        return False
    try:
        with open(file_path, "a") as f:
            pass
    except IOError:
        return True
    return False


def msvc_pdb_rename(env, lib_full_name):
    new_name = (Path(env["addon_output_dir"]) / lib_full_name).as_posix()
    max_files = 256

    onlyfiles = [
        f
        for f in os.listdir(Path(env["addon_output_dir"]))
        if os.path.isfile(os.path.join(Path(env["addon_output_dir"]), f))
    ]
    for of in onlyfiles:
        if of.endswith(".pdb") and of.startswith(lib_full_name):
            try:
                os.remove(Path(env["addon_output_dir"]) / of)
            except:
                pass

    pdb_name = ""
    for s in range(max_files):
        pdb_name = "{}_{}.pdb".format(new_name, s)
        if not is_file_locked(pdb_name):
            break

    env.Append(LINKFLAGS=["/PDB:" + pdb_name])


def get_sources(src):
    res = [src_folder + "/" + file for file in src]
    res = unity_tools.generate_unity_build(res, "qoi_")

    return res


def replace_flag(arr, flag, new_flag):
    if flag in arr:
        arr.remove(flag)
    arr.append(new_flag)


def get_library_object(env, arguments=None, gen_help=None):
    if arguments != None and gen_help:
        setup_options(env, arguments, gen_help)
    setup_defines_and_flags(env)

    env.Append(CPPPATH=[src_folder])

    src = []
    with open(src_folder + "/default_sources.json") as f:
        src = json.load(f)

    # store all obj's in a dedicated folder
    env["SHOBJPREFIX"] = "#obj/"

    # some additional tags
    additional_tags = ""

    library_full_name = "lib{}.{}.{}.{}{}".format(
        lib_name, env["platform"], env["target"], env["arch"], additional_tags
    )

    # using the library with `reloadable = true` and with the debugger block the PDB file,
    # so it needs to be renamed to something not blocked
    if env.get("is_msvc", False) and env["target"] != "template_release":
        msvc_pdb_rename(env, library_full_name)

    env.Default(
        env.SharedLibrary(
            target=env.File(Path(env["addon_output_dir"]) / (library_full_name + env["SHLIBSUFFIX"])),
            source=get_sources(src),
            SHLIBSUFFIX=env["SHLIBSUFFIX"],
        )
    )

    # Needed for easy reuse of this library in other build scripts
    # TODO: not tested at the moment. Probably need some changes in the C++ code
    env = env.Clone()
    env.Append(LIBPATH=[env.Dir(env["addon_output_dir"])])
    if env.get("is_msvc", False):
        env.Append(LIBS=[library_full_name.replace(".dll", ".lib")])
    else:
        env.Append(LIBS=[library_full_name])

    return env
