#!/usr/bin/env bash
cd godot-cpp
cpu=$(grep ^cpu\\scores /proc/cpuinfo | uniq |  awk '{print $4}')
#api=custom_api_file="../api.json"
api= 

git apply ../godot_cpp_restrict_used_classes.patch

scons platform=linux target=release bits=64 -j$cpu $api
scons platform=linux target=debug bits=64 -j$cpu $api
