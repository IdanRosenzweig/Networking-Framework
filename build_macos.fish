#!/usr/bin/env fish

set build_dir build_macos

if not test -d $build_dir
    mkdir $build_dir
end

if not test -f "$build_dir/CMakeCache.txt"
    echo "configuring cmake"
    cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_OS="macos" -S . -B $build_dir
end

if count $argv > /dev/null
    echo "building target $argv"
    cmake --build $build_dir --target $argv
else
    echo "no target specific to build"
end
