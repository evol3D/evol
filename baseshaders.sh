#!/bin/sh

rm -r "$2/BaseShaders"
cp -r "$1/Application/Renderer/BaseShaders" "$2"
cd "$2/BaseShaders"

for file in *.glsl
do
    glslangValidator -V $file -o "$(basename -s .glsl $file).spv"
done
