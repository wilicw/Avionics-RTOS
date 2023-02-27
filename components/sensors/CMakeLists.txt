set(srcs "bmp280.c" "gps.c")
idf_component_register(SRCS "${srcs}" INCLUDE_DIRS "." REQUIRES bsp)
