cmake_minimum_required(VERSION 3.20.0)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

find_package(Zephyr)
project(avionics)

include_directories(lib)
set(SOURCES
  src/main.c 
  lib/gps/gps.c
  lib/gps/gps.h
)

target_sources(app PRIVATE ${SOURCES})
