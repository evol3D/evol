cmake_minimum_required(VERSION 3.1)

project(evol)

add_executable(evol
	src/main.c
)

target_include_directories(evol PUBLIC
	include
)
