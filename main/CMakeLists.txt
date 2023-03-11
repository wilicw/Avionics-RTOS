set(srcs "main.c" "task/sensors.c" "task/commu.c" "task/wdt.c" "task/logger.c")
set(incs "." "task")
idf_component_register(SRCS "${srcs}" INCLUDE_DIRS "${incs}")
