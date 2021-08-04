#!/bin/bash

# I changed my build system to CMake so it could compile my shaders for me 
# To my dismay, It CANNOT do that.
# I've even tried to get it to run this bash scrip automagically, but guess what?
# It didn't do shit.

# Vertex
for shader in *.vert; do
	../bin/shaderc -f "$shader" --type v -o "${shader%.*}.bin"
done
# Frag
for shader in *.frag; do
	../bin/shaderc -f "$shader" --type f -o "${shader%.*}.bin"
done