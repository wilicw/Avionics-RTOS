set(srcs "main.c" "task/sensors.c" "task/wdt.c" "task/logger.c" "task/fsm.c" "task/recv.c")
set(incs "." "task")
idf_component_register(SRCS "${srcs}" INCLUDE_DIRS "${incs}")
