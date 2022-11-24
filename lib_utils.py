#!/usr/bin/env python3

import json

lib_name = 'godot_qoi'
src_folder = lib_name

def setup_options(env, opts, gen_help):
    from SCons.Variables import BoolVariable, EnumVariable

    #opts.Add(BoolVariable("godot_qoi_livepp", "Live++ support... Windows only", False))
    opts.Update(env)

    gen_help(env)

def setup_default_cpp_defines(env):
    pass
    #if env['godot_remote_no_default_resources']:
    #    env.Append(CPPDEFINES=['NO_RESOURCES'])

def gdnative_get_sources(src):
    return [src_folder + "/" + file for file in src]

def gdnative_get_library_object(env):
    setup_default_cpp_defines(env)
    env.Append(CPPDEFINES=['GDEXTENSION_LIBRARY'])

    env.Append(CPPPATH=[src_folder])

    #######################################################
    # library definition

    src = []
    with open(src_folder + '/default_sources.json') as f:
        src = json.load(f)

    env["SHOBJPREFIX"] = "#obj/"

    library = env.SharedLibrary(
        "bin/" + lib_name + ".{}.{}.{}{}".format(
            env["platform"], env["target"], env["arch"], env["SHLIBSUFFIX"]
        ),
        source=gdnative_get_sources(src),
        SHLIBSUFFIX=env["SHLIBSUFFIX"]
    )

    return library