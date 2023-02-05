cmake_minimum_required(VERSION 3.20.0)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")
set(CMAKE_C_FLAGS "-std=gnu11")

find_package(Zephyr)
project(avionics)

include_directories(lib)
include_directories(inc)
set(SOURCES
  src/main.c 
  src/sensors.c
  lib/gps/gps.c
  lib/pid/pid.c
)

target_sources(app PRIVATE ${SOURCES})
