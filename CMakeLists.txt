cmake_minimum_required(VERSION 3.20.0)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

find_package(Zephyr)
project(my_zephyr_app)

target_sources(app PRIVATE src/main.c)
